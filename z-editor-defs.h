#ifndef ZEDITORDEFS_H
#define ZEDITORDEFS_H

//== МАКРОСЫ.
// Общее.
#define E_MAINWINDOW_UI_CONF_PATH       "../Z-Editor/settings/mainwindow_ui.ini"
#define E_SCHEMATICWINDOW_UI_CONF_PATH  "../Z-Editor/settings/schematicwindow_ui.ini"
// Формы.
#define PORT_SHAPE      -3, -3, 6, 6
#define SCALER_DIM      9
#define ELEMENT_MIN_X   40
#define ELEMENT_MIN_Y   30
//
#define C_CONF_PATH						"../Z-Editor/settings/client.xml"
#define CLIENT_REQUEST_UNDEFINED        -1
#define CLIENT_REQUEST_CONNECT          0
#define CLIENT_REQUEST_DISCONNECT       1

//== КОНСТАНТЫ.
// Многократно используемые строки.
const QString cstrMsgError = "Ошибка";
const QString cstrMsgWarning = "Внимание";
const QString cstrStatusConnected = "Соединён";

#endif // ZEDITORDEFS_H
