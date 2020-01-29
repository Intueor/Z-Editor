unix {
QMAKE_RPATHDIR += /usr/games/Z-Editor/lib
}
QT += core gui widgets

TARGET = Z-Editor
TEMPLATE = app

unix {
QMAKE_CXXFLAGS += -fno-strict-aliasing
}

CONFIG += console
win32 {
CONFIG += no_batch
}

win32 {
LIBS += ..\\Z-Editor\\pthread\\lib\\pthreadVC2.lib
}

win32 {
DEFINES += WIN32 _WINDOWS _CRT_SECURE_NO_WARNINGS
}

SOURCES += \
    main.cpp \
    tester.cpp \
    main-hub.cpp \
    parser-ext.cpp \
    TinyXML2/tinyxml2.cpp \
    dlfcn-win32/dlfcn.c \
    main-window.cpp \
    schematic-window.cpp \
    schematic-view.cpp

HEADERS += \
    logger.h \
    tester.h \
    main-hub.h \
    main-hub-defs.h \
    parser-ext.h \
    dlfcn-win32/dlfcn.h \
    TinyXML2/tinyxml2.h \
    pthread/include/pthread.h \
    pthread/include/sched.h \
    pthread/include/semaphore.h \
    main-window.h \
    schematic-window.h \
    schematic-view.h

FORMS += \
    main-window.ui \
    schematic-window.ui

DISTFILES += \
    pthread/lib/pthreadVC2.lib \
    pthread/dll/pthreadVC2.dll

RESOURCES += \
    resources.qrc

