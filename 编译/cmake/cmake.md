# cmake语法

cmake的配置文件CMakeLists.txt，每个目录都应该有一个。

1、开头设置最低版本号要求，否则会有告警：

```cmake
cmake_minimum_required(VERSION 2.8)
```

2、设置项目名称

```cmake
project(muduo)
```

3、设置源文件

```cmake
aux_source_directory(dir var)
#dir 是指定的目录
#var 是变量
#例如
#查找当前目录下的所有源文件
#将名称保存到SRCS变量
aux_source_directory(. SRCS)
```

aux_source_directory会把目录下的所有源文件都加进来，如果我们有一些不需要的文件，可以用另一种方法：

```cmake
SET(SRC_LIST main.c test.c)
```

SET指令用来定义变量

4、添加子目录

```cmake
add_subdirectory(muduo/base)
```

5、指定生成目标

```cmake
add_executable(muduo ${SRCS})
```

6、生成链接库

```cmake
add_library(muduo_base )
```

7、添加链接库

```cmake
target_link_libraries(muduo pthread rt)
```

