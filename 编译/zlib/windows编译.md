zlib1.dll是zlib官网默认编译生成的名字，听说官网能直接下载，但是我没找到。

## 1、编译生成zlib1.dll

(1) 进入zlib源码目录，比如zlib-1.2.13

(2) 运行命令

```shell
nmake -f win32/Makefile.msc
```



## 2、编译生成zlibwapi.dll

(1) 进入zlib-1.2.13\contrib\vstudio

(2) 选择相应的vs版本，然后直接编译