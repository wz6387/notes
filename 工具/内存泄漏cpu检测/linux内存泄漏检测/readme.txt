Valgrind Memcheck 一个强大开源的程序检测工具

下载地址：http://valgrind.org/downloads/current.html

Valgrind快速入门指南：http://valgrind.org/docs/manual/QuickStart.html

 

安装

cd valgrind

./autogen.sh

./configure

make

make install

使用

1、编译你的程序 debug 版本./TestMem

2、执行：valgrind --tool=memcheck --leak-check=full --log-file=./log.txt  ./TestMem

 

mtrace GNU扩展，用来跟踪malloc，mtrace为内存分配函数(malloc,rellaoc,memalign,free)安装hook函数
dmalloc 用于检查C/C++内存泄漏的工具，即是检查是否存在程序运行结束还没有释放的内存，以一个运行库发布
memwatch 和dmalloc一样，它能检测未释放的内存、同一段内存被释放多次、位址存取错误及不当使用未分配之内存区域
mpatrol 一个跨平台的 C++ 内存泄漏检测器
dbgmem 也是一个动态库发布的形式，优点类似dmalloc，但是相比之下，可能特点少了一些
Electric Fence 不仅仅能够跟踪malloc()和free(),同时能够检查读访问以及写入，能够准确指出导致错误的指令