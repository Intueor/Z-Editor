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

//== КОНСТАНТЫ.
const QString cstrMsgFailedToConnect = "Соединение невозможно";
const QString cstrMsgFailedToDisonnect = "Программное разъединение невозможно";
const QString cstrMsgServerDisconnectClients = "Сервер отключил клиенты";
const QString cstrMsgKicked = "Сервер отключил клиент";
const QString cstrMsgWrongPassword = "Неверный пароль";
const QString cstrStatusStartClient = "Запуск клиента...";
const QString cstrStatusConnected = "Соединён";
const QString cstrStatusStopClient = "Остановка клиента...";

#endif // ZEDITORDEFS_H
