/*
预处理后的源码 (.ii)
  → 词法分析：切成一个个"词"（token），如 int、x、=、1、+、2
  → 语法分析：按语法规则把 token 组装成 AST
  → 语义分析：检查类型对不对、变量有没有声明、实例化模板
  → 生成中间代码（Clang 用的是 LLVM IR）
  → 优化
  → 生成目标平台的汇编 (.s)


步骤	          能看到它的命令
1. 词法分析	      clang++ -Xclang -dump-tokens -fsyntax-only apps/cherno/HelloWorld.cpp
2. 语法分析 	  clang++ -Xclang -ast-dump -Xclang -ast-dump-filter -Xclang main -fsyntax-only apps/cherno/HelloWorld.cpp
3. 语义分析	      没有单独的输出，Clang 的语法分析和语义分析是交织在一起的，边解析边检查类型，并不是先建完树再回头检查。所以第 2 步的 AST 输出里已经带着语义分析的结果，比如每个表达式的类型、自动插入的类型转换（ImplicitCastExpr），以及 << 到底匹配到了哪个重载。想只跑到这一步、不生成任何代码，用 -fsyntax-only，它常被用来只检查代码有没有语法和类型错误。
4. 生成中间代码	   clang++ -S -emit-llvm -O0 apps/cherno/HelloWorld.cpp -o HelloWorld.ll
5. 优化	          clang++ -S -emit-llvm -O2 apps/cherno/HelloWorld.cpp -o HelloWorld.opt.ll  把 -O0 和 -O2 生成的 .ll 对比一下：diff -y -W 200 --suppress-common-lines HelloWorld.ll HelloWorld.opt.ll   更好看就 
6. 生成汇编	       clang++ -S apps/cherno/HelloWorld.cpp -o HelloWorld.s


https://godbolt.org/
https://llvm.org/docs/LangRef.html
*/

int main()
{
    int x = 1 + 2 * 3;  // %2 = alloca i32 ; store i32 7, ptr %2
    return x; // %3 = load i32, ptr %2 ; ret i32 %3
}