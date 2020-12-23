#ifndef ZEDITORDEFS_H
#define ZEDITORDEFS_H

//== МАКРОСЫ.
// Общее.
#define E_MAINWINDOW_UI_CONF_PATH			"../Z-Editor/settings/mainwindow_ui.ini"
#define E_SCHEMATICWINDOW_UI_CONF_PATH		"../Z-Editor/settings/schematicwindow_ui.ini"
#define SCH_INTERNAL_POS_UNCHANGED          -2147483647
#define WAITING_FOR_INTERFACE				500
#define PORT_NUMBER_STR_LEN					6
#define OVERMAX_NUMBER						999999999
#define OVERMIN_NUMBER						-999999999
#define SCENE_DIM_X							6000
#define SCENE_DIM_Y							6000
// Макросы для удобного чтения сложных вызовов функций.
#define SEND										true
#define DONT_SEND									false
#define SEND_ELEMENTS								true
#define DONT_SEND_ELEMENTS							false
#define SEND_ELEMENT								true
#define DONT_SEND_ELEMENT							false
#define SEND_GROUP									true
#define DONT_SEND_GROUP								false
#define WITH_GROUP									true
#define WITHOUT_GROUP								false
#define SEND_NEW_ELEMENTS_TO_GROUPS_RELATION		true
#define DONT_SEND_NEW_ELEMENTS_TO_GROUPS_RELATION	false
#define SEND_NEW_GROUPS_TO_GROUPS_RELATION			true
#define DONT_SEND_NEW_GROUPS_TO_GROUPS_RELATION		false
#define GROUPS_TO_GROUPS_FIRST_ONLY					true
#define GROUPS_TO_GROUPS_WHOLE_BRANCH				false
#define SEND_ELEMENT_GROUP_CHANGE					true
#define DONT_SEND_ELEMENT_GROUP_CHANGE				false
#define ADD_SEND_BUSY								true
#define ADD_SEND_ZPOS								false
#define ADD_SEND_FRAME								true
#define DONT_ADD_SEND_FRAME							false
#define GET_SELECTED_OBJECTS_UP						true
#define DONT_GET_SELECTED_OBJECTS_UP				false
#define WITH_FRAME									true
#define WITHOUT_FRAME								false
#define WITH_POSITION								true
#define WITHOUT_POSITION							false
#define WITH_ELEMENTS_FRAMES						true
#define WITHOUT_ELEMENTS_FRAMES						false
#define FROM_ELEMENT								true
#define NOT_FROM_ELEMENT							false
#define REMOVE_FROM_CLIENT							true
#define DONT_REMOVE_FROM_CLIENT						false
#define TO_TOP										true
#define LEAVE_IN_PLACE								false
#define AFFECT_SELECTED								true
#define DONT_AFFECT_SELECTED						false
#define RECURSION_CONTINUE							false
#define MOVE_BODY									true
#define DONT_MOVE_BODY								false
#define IS_SRC										true
#define IS_DST										false
#define IS_INCOMING									true
#define IS_NOT_INCOMING								false
#define BLOCKING_ON									true
#define BLOCKING_OFF								false
#define REMOVE_GROUPS_FROM_SELECTED					true
#define DONT_REMOVE_GROUPS_FROM_SELECTED			false

// Размеры.
#define SCALER_RECT_DIM					9.0f
#define SCALER_TR_DIM					6.0f
#define SCALER_CIR_DIM					6.0f
#define PORT_DIM						3.0f
#define FRAME_DIM_INC					2.0f
//
#define MINIMIZED_DIM					30
// Корректировки размеров.
#define SCALER_RECT_DIM_CORR			0.2f
#define SCALER_TR_DIM_CORR				0.25f
#define SCALER_CIR_DIM_CORR				0.15f
#define FRAME_DIM_INC_CORR				0.275f
#define TRIANGLE_DECR_PROPORTION		15.185f
#define MIN_ELEMENT_PROPORTION			0.715f
#define MIN_CIRCLE_PROPORTION			0.785f
#define MIN_GROUP_PROPORTION			1.43f
//
#define ELEMENT_MIN_X					40
#define ELEMENT_MIN_Y					30
//
#define C_CONF_PATH							"../Z-Editor/settings/client.xml"
#define CLIENT_REQUEST_UNDEFINED			2
#define CLIENT_REQUEST_CONNECT				0
#define CLIENT_REQUEST_DISCONNECT			1
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
#define SchBackgroundActive				QColor(22, 34, 26, 255)
#define SchBackgroundInactive			QColor(80, 80, 80, 255)
//
#define SCH_TYPE_OF_ITEM				0
#define SCH_TYPE_ITEM_UI				1
//
#define SCH_KIND_OF_ITEM				1
#define SCH_KIND_ITEM_ELEMENT			0
#define SCH_KIND_ITEM_LINK				1
#define SCH_KIND_ITEM_PORT				2
#define SCH_KIND_ITEM_SCALER			3
#define SCH_KIND_ITEM_FRAME				4
#define SCH_KIND_ITEM_GROUP				5
//
#define SCH_UPDATE_MAIN					0b00000000001
#define SCH_UPDATE_ELEMENT_ZPOS			0b00000000010
#define SCH_UPDATE_ELEMENT_FRAME		0b00000000100
#define SCH_UPDATE_LINKS_POS			0b00000001000
#define SCH_UPDATE_LINK_POS				0b00000010000
#define SCH_UPDATE_PORTS_POS			0b00000100000
#define SCH_UPDATE_PORT_SRC_POS			0b00001000000
#define SCH_UPDATE_PORT_DST_POS			0b00010000000
#define SCH_UPDATE_GROUP_FRAME			0b00100000000
#define SCH_UPDATE_GROUP_ZPOS			0b01000000000
#define SCH_UPDATE_GROUP				0b10000000000
//
#define DEFAULT_NEW_PORT				0

//== КОНСТАНТЫ.
// Многократно используемые строки.
const char m_chLogWrongData[] = "Wrong data in pocket.";
const char m_chLogSyncFault[] = "Schematic object operations synchronization fault.";
const char m_chLogSource[] = "Source ";
const char m_chLogDestination[] = "Destination ";
const char m_chStatusConnected[] = "Соединён";
const char m_chMsgServerPassword[] = "Пароль сервера.";
const char m_chNewElement[] = "Новый элемент";
const char m_chNewGroup[] = "Новая группа";
const char m_chPortTooltip[] = "Порт: ";
//
const char m_chMenuDelete[] = "Удалить";
#define MENU_DELETE				1
const char m_chMenuPorts[] = "Порты";
#define MENU_PORTS				2
const char m_chMenuSrcPort[] = "Порт источника ";
#define MENU_SRC_PORT			3
const char m_chMenuDstPort[] = "Порт приёмника ";
#define MENU_DST_PORT			4
const char m_chMenuSelectedPort[] = "Выбранный порт ";
#define MENU_SELECTED_PORT		5
const char m_chMenuCreateGroup[] = "Создать группу с выбранным";
#define MENU_CREATE_GROUP		6
const char m_chMenuCreateElement[] = "Создать элемент";
#define MENU_CREATE_ELEMENT		7
const char m_chMenuAddFreeSelected[] = "Добавить выбранное в группу";
#define MENU_ADD_SELECTED		8
const char m_chMenuBackground[] = "Изменить цвет фона";
#define MENU_CHANGE_BACKGROUND	9
const char m_chMenuDisband[] = "Расформировать выбранные группы";
#define MENU_DISBAND			10
const char m_chMenuDetach[] = "Отсоединить выбранное от групп";
#define MENU_DETACH				11
const char m_chMenuAddElement[] = "Создать элемент в группе";
#define MENU_ADD_ELEMENT		12
const char m_chMenuSetPassword[] = "Задать пароль";
#define MENU_SET_PASSWORD		13
const char m_chMenuSetAsDefault[] = "Установить как текущий";
#define MENU_SET_AS_DEFAULT		14
const char m_chMenuRename[] = "Переименовать";
#define MENU_RENAME				15
const char m_chMenuRenameSelection[] = "Переименовать выборку";
#define MENU_RENAME_SELECTION	16
#endif // ZEDITORDEFS_H
