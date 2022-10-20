TARGET = WebSocket
TEMPLATE = lib
CONFIG += debug_and_release warn_on plugin
CONFIG -= qt 
CONFIG += c++11 console
QT   -= gui

CONFIG += force_debug_info c++11 
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -O1
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -O2
QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO -= -O


include(../../common.pri)
include(../../PLATFORM_SUFFIX.pri)

CONFIG(debug, debug|release) {
    DESTDIR = ../../CommonFile/CommonLib/Debug$$SUFFIX
    OBJECTS_DIR = ./Debug$$SUFFIX
    UI_DIR = ./Debug$$SUFFIX
    RCC_DIR = ./Debug$$SUFFIX
    MOC_DIR = ./Debug$$SUFFIX
    LIBS += -L../../CommonFile/CommonLib/Debug$$SUFFIX
} else {
    DESTDIR = ../../CommonFile/CommonLib/Release$$SUFFIX
    OBJECTS_DIR = ./Release$$SUFFIX
    UI_DIR = ./Release$$SUFFIX
    RCC_DIR = ./Release$$SUFFIX
    MOC_DIR = ./Release$$SUFFIX
    LIBS += -L../../CommonFile/CommonLib/Release$$SUFFIX
}

INCLUDEPATH += .

!mac:INCLUDEPATH +=../../CommonFile/CommonLib/third_lib$$SUFFIX/libevent/include
mac:INCLUDEPATH += ../../CommonFile/CommonLib/third_lib_64/libevent/include
    
LIBS += \
    -L../../CommonFile/CommonLib

!mac {
    LIBS += -L../../CommonFile/CommonLib/third_lib$$SUFFIX/libevent/lib/ 
}
mac {
    LIBS += -L../../CommonFile/CommonLib/third_lib_64/libevent/lib/
}

LIBS += -levent

win32:{
	LIBS += -lws2_32 \
}
unix :{
    DEFINES += __DLL_EXPORTS__
    DEFINES += EVENT__HAVE_PTHREADS
    DEFINES += EVENT2_EXPORT_SYMBOL
}
unix:!mac: {
    LIBS += -ldl
    linux-g++ {
        contains(DEFINES,OS_IS_64BIT) {
        message("Linux 64bit Websocket")
        }
        else {
            message("Linux 32bit Websocket")
        }
    }
    else : linux-g++-32{
        message("Linux 32bit Websocket")
     }
    else {
        message("...Linux_ARM... Websocket")
        DEFINES +=  CONFIG_ARM
        LIBS += -L../../CommonFile/CommonLib/third_lib/
     }
    
}

mac: {
    DEFINES += __ENVIRONMENT_MAC__
    DEFINES -= USE_TR1_NAMESPACE
    CONFIG += c++11
    CONFIG(debug, debug|release) {
        QMAKE_POST_LINK += sh $${PWD}/../../Projects/NVMS5000/Build/mac-build-postlink.sh $${PWD}/../../CommonFile/CommonLib/Debug_mac/lib$${TARGET}.dylib $${PWD}/../ Debug
    } else {
        # to create .dsym file
        QMAKE_CFLAGS+=-gdwarf-2
        QMAKE_CXXFLAGS+=-gdwarf-2
        QMAKE_POST_LINK += sh $${PWD}/../../Projects/NVMS5000/Build/mac-build-postlink.sh $${PWD}/../../CommonFile/CommonLib/Release_mac/lib$${TARGET}.dylib $${PWD}/../ Release
    }
    LIBS += -liconv
}
unix:!mac: {
    QMAKE_CC += -g
    QMAKE_CXX += -g
    QMAKE_LINK += -g
}

SOURCES += \
    base64.cpp \
    SHA1.cpp \
    Timer.cpp \
    TimerQueue.cpp \
    WebSocketClient.cpp \
    WebSocketClientImpl.cpp \
    WebSocketConnection.cpp \
    WebSocketEventLoop.cpp \
    WebSocketEventLoopImpl.cpp \
    WebSocketEventLoopThread.cpp \
    WebSocketEventLoopThreadImpl.cpp \
    WebSocketSplitter.cpp \
    main.cpp 

HEADERS += \
    base64.h \
    Callbacks.h \
    SHA1.h \
    Timer.h \
    TimerId \
    TimerQueue.h \
    WebSocketClient.h \
    WebSocketClientImpl.h \
    WebSocketConnection.h \
    WebSocketEventLoop.h \
    WebSocketEventLoopImpl.h \
    WebSocketEventLoopThread.h \
    WebSocketEventLoopThreadImpl.h \
    WebSocketSplitter.h \
    WorkingQueue.h 

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
