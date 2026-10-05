#include <cstddef>

int main()
{
    /* c++ 标准中的基本数据类型
    
    基础类型
    ├── void
    ├── std::nullptr_t              （nullptr 的类型）
    └── 算术类型
        ├── 整型
        │   ├── bool
        │   ├── 字符型：char, signed char, unsigned char,
        │   │          wchar_t, char8_t(C++20), char16_t, char32_t
        │   └── 整数型：short, int, long, long long
        │              （每个都有 signed / unsigned 两种）
        └── 浮点型：float, double, long double
    
    */ 
    
    std::nullptr_t nptr = nullptr;

    bool b = true;
    char c = 'A';
    short s = 1;
    int i = 10;
    long l = 100;
    long long ll = 1000;

    float f = 3.14f;
    double d = 3.14;
    long double ld = 3.14l;

    size_t int_len = sizeof(int);

    

}