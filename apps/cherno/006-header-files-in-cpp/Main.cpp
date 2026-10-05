/*
头文件的语言层面本质是供预处理器复制的文本；工程层面的主要职责是发布共享接口，而接口通常以声明为主。


// 重复函数声明：合法
void f(int);
void f(int);

// 不兼容声明：错误
void f(int);
int f(int); // 不能只通过返回值区分函数

// 不同参数：形成重载，合法
void f(int);
void f(double);

// 重复结构体前置声明：合法
struct A;
struct A;

// 重复结构体定义：错误
struct A {};
struct A {}; // redefinition
*/

#include "Log.h" // IWYU pragma: keep
#include "Log.h"

void runMain2(); // 故意直接声明，说明声明不一定非得写在 .h 中

int main()
{
    log("hello cpp");
    runMain2();
}