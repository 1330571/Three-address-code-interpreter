# 编译原理三地址码的一些小工具

## 三地址码解释器

主要用来检验编译原理课设生成的三地址码是否能够正确运行，全部模拟的方式使用，所有变量存在map里。

三地址代码文件 `midCode.txt`

符号信息文件`symbol.txt`

### 指令

载入符号表 `loadfile symbol.txt`

载入代码 `loadcode midCode.txt`

从`main`开始执行 `main`

下一步`c`

查看当前函数符号信息`frame`

查看当前全局地址变量信息`var`

所有历史变量信息`total`

当前函数调用链`stack`

input和output为保留函数名，通过C++输入或者输出

### 解释

curFrame: 当前执行的函数体的所有变量及其值

variableRecord: 全局变量记录

functionRecord: 函数位置记录

functionBaseRecord: 记录函数的原型(及符号信息)

对于递归调用，会不断通过`buildFunctionFrame`生成一个新的变量

tempVariables: 对于使用`param`指令存入的数据会存入该变量中

### 文件

- midCode.txt gcd示范代码的三地址码
- symbol.txt gcd程序的符号表
- midCode2.txt 循环求和
- symbol2.txt 循环求和的符号表

## 中间代码优化器

### 基本块划分

`midCodeOptimizer`可执行文件

命令:

- load xxx.file 装载文件xxx.file
- split block 基本块划分
- print block 输出基本块
- pf xxx.file 输出基本块划分信息到文件xxx.file

例子:

```
load tCode.txt
split block
pf out.txt
```

### 优化流程[未实现]

- 消除局部公共子表达式(local common subexpression)
- 消除死代码(dead code)
- 重新排序，降低临时变量存储的时间
- 利用代数规则简化计算过程

### 优化技术[未实现]

- Constant Folding

- Eliminate Unreachable Basic Blocks

- Single Assignment Form

- Common Subexpression Elimination

- Copy Propagation

  - ```
    y = x
    z = 3 + y
    ```

  - Copy propagation

  - ```
    z = 3 + x
    ```

## 中间代码 TO LLVM IR

可以直接运行CompileToTarget，用

```bash
mkdir build
cd build
cmake ..
make
```

然后运行编译完的文件，可以生成一个fib.o文件

详情查看testFib.cpp文件，和test.cpp文件查看调用方法

编译testFib

```bash
clang++ testFib.cpp fib.o -o testFib
```

运行testFib查看结果

```bash
./testFib
```

codeGen.hpp中定义了LLVM的全局变量

buildObj函数是生成可重定位文件的函数，将.o文件生成在可执行文件同级目录下

详情可以参考 [LLVM Tutorial](https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html)

## Reference

[CS143 lecture14](http://web.stanford.edu/class/cs143/lectures/lecture14.pdf)

