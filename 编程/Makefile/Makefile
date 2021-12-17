#编译的base路径
#$@目标文件，$^所有的依赖文件，$<第一个依赖文件
BASE_DIR=.

C_SRC 			= $(shell find . -name "*.c")
C_OBJS 			= $(C_SRC:%.c=%.o)

CPP_SRC 		= $(shell find . -name "*.cpp")
CPP_OBJS 		= $(CPP_SRC:%.cpp=%.o)

#目标库名
#TARGET=
TARGET=$(BASE_DIR)/RecStreamTool_1

#公共头文件
INCLUDE =-I$(BASE_DIR)/include 
LIB=  -L$(BASE_DIR)/lib -lzlmediakit -lzltoolkit -lpthread -lssl -lcrypto -lflv -lmov -lmpeg

CC = g++
CPPFLAG = -Wall -Werror -g -rdynamic -w -fPIC -fno-use-cxa-atexit -fexceptions 
all: $(TARGET) del 
  
$(TARGET):$(CPP_OBJS) $(C_OBJS) 
	$(CC) -o $(TARGET) $(CPPFLAG) $(CPP_OBJS) $(C_OBJS) $(INCLUDE) $(LIB) 

$(C_OBJS):%.o : %.c
	$(CC) -c $(CPPFLAG) $(INCLUDE) $< -o $@
	
$(CPP_OBJS):%.o : %.cpp
	$(CC) -c $(CPPFLAG) $(INCLUDE) $< -o $@

clean:
	rm -rf $(TARGET) $(CPP_OBJS) $(C_OBJS)

del:
	rm $(CPP_OBJS) $(C_OBJS)

