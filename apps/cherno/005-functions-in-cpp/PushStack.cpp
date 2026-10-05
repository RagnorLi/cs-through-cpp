/*
函数调用
= 代码区里的机器指令
+ 跳转指令
+ 寄存器传参
+ 必要时建立栈帧
+ 必要时访问全局数据
+ 必要时申请堆内存

这个实验不再预先把地址手写成“代码 / 数据 / 堆 / 栈”，而是查询：
1. macOS 为该地址建立的虚拟内存区域及其权限；
2. 地址是否落在当前可执行文件的某个 Mach-O 段 / 节中；
3. 地址是否落在当前线程的栈范围中；
4. malloc 返回的地址是否被 malloc zone 识别。

注意：下面使用了 macOS 专用 API，不是可移植 C++。
*/

#include <mach-o/dyld.h>
#include <mach-o/loader.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <malloc/malloc.h>
#include <pthread.h>
#include <unistd.h>

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

int         g_data = 1;
int         g_bss;
const char* g_str = "hi";

struct VmRegion {
    bool              found = false;
    mach_vm_address_t start = 0;
    mach_vm_size_t    size = 0;
    vm_prot_t         protection = VM_PROT_NONE;
    unsigned int      user_tag = 0;
};

struct MachLocation {
    bool           found = false;
    std::string    segment;
    std::string    section;
    std::uintptr_t segment_start = 0;
    std::uintptr_t segment_end = 0;
};

struct StackRange {
    std::uintptr_t low = 0;
    std::uintptr_t high = 0;
};

struct ProbeOptions {
    bool is_malloc_pointer = false;
};

std::string fixed_mach_name(const char* name) {
    return std::string(name, ::strnlen(name, 16));
}

VmRegion query_vm_region(std::uintptr_t target) {
    mach_vm_address_t cursor = static_cast<mach_vm_address_t>(target);
    mach_vm_size_t size = 0;
    natural_t depth = 0;

    while (true) {
        vm_region_submap_info_data_64_t info{};
        mach_msg_type_number_t count = VM_REGION_SUBMAP_INFO_COUNT_64;

        const kern_return_t result = mach_vm_region_recurse(
            mach_task_self(),
            &cursor,
            &size,
            &depth,
            reinterpret_cast<vm_region_recurse_info_t>(&info),
            &count);

        if (result != KERN_SUCCESS) {
            return {};
        }

        if (info.is_submap) {
            ++depth;
            continue;
        }

        const auto end = cursor + size;
        const bool contains = target >= cursor && target < end;
        return {
            .found = contains,
            .start = cursor,
            .size = size,
            .protection = info.protection,
            .user_tag = info.user_tag,
        };
    }
}

MachLocation query_main_image(std::uintptr_t target) {
    const auto* header = reinterpret_cast<const mach_header_64*>(
        _dyld_get_image_header(0));

    if (header == nullptr || header->magic != MH_MAGIC_64) {
        return {};
    }

    const auto slide = static_cast<std::uintptr_t>(
        _dyld_get_image_vmaddr_slide(0));

    const auto* command = reinterpret_cast<const load_command*>(
        reinterpret_cast<const std::uint8_t*>(header) + sizeof(mach_header_64));

    for (std::uint32_t command_index = 0;
         command_index < header->ncmds;
         ++command_index) {
        if (command->cmd == LC_SEGMENT_64) {
            const auto* segment = reinterpret_cast<const segment_command_64*>(command);
            const auto segment_start =
                static_cast<std::uintptr_t>(segment->vmaddr) + slide;
            const auto segment_end = segment_start + segment->vmsize;

            if (target >= segment_start && target < segment_end) {
                MachLocation location{
                    .found = true,
                    .segment = fixed_mach_name(segment->segname),
                    .section = "(不在具体 section 中)",
                    .segment_start = segment_start,
                    .segment_end = segment_end,
                };

                const auto* section = reinterpret_cast<const section_64*>(
                    reinterpret_cast<const std::uint8_t*>(segment) +
                    sizeof(segment_command_64));

                for (std::uint32_t section_index = 0;
                     section_index < segment->nsects;
                     ++section_index, ++section) {
                    const auto section_start =
                        static_cast<std::uintptr_t>(section->addr) + slide;
                    const auto section_end = section_start + section->size;

                    if (target >= section_start && target < section_end) {
                        location.section = fixed_mach_name(section->sectname);
                        break;
                    }
                }

                return location;
            }
        }

        command = reinterpret_cast<const load_command*>(
            reinterpret_cast<const std::uint8_t*>(command) + command->cmdsize);
    }

    return {};
}

StackRange current_stack_range() {
    const pthread_t thread = pthread_self();
    const auto high = reinterpret_cast<std::uintptr_t>(
        pthread_get_stackaddr_np(thread));
    const auto size = static_cast<std::uintptr_t>(
        pthread_get_stacksize_np(thread));

    return {
        .low = high - size,
        .high = high,
    };
}

bool contains(StackRange range, std::uintptr_t address) {
    return address >= range.low && address < range.high;
}

std::string permissions(vm_prot_t protection) {
    std::string result = "---";
    result[0] = (protection & VM_PROT_READ) ? 'r' : '-';
    result[1] = (protection & VM_PROT_WRITE) ? 'w' : '-';
    result[2] = (protection & VM_PROT_EXECUTE) ? 'x' : '-';
    return result;
}

const char* vm_tag_name(unsigned int tag) {
    switch (tag) {
        case VM_MEMORY_MALLOC:
            return "malloc";
        case VM_MEMORY_MALLOC_SMALL:
            return "malloc small";
        case VM_MEMORY_MALLOC_LARGE:
            return "malloc large";
        case VM_MEMORY_MALLOC_HUGE:
            return "malloc huge";
        case VM_MEMORY_MALLOC_TINY:
            return "malloc tiny";
        case VM_MEMORY_MALLOC_NANO:
            return "malloc nano";
        case VM_MEMORY_MALLOC_MEDIUM:
            return "malloc medium";
        case VM_MEMORY_STACK:
            return "thread stack";
        case VM_MEMORY_GUARD:
            return "guard page";
        case VM_MEMORY_DYLIB:
            return "dynamic library";
        default:
            return nullptr;
    }
}

std::string conclusion(const MachLocation& image,
                       bool in_current_stack,
                       bool malloc_recognized) {
    if (image.found) {
        if (image.section == "__text") {
            return "当前程序的机器指令";
        }
        if (image.section == "__cstring") {
            return "当前程序的字符串字面量";
        }
        if (image.section == "__data") {
            return "当前程序的已初始化全局 / 静态数据";
        }
        if (image.section == "__bss" || image.section == "__common") {
            return "当前程序的零初始化全局 / 静态数据";
        }
        return "当前程序 Mach-O 映像中的内容";
    }

    if (in_current_stack) {
        return "当前线程栈中的内容";
    }

    if (malloc_recognized) {
        return "malloc 管理的动态存储";
    }

    return "尚未判定";
}

void probe(const char* name,
           std::uintptr_t address,
           ProbeOptions options = {}) {
    const VmRegion vm = query_vm_region(address);
    const MachLocation image = query_main_image(address);
    const StackRange stack = current_stack_range();
    const bool in_current_stack = contains(stack, address);

    malloc_zone_t* zone = nullptr;
    std::size_t allocation_size = 0;
    if (options.is_malloc_pointer) {
        const auto* pointer = reinterpret_cast<const void*>(address);
        zone = malloc_zone_from_ptr(pointer);
        if (zone != nullptr) {
            allocation_size = malloc_size(pointer);
        }
    }

    std::printf("\n%s\n", name);
    std::printf("  地址       0x%016" PRIxPTR "\n", address);

    if (vm.found) {
        const auto vm_end = vm.start + vm.size;
        std::printf("  虚拟区域   [0x%016" PRIx64 ", 0x%016" PRIx64 ")\n",
                    static_cast<std::uint64_t>(vm.start),
                    static_cast<std::uint64_t>(vm_end));
        std::printf("  权限       %s\n", permissions(vm.protection).c_str());

        if (const char* tag = vm_tag_name(vm.user_tag); tag != nullptr) {
            std::printf("  VM 标签    %s (%u)\n", tag, vm.user_tag);
        }
    } else {
        std::printf("  虚拟区域   未找到包含该地址的映射\n");
    }

    if (image.found) {
        std::printf("  Mach-O     %s / %s\n",
                    image.segment.c_str(),
                    image.section.c_str());
        std::printf("  映像段     [0x%016" PRIxPTR ", 0x%016" PRIxPTR ")\n",
                    image.segment_start,
                    image.segment_end);
    }

    if (in_current_stack) {
        std::printf("  线程栈     [0x%016" PRIxPTR ", 0x%016" PRIxPTR ")，包含：是\n",
                    stack.low,
                    stack.high);
    }

    if (options.is_malloc_pointer) {
        std::printf("  malloc     zone 识别：%s",
                    zone != nullptr ? "是" : "否");
        if (zone != nullptr) {
            std::printf("，实际块大小：%zu 字节", allocation_size);
        }
        std::printf("\n");
    }

    std::printf("  判定       %s\n",
                conclusion(image, in_current_stack, zone != nullptr).c_str());
}

void callee() {
    int local = 0;

    probe("callee 返回后继续执行的指令",
          reinterpret_cast<std::uintptr_t>(__builtin_return_address(0)));
    probe("callee::local", reinterpret_cast<std::uintptr_t>(&local));
}

int main() {
    int local = 0;
    int* heap = static_cast<int*>(std::malloc(sizeof(int)));

    if (heap == nullptr) {
        std::fprintf(stderr, "malloc failed\n");
        return EXIT_FAILURE;
    }

    const StackRange stack = current_stack_range();

    std::printf("进程虚拟内存探针（PID %d）\n", getpid());
    std::printf("当前线程栈范围 [0x%016" PRIxPTR ", 0x%016" PRIxPTR ")\n",
                stack.low,
                stack.high);

    probe("callee 函数入口",
          reinterpret_cast<std::uintptr_t>(&callee));
    probe("字符串字面量 \"hi\"",
          reinterpret_cast<std::uintptr_t>(g_str));
    probe("g_str 指针变量自身",
          reinterpret_cast<std::uintptr_t>(&g_str));
    probe("g_data",
          reinterpret_cast<std::uintptr_t>(&g_data));
    probe("g_bss",
          reinterpret_cast<std::uintptr_t>(&g_bss));
    probe("malloc 返回的 int",
          reinterpret_cast<std::uintptr_t>(heap),
          {.is_malloc_pointer = true});
    probe("main::local",
          reinterpret_cast<std::uintptr_t>(&local));

    callee();

    std::free(heap);
    return EXIT_SUCCESS;
}
