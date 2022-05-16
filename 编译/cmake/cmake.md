# cmake语法

cmake的配置文件CMakeLists.txt，每个目录都应该有一个。

1、开头设置最低版本号要求，否则会有告警：

```cmake
cmake_minimum_required(VERSION 2.8)
```

2、设置项目名称

```cmake
project(muduo)
cmake自动定义了两个变量:PROJECT_BINARY_DIR 和 PROJECT_SOURCE_DIR
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
SET(SRCS main.c test.c)
```

SET指令用来定义变量

4、添加子目录

```cmake
add_subdirectory(muduo/base)
```

5、添加头文件搜索路径

```cmake
include_directories命令添加多个指定头文件的搜索路径，路径之间用空格分隔，如：
include_directories(${Boost_INCLUDE_DIRS})
```

6、使用其它库

```cmake
FIND_PATH指明头文件查找路径，原型如下:
find_path(VAR name1 [path1 path2 ...]) 该命令将参数path的目录中查找文件name1，并将找到的路径保存在变量VAR中

FIND_LIBRARY用于查找链接库并将结果保存在变量中，同find_path类似

如果没有指定路径，会根据环境变量CMAKE_INCLUDE_PATH和CMAKE_LIBRARY_PATH去搜索，注意这两个是环境变量，不是cmake变量，可以在bash中用export设置，比如：
export CMAKE_INCLUDE_PATH=/usr/include/hello
```

7、添加编译选项

```cmake
add_compile_options(-std=c++11 -Wall)
```

8、生成debug版和release版

```cmake
cmake中有一个变量CMAKE_BUILD_TYPE，可以取值Debug、Release、RelWithDebInfo和MinSizeRel。当这个变量是Debug时，cmake会使用CMAKE_CXX_FLAGS_DEBUG 和 CMAKE_C_FLAGS_DEBUG 中的字符串作为编译选项生成 Makefile;当这个变量为Release时，会使用变量 CMAKE_CXX_FLAGS_RELEASE 和 CMAKE_C_FLAGS_RELEASE 选项生成 Makefile。
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall")
```



8、指定生成目标

```cmake
add_executable(muduo ${SRCS})
```

9、生成链接库

```cmake
add_library(muduo_base ${base_SRCS})
```

10、添加链接库

```cmake
target_link_libraries(muduo pthread rt)
```





