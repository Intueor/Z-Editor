#ifndef ZEDITORDEFS_H
#define ZEDITORDEFS_H

//== МАКРОСЫ.
// Общее.
#define E_MAINWINDOW_UI_CONF_PATH       "../Z-Editor/settings/mainwindow_ui.ini"
#define E_SCHEMATICWINDOW_UI_CONF_PATH  "../Z-Editor/settings/schematicwindow_ui.ini"
#define WAITING_FOR_INTERFACE			500
#define SEND_ELEMENTS						true
#define DONT_SEND_ELEMENTS					false
#define SEND_GROUP							true
#define DONT_SEND_GROUP						false
#define ELEMENTS_BLOCKING_PATTERN_ON		true
#define ELEMENTS_BLOCKING_PATTERN_OFF		false
#define WITH_GROUP							true
#define WITHOUT_GROUP						false
#define SEND_NEW_ELEMENTS_TO_GROUP			true
#define DONT_SEND_NEW_ELEMENTS_TO_GROUP		false
#define ADD_SEND_BUSY						true
#define ADD_SEND_ZPOS						false
#define ADD_SEND_FRAME						true
#define DONT_ADD_SEND_FRAME					false
#define GET_SELECTED_ELEMENTS_UP			true
#define DONT_GET_SELECTED_ELEMENTS_UP		false
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
//
#define QRealToDbFrame(qreal, dbframe)                                                              \
	dbframe.dbX = qreal.x();                                                                        \
	dbframe.dbY = qreal.y();                                                                        \
	dbframe.dbH = qreal.height();                                                                   \
	dbframe.dbW = qreal.width();
#define ThrUiAccessV(element,func,type,value)   QMetaObject::invokeMethod(p_ui->element, #func, Qt::QueuedConnection, Q_ARG(type, value))
#define ThrUiAccessE(element,func)              QMetaObject::invokeMethod(p_ui->element, #func, Qt::QueuedConnection)
#define ThrUiAccessVT(element,func,type,value)  QMetaObject::invokeMethod(element, #func, Qt::QueuedConnection, Q_ARG(type, value))
#define ThrUiAccessET(element,func)             QMetaObject::invokeMethod(element, #func, Qt::QueuedConnection)
//
#define TrySendBufferToServer			if(!MainWindow::p_Client->oInternalNetHub.CheckIsBufferFree()) MainWindow::p_Client->SendBufferToServer()
//
#define SchBackgroundActive		QColor(22, 34, 26, 255)
#define SchBackgroundInactive	QColor(80, 80, 80, 255)
//
#define SCH_TYPE_OF_ITEM        0
#define SCH_TYPE_ITEM_UI        1
//
#define SCH_KIND_OF_ITEM        1
#define SCH_KIND_ITEM_ELEMENT   0
#define SCH_KIND_ITEM_LINK      1
#define SCH_KIND_ITEM_PORT      2
#define SCH_KIND_ITEM_SCALER    3
#define SCH_KIND_ITEM_FRAME     4
#define SCH_KIND_ITEM_GROUP     5
//
#define SCH_UPDATE_MAIN             0b000000000001
#define SCH_UPDATE_ELEMENT_POS      0b000000000010
#define SCH_UPDATE_ELEMENT_ZPOS     0b000000000100
#define SCH_UPDATE_ELEMENT_FRAME    0b000000001000
#define SCH_UPDATE_LINKS_POS        0b000000010000
#define SCH_UPDATE_LINK_POS         0b000000100000
#define SCH_UPDATE_PORTS_POS        0b000001000000
#define SCH_UPDATE_PORT_SRC_POS     0b000010000000
#define SCH_UPDATE_PORT_DST_POS     0b000100000000
#define SCH_UPDATE_GROUP_FRAME      0b001000000000
#define SCH_UPDATE_GROUP_ZPOS       0b010000000000
#define SCH_UPDATE_GROUP	0b100000000000
//
#define PORT_SHAPE      -3, -3, 6, 6
#define SCALER_DIM      9
#define ELEMENT_MIN_X   40
#define ELEMENT_MIN_Y   30
// Коды изменяемых полей.
#define SCH_ELEMENT_BIT_BUSY            0b00000001
#define SCH_ELEMENT_BIT_BKG_COLOR       0b00000010
#define SCH_ELEMENT_BIT_FRAME           0b00000100
#define SCH_ELEMENT_BIT_POS             0b00001000
#define SCH_ELEMENT_BIT_GROUP           0b00010000
#define SCH_ELEMENT_BIT_ZPOS            0b00100000
//
#define SCH_LINK_BIT_SCR_PORT_POS       0b00000001
#define SCH_LINK_BIT_DST_PORT_POS       0b00000010
#define SCH_LINK_BIT_INIT_ERROR         0b11111111
//
#define SCH_GROUP_BIT_BUSY              0b00000001
#define SCH_GROUP_BIT_BKG_COLOR         0b00000010
#define SCH_GROUP_BIT_FRAME             0b00000100
#define SCH_GROUP_BIT_ZPOS              0b00001000
#define SCH_GROUP_BIT_ELEMENTS_SHIFT    0b00010000
//
#define SCH_NEXT_Z_SHIFT				1.0f
#define SCH_LINK_Z_SHIFT				0.1f

//== КОНСТАНТЫ.
// Многократно используемые строки.
const char m_chLogWrongData[] = "Wrong data in pocket.";
const char m_chLogSyncFault[] = "Element operations synchronization fault.";
const char m_chMenuDelete[] = "Удалить";
const char m_chMenuSetPassword[] = "Задать пароль";
const char m_chMenuSetAsDefault[] = "Установить как текущий";
const char m_chStatusConnected[] = "Соединён";
const char m_chMsgServerPassword[] = "Пароль сервера.";
const char m_chNewElement[] = "Новый элемент";
const char m_chGroup[] = "Группа";
const char m_chElement[] = "Элемент";
const char m_chMenuRename[] = "Переименовать";
const char m_chMenuRenameSelection[] = "Переименовать выборку";
const char m_chDelete[] = "Удалить";
const char m_chPorts[] = "Порты";
const char m_chRemoveFromGroup[] = "Удалить из группы";
const char m_chAddFreeSelected[] = "Добавить выбранные свободные элементы в группу";
const char m_chBackground[] = "Изменить цвет фона";
const char m_chDisband[] = "Расформировать";
const char m_chAddElement[] = "Создать элемент в группе";

#endif // ZEDITORDEFS_H
