## CMake (Windows)

```
$ md build && cd build
$ cmake -G "Visual Studio 10" ..   # Or whatever generator you want to use cmake --help for a list.
$ start libevent.sln
```

## 调试libevent源码

1、新建libevent_test解决方案

2、将event_shared项目加入解决方案，event_shared就是编译libevent库的项目

3、 把event_shared的输出目录改为libevent_test的输出目录

![image-20220221114832137](C:\Users\A10727\AppData\Roaming\Typora\typora-user-images\image-20220221114832137.png)

4、解决方案中项目依赖项选中event_shared

![image-20220221114918721](C:\Users\A10727\AppData\Roaming\Typora\typora-user-images\image-20220221114918721.png)

这样就能在libevent_test中调试libevent源码了