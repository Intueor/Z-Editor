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
LIBS += ..\\Z-Hub\\pthread\\lib\\pthreadVC2.lib \
	..\\Z-Hub\\win-libs\\Ws2_32.lib
}

win32 {
DEFINES += WIN32 _WINDOWS _CRT_SECURE_NO_WARNINGS
}

SOURCES += \
    ../Z-Hub/Dialogs/set-server-dialog.cpp \
    Dialogs/set-password-dialog.cpp \
    main.cpp \
    main-window.cpp \
    schematic-window.cpp \
    schematic-view.cpp \
    ../Z-Hub/tester.cpp \
    ../Z-Hub/TinyXML2/tinyxml2.cpp \
    ../Z-Hub/dlfcn-win32/dlfcn.c \
    ../Z-Hub/main-hub.cpp \
    ../Z-Hub/parser-ext.cpp \
    Client/client.cpp \
    ../Z-Hub/Server/net-hub.cpp \
    ../Z-Hub/Server/proto-parser.cpp \
    ../Z-Hub/Server/proto-util.cpp \
    ../Z-Hub/Dialogs/message-dialog.cpp

HEADERS += \
    ../Z-Hub/Dialogs/set-server-dialog.h \
    Dialogs/set-password-dialog.h \
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
    ../Z-Hub/parser-ext.h \
    z-editor-defs.h \
    Client/client.h \
    ../Z-Hub/Server/protocol.h \
    ../Z-Hub/Server/net-hub.h \
    ../Z-Hub/Server/proto-parser.h \
    ../Z-Hub/Dialogs/message-dialog.h

FORMS += \
    ../Z-Hub/Dialogs/set-server-dialog.ui \
    Dialogs/set-password-dialog.ui \
    main-window.ui \
    schematic-window.ui \
    ../Z-Hub/Dialogs/message-dialog.ui

DISTFILES += \
    ../Z-Hub/pthread/lib/pthreadVC2.lib \
    ../Z-Hub/pthread/dll/pthreadVC2.dll \
    ../Z-Hub/win-libs/WS2_32.Lib

RESOURCES += \
    ../Z-Hub/resources.qrc

