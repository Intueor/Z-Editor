unix {
QMAKE_RPATHDIR += /usr/games/Z-Editor/lib
}
QT += core gui widgets

TARGET = Z-Editor
TEMPLATE = app

unix {
QMAKE_CXXFLAGS += -fno-strict-aliasing -Wno-implicit-fallthrough -std=gnu++11
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
    ../Z-Hub/Dialogs/set_server_dialog.cpp \
    ../Z-Hub/Dialogs/set_proposed_string_dialog.cpp \
    Dialogs/create_link_dialog.cpp \
    Dialogs/edit_links_dialog.cpp \
    Dialogs/edit_port_dialog.cpp \
    Dialogs/set_proposed_number_dialog.cpp \
    graphics-background-item.cpp \
    graphics-element-item.cpp \
    graphics-frame-item.cpp \
    graphics-group-item.cpp \
    graphics-link-item.cpp \
    graphics-port-item.cpp \
    graphics-scaler-item.cpp \
    graphics-text-edit.cpp \
    main.cpp \
    main-window.cpp \
    safe-searching-line-edit.cpp \
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
    ../Z-Hub/Dialogs/message_dialog.cpp \
    Dialogs/batch_rename_dialog.cpp \
    switch-another-push-button.cpp

HEADERS += \
    ../Z-Hub/Dialogs/set_server_dialog.h \
    ../Z-Hub/Dialogs/set_proposed_string_dialog.h \
    Dialogs/create_link_dialog.h \
    Dialogs/edit_links_dialog.h \
    Dialogs/edit_port_dialog.h \
    Dialogs/set_proposed_number_dialog.h \
    graphics-background-item.h \
    graphics-element-item.h \
    graphics-frame-item.h \
    graphics-group-item.h \
    graphics-link-item.h \
    graphics-port-item.h \
    graphics-scaler-item.h \
    graphics-text-edit.h \
    main-window.h \
    safe-searching-line-edit.h \
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
    switch-another-push-button.h \
    z-editor-defs.h \
    Client/client.h \
    ../Z-Hub/Server/protocol.h \
    ../Z-Hub/Server/net-hub.h \
    ../Z-Hub/Server/proto-parser.h \
    ../Z-Hub/Dialogs/message_dialog.h \
    Dialogs/batch_rename_dialog.h

FORMS += \
    ../Z-Hub/Dialogs/set_server_dialog.ui \
    ../Z-Hub/Dialogs/set_proposed_string_dialog.ui \
    Dialogs/create_link_dialog.ui \
    Dialogs/edit_links_dialog.ui \
    Dialogs/edit_port_dialog.ui \
    Dialogs/set_proposed_number_dialog.ui \
    main-window.ui \
    schematic-window.ui \
    ../Z-Hub/Dialogs/message_dialog.ui \
    Dialogs/batch_rename_dialog.ui

DISTFILES += \
    ../Z-Hub/pthread/lib/pthreadVC2.lib \
    ../Z-Hub/pthread/dll/pthreadVC2.dll \
    ../Z-Hub/win-libs/WS2_32.Lib \
    Z-Editor.supp

RESOURCES += \
    ../Z-Hub/resources.qrc

