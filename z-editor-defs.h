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
#define GROUPS_TO_GROUPS_EXCLUDE_VAR_FOR_DETACH		1
#define GROUPS_TO_GROUPS_EXCLUDE_VAR_FOR_GROUPING	2
#define GROUPS_TO_GROUPS_WHOLE_BRANCH				OVERMAX_NUMBER
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
#define MIN_ELEMENT_PROPORTION			0.635f
#define MIN_CIRCLE_PROPORTION			0.725f
#define MIN_TRIANGLE_PROPORTION			0.5f
#define MIN_GROUP_PROPORTION			1.333333f
#define P_HORR							1
#define P_VERT							2
#define P_N_DEF							0
#define P_LT							0b00
#define P_RB							0b11
#define P_LB							0b01
#define P_RT							0b10
//
#define ELEMENT_MIN_X					40
#define ELEMENT_MIN_Y					30
#define BROADCASTER_MIN					80
#define RECEIVER_MIN					50
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
//
#define RENAME_TYPE_CLASSIC				0
#define RENAME_TYPE_EMPTY				1
#define RENAME_TYPE_UID					2
#define RENAME_TYPE_DIGITS				3
//
#define SCH_SETTINGS_TYPE_MASK			0b11110011

//== КОНСТАНТЫ.
// Многократно используемые строки.
// Логи.
const char m_chLogWrongData[] = "Wrong data in pocket.";
const char m_chLogSyncFault[] = "Schematic object operations synchronization fault.";
// Тексты.
const char m_chStatusConnected[] = "Соединён";
const char m_chMsgServerPassword[] = "Пароль сервера.";
const char m_chNewElement[] = "Новый элемент";
const char m_chNewBroadcaster[] = "Новый транслятор";
const char m_chNewReceiver[] = "Новый приёмник";
const char m_chNewGroup[] = "Новая группа";
const char m_chNewLink[] = "Линк";
const char m_chPortTooltip[] = "Порт: ";
const char m_chSelection[] = "Выборка";
const char m_chPortNumExt[] = "Номер внешнего порта";
const char m_chPreElementName[] = "Элемент: ";
const char m_chPreBroadcasterName[] = "Транслятор: ";
const char m_chPreReceiverName[] = "Приёмник: ";
const char m_chPreGroupName[] = "Группа: ";
const char m_chPreObjectName[] = "Объект: ";
//
// <== МЕНЮ ПКМ ОБЪЕКТОВ ВИДА ==>
//										УДАЛЕНИЕ.
#define MENU_DELETE						1
const char m_chMenuDeleteS[] =			"Удалить выбранное";
const char m_chMenuDeleteE[] =			"Удалить элемент";
const char m_chMenuDeleteB[] =			"Удалить транслятор";
const char m_chMenuDeleteR[] =			"Удалить приёмник";
const char m_chMenuDeleteG[] =			"Удалить группу";
const char m_chMenuDeleteL[] =			"Удалить линк";
//										ПОРТЫ.
#define MENU_LINKS						2
const char m_chMenuLinksS[] =			"Линки выбранного";
const char m_chMenuLinksE[] =			"Линки элемента";
const char m_chMenuLinksB[] =			"Линки транслятора";
const char m_chMenuLinksR[] =			"Линки приёмника";
const char m_chMenuLinksG[] =			"Линки внутри группы";
#define MENU_SRC_PORT					3
const char m_chMenuPortSrc[] =			"Порт источника ";
#define MENU_DST_PORT					4
const char m_chMenuPortDst[] =			"Порт цели ";
#define MENU_EXTPORT					5
const char m_chMenuExtPortB[] =			"Внешний порт транслятора";
const char m_chMenuExtPortR[] =			"Внешний порт приёмника";
//										СОЗДАНИЕ ГРУППЫ.
#define MENU_CREATE_GROUP				6
const char m_chMenuCreateFromS[] =		"Создать группу для выбранного";
const char m_chMenuCreateFromE[] =		"Создать группу для элемента";
const char m_chMenuCreateFromB[] =		"Создать группу для транслятора";
const char m_chMenuCreateFromR[] =		"Создать группу для приёмника";
const char m_chMenuCreateFromG[] =		"Создать группу для группы";
//										ДОБАВЛЕНИЕ В ГРУППУ.
#define MENU_ADD						7
const char m_chMenuAddFreeS[] =			"Добавить выбранное в группу";
const char m_chMenuAddFreeE[] =			"Добавить элемент в группу";
const char m_chMenuAddFreeB[] =			"Добавить транслятор в группу";
const char m_chMenuAddFreeR[] =			"Добавить приёмник в группу";
//										ЦВЕТ.
#define MENU_CHANGE_BKG					8
const char m_chMenuBackgroundS[] =		"Изменить цвет фона выбранного";
const char m_chMenuBackgroundE[] =		"Изменить цвет фона элемента";
const char m_chMenuBackgroundB[] =		"Изменить цвет фона транслятора";
const char m_chMenuBackgroundR[] =		"Изменить цвет фона приёмника";
const char m_chMenuBackgroundG[] =		"Изменить цвет фона группы";
//										РАСФОРМИРОВАТЬ.
#define MENU_DISBAND					9
const char m_chMenuDisbandS[] =			"Расформировать выбранные группы";
const char m_chMenuDisbandG[] =			"Расформировать группу";
//										ОТСОЕДИНИТЬ.
#define MENU_DETACH						10
const char m_chMenuDetachS[] =			"Отсоединить выбранное от групп";
const char m_chMenuDetachE[] =			"Отсоединить элемент от группы";
const char m_chMenuDetachB[] =			"Отсоединить транслятор от группы";
const char m_chMenuDetachR[] =			"Отсоединить приёмник от группы";
const char m_chMenuDetachG[] =			"Отсоединить группу от группы";
//										СОЗДАТЬ.
#define MENU_CREATE_ELEMENT				11
const char m_chMenuCreateElement[] =	"Создать элемент";
#define MENU_CREATE_BROADCASTER			12
const char m_chMenuCreateBroadcaster[] ="Создать транслятор";
#define MENU_CREATE_RECEIVER			13
const char m_chMenuCreateReceiver[] =	"Создать приёмник";
#define MENU_ADD_ELEMENT				14
const char m_chMenuAddElement[] =		"Создать элемент в группе";
#define MENU_ADD_BROADCASTER			15
const char m_chMenuAddBroadcaster[] =	"Создать транслятор в группе";
#define MENU_ADD_RECEIVER				16
const char m_chMenuAddReceiver[] =		"Создать приёмник в группе";
//										ПЕРЕИМЕНОВАТЬ.
#define MENU_RENAME_SELECTED			17
const char m_chMenuRenameS[] =			"Переименовать выборку";
#define MENU_RENAME_EG					18
const char m_chMenuRenameE[] =			"Переименовать элемент";
const char m_chMenuRenameB[] =			"Переименовать транслятор";
const char m_chMenuRenameR[] =			"Переименовать приёмник";
const char m_chMenuRenameG[] =			"Переименовать группу";
//
// <== ПРОЧИЕ МЕНЮ ==>
//										МЕНЮ ПОРТОВ.
#define MENU_SELECTED_PORT				19
const char m_chMenuSelectedPort[] =		"Выбранный порт ";
//										МЕНЮ СЕРВЕРА.
#define MENU_SET_PASSWORD				20
const char m_chMenuSetPassword[] =		"Задать пароль";
#define MENU_SET_AS_DEFAULT				21
const char m_chMenuSetAsDefault[] =		"Установить как текущий";
#define MENU_DELETE_ITEM				22
const char m_chMenuDeleteItem[] =		"Удалить выбранный";
#define MENU_RENAME_PSEUDONYM			23
const char m_chMenuRenameP[] =			"Переименовать псевдоним";
#define MENU_DELETE_PSEUDONYM			24
const char m_chMenuDeleteP[] =			"Удалить псевдоним";
// Для диалогов.
#define ROLE_PORT_NUMBER				0x100
#define EDIT_LINK_DIRECTION_WIDTH		25
#endif // ZEDITORDEFS_H
