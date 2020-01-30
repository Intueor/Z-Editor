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
    main-window.cpp \
    schematic-window.cpp \
    schematic-view.cpp \
    ../Z-Hub/tester.cpp \
    ../Z-Hub/TinyXML2/tinyxml2.cpp \
    ../Z-Hub/dlfcn-win32/dlfcn.c \
    ../Z-Hub/main-hub.cpp \
    ../Z-Hub/parser-ext.cpp

HEADERS += \
    main-window.h \
    schematic-window.h \
    schematic-view.h \
    ../Z-Hub/logger.h \
    ../Z-Hub/tester.h \
    ../Z-Hub/dlfcn-win32/dlfcn.h \
    ../Z-Hub/pthread/include/pthread.h \
    ../Z-Hub/pthread/include/sched.h \
    ../Z-Hub/pthread/include/semaphore.h \
    ../Z-Hub/TinyXML2/tinyxml2.h \
    ../Z-Hub/main-hub.h \
    ../Z-Hub/main-hub-defs.h \
    ../Z-Hub/parser-ext.h

FORMS += \
    main-window.ui \
    schematic-window.ui

DISTFILES += \
    ../Z-Hub/pthread/lib/pthreadVC2.lib \
    ../Z-Hub/pthread/dll/pthreadVC2.dll

RESOURCES += \
    ../Z-Hub/resources.qrc

