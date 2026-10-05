# Goal

cs-through-cpp 要培养的，是一个先能独立写、独立想、独立调试，随后能判断 AI、驾驭 AI，并对最终系统负责的人。

# Why C++

我想弄明白程序在机器上到底是怎么跑的，C++ 是能让我最直接看到内存和编译过程的语言。我不确定它未来还能多值钱，也知道 AI 写得比我好，但这件事我想自己搞懂。从业10年我无数次的想，如今我来做了，应了我10年前的slogon - 走慢点、看近点。


# Pain

我是从GPT2就开始接触"AI"了，那是2019年了，如今7年过去了，期间我跟随者这股浪潮经历了数以千记的 `Aha moment` -> `f**k moment`。而今我是真的累了，我对LLM有3点看法：

1. 深度学习炼化了LLM，将人类语料中的模式识别为浮点数，谓之“智能”，其也必将还之于“数据”，如果你不是一个丰富、有趣、专业的“数据”那你永远也无法点亮“沉睡的数据元”。
2. 任何可被清晰定义 - 验证 - 循环的问题，最终都会被LLM取代。
3. 你要冷静，要默默耕耘自己的小智慧，不要被所谓之当今时代的大智慧裹挟着走，走着走着你发现自己成了“空白”。

# RoadPath

## The Cherno 

| # | Video | My understanding | Code |
|---:|---|---|---|
| 000 | hello world |   `iosteam` | [ `000-hello-world`](apps/cherno/000-hello-world/)|
| 001 | How C++ Works | `.cpp → .ii → .s → .o → executable`：预处理、编译、汇编、链接。 | [`001-how-cpp-works`](apps/cherno/001-how-cpp-works/) |
| 002 | How The C++ Compiler Works |   `词法分析 - 语法分析 - 语义分析 - 生成IR - 优化代码 - 生成汇编` : 编译过程 | [ `002-how-the-cpp-compiler-works`](apps/cherno/002-how-the-cpp-compiler-works/) |
| 003 | How the C++ Linker Works | `符号解析 - 合并段 - 重定位` : 链接过程 | [`003-how-the-cpp-linker-works`](apps/cherno/003-how-the-cpp-linker-works/) |
| 004 | variable in cpp | `c++标准中有3大类基本类型：void、nullptr_t、arithmetic type）` | [`004-variable-in-cpp`](apps/cherno/004-variable-in-cpp/) | 


## Todos

- [x] 003 linker 链接器工作过程 链接问题
- [x] 004 变量 5 + 2
- [ ] 005 函数 压栈
- [ ] 006 头文件的本质 与 2种防止头文件包含语法





# Commit Messages

```bash
ci: 所有与测试相关的更改，以及对 GitHub 工作流等的修改
dev: 开发相关更改，包括对 cursor 或 claude 规则的更新
fix(模块): 修复缺陷/BUG
feat(模块): 新增功能
enh(模块): 功能增强或优化
docs: 文档相关修改
ref(模块): 代码重构
chore: 其他例行维护工作（如 pre-commit 钩子、导包整理等）
```

Generally, the description should focus on the intent of the changes, not the implementation details.

# workflow

```bash
gh pr create --base main --head dev --title "feat: 初始化 CMake 工程并加入 hello world" --body "XXX"
```

