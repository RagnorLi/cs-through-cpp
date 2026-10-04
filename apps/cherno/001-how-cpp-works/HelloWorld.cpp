#include <cstdlib>
#include <iostream>

/*
clang++ -std=c++20 -E apps/cherno/HelloWorld.cpp -o HelloWorld.ii   # 第1步：预处理
clang++ -std=c++20 -S HelloWorld.ii -o HelloWorld.s                 # 第2步：编译成汇编
clang++ -c HelloWorld.s -o HelloWorld.o                             # 第3步：汇编成目标文件
clang++ HelloWorld.o -o hello_world                                 # 第4步：链接

-E：只做预处理就停下。预处理会把 #include <iostream> 替换成头文件的全部内容，把宏展开。输出的 .ii 是个很长的文本文件，你可以打开看看，里面你的代码在最末尾。
-S：编译到汇编就停下。.s 是汇编代码，可以用文本打开，里面是 CPU 指令的文字形式。
-c：编译到目标文件就停下。.o 是二进制的机器码，但还缺少库函数的地址，不能直接运行。
什么都不加：把 .o 和标准库等连起来，生成可执行文件。

*/


int main()
{
    std::cout << "预处理 .cpp - 编译 .ii - 汇编 .s - 链接 .o -> executable" << std::endl;
    std::cin.get();
    return EXIT_SUCCESS;
}

