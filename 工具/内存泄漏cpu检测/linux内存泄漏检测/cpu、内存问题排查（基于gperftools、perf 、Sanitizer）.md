@[TOC](cpu、内存问题排查)
#  常见的问题
1. 内存泄漏
2. 内存溢出，内存越界，释放后使用等内存使用问题
3. CPU占用过高，热点函数检测

#  常用的工具
## valgrind
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210416162105431.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RoZV9PbGRfbWFuX2FuZF9zZWE=,size_16,color_FFFFFF,t_70)

 valgrind直接分析非常困难，需要自己编译debug版本的，使用valgrind会让程序运行速度变得非常慢，一般很少使用

## gperftools
[gperftools github](https://github.com/gperftools/gperftools.git)

使用简单，无需重新编译代码即可运行，对运行速度的影响也比较小

gperftool主要支持以下四个功能：

1. thread-caching malloc
2. heap-checking using tcmalloc
3. heap-profiling using tcmalloc
4. CPU profiler

## Sanitizer
[sanitizers github](https://github.com/google/sanitizers/wiki/AddressSanitizer)

ASAN（Address-Sanitizier）早先是LLVM中的特性，后被加入GCC 4.8，在GCC 4.9后加入对ARM平台的支持。因此GCC 4.8以上版本使用ASAN时不需要安装第三方库，通过在编译时指定编译CFLAGS即可打开开关
![在这里插入图片描述](https://img-blog.csdnimg.cn/20210415144842302.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1RoZV9PbGRfbWFuX2FuZF9zZWE=,size_16,color_FFFFFF,t_70)
## perf 
Perf是一个基于Linux 2.6 +系统的分析工具，它抽象了在Linux中性能度量中CPU的硬件差异 ,提供一个简单的命令行界面。 Perf基于最新版本Linux内核 的perf_events 接口

#  内存泄漏排查（基于gperftools）
[链接方法](https://liam.page/2020/06/22/CPU-Profiler-in-gperftools-not-working/)

1. 编译安装
2. 链接运行
3. 对堆的内存快照进行分析

[Visual Studio检查内存泄露方法](https://blog.jianchihu.net/visual-studio-memory-leak.html)
#  内存溢出等问题排查（基于asan）
```cpp
-fsanitize=address    #开启地址越界检查功能

-fno-omit-frame-pointer  #开启后，可以出界更详细的错误信息

-fsanitize=leak   #开启内存泄露检查功能

GCC编译选项详细了解可参考地址：https://gcc.gnu.org/onlinedocs/
```

[内存溢出问题](https://blog.csdn.net/taos_data/article/details/115000496?spm=1001.2014.3001.5501)

g++ -g -fsanitize=address main.cpp

```bash
A：“@B，帮忙看看这个 PR，解决了那个空指针导致的崩溃问题。
”B：“赞一个，怎么解决的？
”A：“额，在崩溃的地方加了一个 NULL 指针判断。
”B：“你都不去深入研究代码逻辑这里为什么会出现空指针么？？不要 work around，要追根溯源，找到 root cause 呀！！”
```

#  热点函数排查（基于perf ）
1. perf 采集分析进程
    perf record -F 500 -p $task_id -o perf.data -g sleep $time 
 
2. 基于FlameGraph 进行火焰图分析
     https://github.com/brendangregg/FlameGraph.git
     
   perf script -i perf.data > out.perf
   ./FlameGraph/stackcollapse-perf.pl out.perf > out.folded
   ./FlameGraph/flamegraph.pl out.folded > cpu.svg
   
    [如何读懂火焰图？](http://www.ruanyifeng.com/blog/2017/09/flame-graph.html)

[CPU profiler(gperftools)](https://blog.csdn.net/10km/article/details/83820080)
