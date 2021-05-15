//== ВКЛЮЧЕНИЯ.
#include <QList>
#include "main-window.h"
#include "ui_main-window.h"
#include "../Z-Hub/Dialogs/message_dialog.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"
#include "../Z-Hub/z-hub-defs.h"

//== МАКРОСЫ.
#define LOG_NAME				"main-window"
#define LOG_DIR_PATH			"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS(MainWindow)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(MainWindow)
MainWindow* MainWindow::p_This = nullptr;
int MainWindow::iInitRes;
Ui::MainWindow* MainWindow::p_ui = new Ui::MainWindow;
QSettings* MainWindow::p_UISettings = nullptr;
const char* MainWindow::cp_chUISettingsName = E_MAINWINDOW_UI_CONF_PATH;
QLabel* MainWindow::p_QLabelStatusBarText;
SchematicWindow* MainWindow::p_SchematicWindow = nullptr;
Client* MainWindow::p_Client = nullptr;
PServerName MainWindow::oPServerName;
char MainWindow::m_chIPInt[IP_STR_LEN];
char MainWindow::m_chPortInt[PORT_STR_LEN];
char MainWindow::m_chPasswordInt[AUTH_PASSWORD_STR_LEN];
NetHub::IPPortPassword MainWindow::oIPPortPassword;
char MainWindow::chLastClientRequest = CLIENT_REQUEST_UNDEFINED;
bool MainWindow::bBlockConnectionButtons = false;
bool MainWindow::bBlockingGraphics = false;
PSchReadyFrame MainWindow::oPSchReadyFrame;
Ui::MainWindow* WidgetsThrAccess::p_uiInt = nullptr;
PSchElementBase* WidgetsThrAccess::p_PSchElementBase = nullptr;
PSchLinkBase* WidgetsThrAccess::p_PSchLinkBase = nullptr;
PSchGroupBase* WidgetsThrAccess::p_PSchGroupBase = nullptr;
GraphicsElementItem* WidgetsThrAccess::p_ConnGraphicsElementItem = nullptr;
GraphicsLinkItem* WidgetsThrAccess::p_ConnGraphicsLinkItem = nullptr;
GraphicsGroupItem* WidgetsThrAccess::p_ConnGraphicsGroupItem = nullptr;
QList<GraphicsElementItem*> WidgetsThrAccess::vp_ConnGraphicsElementItems;
QList<GraphicsGroupItem*> WidgetsThrAccess::vp_ConnGraphicsGroupItems;
WidgetsThrAccess* MainWindow::p_WidgetsThrAccess = nullptr;
bool MainWindow::bSchemaIsOpened = false;

//== ФУНКЦИИ КЛАССОВ.
//== Класс добавки данных сервера к стандартному элементу лист-виджета.
// Конструктор.
ServersListWidgetItem::ServersListWidgetItem(NetHub::IPPortPassword* p_IPPortPassword,
											 bool bIsIPv4, char *p_chName, QListWidget* p_ListWidget) : QListWidgetItem(p_ListWidget), m_chName("")
{
	if(p_chName)
	{
		CopyStrArray(p_chName, m_chName, SERVER_NAME_STR_LEN);
	}
	CopyStrArray(p_IPPortPassword->p_chIPNameBuffer, m_chIP, IP_STR_LEN);
	CopyStrArray(p_IPPortPassword->p_chPortNameBuffer, m_chPort, PORT_STR_LEN);
	CopyStrArray(p_IPPortPassword->p_chPasswordNameBuffer, m_chPassword, AUTH_PASSWORD_STR_LEN);
	if(bIsIPv4)
	{
		if(m_chName[0])
		{
			this->setText(QString(m_chName));
			this->setToolTip(QString(m_chName) + " - " + QString(m_chIP) + ":" + QString(m_chPort));
		}
		else
		{
			this->setText(QString(m_chIP) + ":" + QString(m_chPort));
		}
	}
	else
	{
		if(m_chName[0])
		{
			this->setText(QString(m_chName));
			this->setToolTip(QString(m_chName) + " - [" + QString(m_chIP) + "]:" + QString(m_chPort));
		}
		else
		{
			this->setText("[" + QString(m_chIP) + "]:" + QString(m_chPort));
		}
	}
}

// Получение указателя строку с именем сервера или 0 при пустой строке.
char* ServersListWidgetItem::GetName()
{
	if(m_chName[0])
	{
		return &m_chName[0];
	}
	return 0;
}

//== Класс главного окна.
// Конструктор.
MainWindow::MainWindow(QWidget* p_parent) :
	QMainWindow(p_parent)
{
	// Для избежания ошибки при доступе из другого потока.
	qRegisterMetaType<QVector<int>>("QVector<int>");
	//
	p_This = this;
	LOG_CTRL_INIT;
	LOG_P_0(LOG_CAT_I, m_chLogStart);
	iInitRes = RETVAL_OK;
	//
	connect(this, SIGNAL(RemoteSlotSetConnectionButtonsState(bool)), this, SLOT(SlotSetConnectionButtonsState(bool)));
	connect(this, SIGNAL(RemoteSlotMsgDialog(QString, QString)), this, SLOT(SlotMsgDialog(QString, QString)));
	connect(this, SIGNAL(RemoteSlotClientStopProcedures()), this, SLOT(SlotClientStopProcedures()));
	//
	p_UISettings = new QSettings(cp_chUISettingsName, QSettings::IniFormat);
	p_ui->setupUi(this);
	connect(this, SIGNAL(RemoteClearScene()), p_SchematicWindow, SLOT(ClearScene()));
	connect(this, SIGNAL(RemoteUpdateSchView()), p_SchematicWindow, SLOT(UpdateScene()));
	p_QLabelStatusBarText = new QLabel(this);
	p_QLabelStatusBarText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	p_ui->statusBar->addWidget(p_QLabelStatusBarText);
	if(IsFileExists((char*)cp_chUISettingsName))
	{
		LOG_P_1(LOG_CAT_I, m_chLogRestoreUI);
		// Splitters.

		// MainWidow.
		if(!restoreGeometry(p_UISettings->value("Geometry").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, m_chLogNoGeometryState);
			RETVAL_SET(RETVAL_ERR);
		}
		if(!restoreState(p_UISettings->value("WindowState").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, m_chLogNoWindowState);
			RETVAL_SET(RETVAL_ERR);
		}
	}
	else
	{
		LOG_P_0(LOG_CAT_W, m_chLogMainWindowIniAbsent);
	}
	if(!LoadClientConfig())
	{
		iInitRes = RETVAL_ERR;
		RETVAL_SET(RETVAL_ERR);
		return;
	}
	BlockSchematic(true);
	p_SchematicWindow->p_SchematicView->bLoading = true;
	p_Client = new Client(LOG_MUTEX);
	p_Client->SetServerCommandArrivedCB(ServerCommandArrivedCallback);
	p_Client->SetServerDataArrivedCB(ServerDataArrivedCallback);
	p_Client->SetServerStatusChangedCB(ServerStatusChangedCallback);
	//
	bSchemaIsOpened = p_UISettings->value("Schema").toBool();
	p_ui->action_Schematic->setChecked(bSchemaIsOpened);
	if(p_UISettings->value("AutoConnection").toBool())
	{
		p_ui->action_ConnectAtStartup->setChecked(true);
		p_ui->pushButton_Connect->click();
	}
	else
	{
		SetStatusBarText(m_chStatusReady);
	}
#ifndef WIN32
	// Из-за глюков алигмента Qt на linux.
	p_ui->groupBox_CurrentServer->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 6 5px;}");
	p_ui->groupBox_AvailableServers->setStyleSheet("QGroupBox::title {subcontrol-position: top left; padding: 6 5px;}");
#endif
	p_WidgetsThrAccess = new WidgetsThrAccess(p_ui);
}

// Деструктор.
MainWindow::~MainWindow()
{
	delete p_WidgetsThrAccess;
	if(RETVAL == RETVAL_OK)
	{
		LOG_P_0(LOG_CAT_I, m_chLogExit);
	}
	LOG_CLOSE;
	delete p_ui;
}

// Процедуры при закрытии окна приложения.
void MainWindow::closeEvent(QCloseEvent *p_Event)
{
	SetStatusBarText(m_chStatusShutdown);
	if(p_Client)
	{
		if(p_Client->CheckServerAlive())
		{
			SlotClientStopProcedures();
		}
		delete p_Client;
	}
	// Main.
	p_UISettings->setValue("Geometry", saveGeometry());
	p_UISettings->setValue("WindowState", saveState());
	// Splitters.

	//
	if(p_SchematicWindow != 0)
	{
		if(p_SchematicWindow->isVisible())
			p_SchematicWindow->RefClose();
	}
	QMainWindow::closeEvent(p_Event);
}

// Для внешнего отключения чекбокса кнопки 'Схема'.
void MainWindow::UncheckSchemaCheckbox()
{
	p_ui->action_Schematic->setChecked(false);
	p_UISettings->setValue("Schema", false);
}

// Кэлбэк обработки отслеживания статута сервера.
void MainWindow::ServerStatusChangedCallback(bool bStatus)
{
	emit p_This->RemoteSlotSetConnectionButtonsState(bStatus);
}

// Кэлбэк обработки прихода команд от сервера.
void MainWindow::ServerCommandArrivedCallback(unsigned short ushCommand)
{
	LOG_P_1(LOG_CAT_I, "Server command: [" << ushCommand << "].");
	switch(chLastClientRequest)
	{
		case CLIENT_REQUEST_CONNECT:
		{
			switch(ushCommand)
			{
				case PROTO_S_PASSW_OK:
				{
					break;
				}
				case PROTO_S_PASSW_ERR:
				{
					LOG_P_0(LOG_CAT_W, "Wrong password.");
					emit p_This->RemoteSlotMsgDialog(m_chMsgError, "Неверный пароль.");
					emit p_This->RemoteSlotClientStopProcedures();
					break;
				}
			}
			break;
		}
		case CLIENT_REQUEST_DISCONNECT:
		{
			switch(ushCommand)
			{
				case PROTO_S_ACCEPT_LEAVING:
				{
					break;
				}
			}
			break;
		}
		case CLIENT_REQUEST_UNDEFINED:
		{
			switch(ushCommand)
			{
				case PROTO_S_SHUTDOWN_INFO:
				{
					emit p_This->RemoteSlotMsgDialog("Информация", "Сервер отключил клиенты.");
gD:					emit p_This->RemoteSlotSetConnectionButtonsState(false);
					BlockSchematic(true);
					break;
				}
				case PROTO_S_KICK:
				{
					emit p_This->RemoteSlotMsgDialog(m_chMsgWarning, "Сервер отключил клиент.");
					goto gD;
				}
			}
			break;
		}
	}
	chLastClientRequest = CLIENT_REQUEST_UNDEFINED;
}

// Кэлбэк обработки прихода пакетов от сервера.
void MainWindow::ServerDataArrivedCallback(unsigned short ushType, void* p_ReceivedData, int iPocket)
{
	bool bProcessed = false;
	//
	switch(ushType)
	{
		//========  Раздел PROTO_O_SCH_STATUS. ========
		case PROTO_O_SCH_STATUS: // По приходу ответа о статусе среды...
		{
			if(p_ReceivedData != 0)
			{
				PSchStatusInfo oPSchStatusInfo;
				//
				oPSchStatusInfo = *static_cast<PSchStatusInfo*>(p_ReceivedData);
				if(oPSchStatusInfo.uchBits & SCH_STATUS_LOADED)
				{
					SchematicView::AfterLoadingPlacement();
					BlockSchematic(BLOCKING_OFF);
					p_SchematicWindow->p_SchematicView->bLoading = false;
					SchematicView::CreateBackground();
					p_SchematicWindow->oScene.update();
					LOG_P_1(LOG_CAT_I, "Loading completed.");
					goto gLO;
				}
				if(oPSchStatusInfo.uchBits & SCH_STATUS_READY) // Если вкл. - очистка сцены и отправка сообщ. о готовности клиента начать загр.
				{
					LOG_P_1(LOG_CAT_I, "Hub is alive.");
					p_SchematicWindow->bCleaningSceneNow = true;
					emit p_This->RemoteClearScene();
					QCoreApplication::processEvents();
					while(p_SchematicWindow->bCleaningSceneNow) // Ждём, пока сигнал дойдёт и флаг очистки сбросится слотом.
					{
						MSleep(INTERFACE_RESPONSE_MS);
					}
					SchematicView::dbObjectZPos = 1;
					RemoteUpdateSchViewAndSendRFrame();
					p_SchematicWindow->p_SchematicView->bLoading = true;
					LOG_P_1(LOG_CAT_I, "Loading...");
				}
				else // Если отключена - блокировка схем.
				{
					LOG_P_1(LOG_CAT_I, "Hub is inactive.");
					BlockSchematic(true);
				}
			}
gLO:		bProcessed = true;
			break;
		}
		//========  Раздел PROTO_S_SERVER_NAME. ========
		case PROTO_S_SERVER_NAME:
		{
			PSchStatusInfo oPSchReadyInfo;
			//
			if(p_ReceivedData != 0)
			{
				if(QString(oPServerName.m_chServerName) != QString((char*)p_ReceivedData))
				{
					memcpy(oPServerName.m_chServerName, p_ReceivedData, sizeof(PServerName)); // Загрузка в соотв. структуру.
					LCHECK_BOOL(SaveClientConfig());
				}
				LOG_P_1(LOG_CAT_I, "Server name: [" << oPServerName.m_chServerName << "].");
				p_ui->label_CurrentServer->setText(QString(oPServerName.m_chServerName));
				oPSchReadyInfo.uchBits = SCH_STATUS_READY;
				// По приходу имени сервера, ясно, что авторизация прошла успешно. Даётся запрос про статус среды.
				p_Client->SendToServerImmediately(PROTO_O_SCH_STATUS, (char*)&oPSchReadyInfo, sizeof(PSchStatusInfo), true, false);
			}
			else
			{
				LOG_P_0(LOG_CAT_W, "Wrong data in pocket.");
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_ELEMENT_BASE. ========
		case PROTO_O_SCH_ELEMENT_BASE:
		{
			if(p_ReceivedData != 0)
			{
				PSchElementBase oPSchElementBase;
				GraphicsElementItem* p_GraphicsElementItem;
				GraphicsGroupItem* p_GraphicsGroupItem;
				bool bGroupFounded = false;
				//
				oPSchElementBase = *static_cast<PSchElementBase*>(p_ReceivedData);
				LOG_P_2(LOG_CAT_I, "{In} Element base [" << QString(oPSchElementBase.m_chName).toStdString() << "].");
				for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++)
				{
					if(SchematicWindow::vp_Elements.at(iF)->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
							oPSchElementBase.oPSchElementVars.ullIDInt)
					{
						LOG_P_0(LOG_CAT_W, "Duplicate element arrived, ignored.");
						goto gP;
					}
				}
				p_WidgetsThrAccess->p_PSchElementBase = &oPSchElementBase;
				p_WidgetsThrAccess->p_ConnGraphicsElementItem = 0;
				ThrUiAccessET(p_WidgetsThrAccess, AddGraphicsElementItem);
				while(p_WidgetsThrAccess->p_ConnGraphicsElementItem == 0)
				{
					MSleep(INTERFACE_RESPONSE_MS);
				}
				p_GraphicsElementItem = p_WidgetsThrAccess->p_ConnGraphicsElementItem;
				p_GraphicsElementItem->setZValue(oPSchElementBase.oPSchElementVars.oSchEGGraph.dbObjectZPos);
				if(bBlockingGraphics)
				{
					SchematicView::SetElementBlockingPattern(p_GraphicsElementItem, BLOCKING_ON);
				}
				SchematicWindow::vp_Elements.push_front(p_GraphicsElementItem);
				if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
				{
					for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++)
					{
						p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
						if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt ==
								p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup)
						{
							p_GraphicsGroupItem->vp_ConnectedElements.push_front(p_GraphicsElementItem);
							p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
							bGroupFounded = true;
							if(p_GraphicsElementItem->oPSchElementBaseInt.bRequestGroupUpdate)
							{
								SchematicView::UpdateGroupFrameByContentRecursivelyUpstream(p_GraphicsGroupItem);
							}
							break;
						}
					}
					if(!bGroupFounded)
					{
						SchematicWindow::vp_LonelyElements.push_front(p_GraphicsElementItem);
					}
				}
gP:				if(oPSchElementBase.oPSchElementVars.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_ELEMENT_VARS. ========
		case PROTO_O_SCH_ELEMENT_VARS:
		{
			if(p_ReceivedData != 0)
			{
				PSchElementVars oPSchElementVars;
				GraphicsElementItem* p_GraphicsElementItem;
				//
				oPSchElementVars = *static_cast<PSchElementVars*>(p_ReceivedData);
				for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++)
				{
					p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
					if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
							oPSchElementVars.ullIDInt)
					{
						// Установка новых параметров.
						LOG_P_2(LOG_CAT_I, "{In} Element vars [" <<
								QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "].");
						IncomingUpdateElementParameters(p_GraphicsElementItem, oPSchElementVars);
						goto gC;
					}
				}
				// Не нашлось в имеющихся ID - ошибка.
				LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
				//
gC:				if(oPSchElementVars.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_ELEMENT_NAME. ========
		case PROTO_O_SCH_ELEMENT_NAME:
		{
			if(p_ReceivedData != 0)
			{
				PSchElementName oPSchElementName;
				GraphicsElementItem* p_GraphicsElementItem;
				//
				oPSchElementName = *static_cast<PSchElementName*>(p_ReceivedData);
				LOG_P_2(LOG_CAT_I, "{In} Element name [" << QString(oPSchElementName.m_chName).toStdString() << "].");
				for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++)
				{
					p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
					if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
							oPSchElementName.ullIDInt)
					{
						// Установка нового имени.
						CopyStrArray(oPSchElementName.m_chName, p_GraphicsElementItem->oPSchElementBaseInt.m_chName, SCH_OBJ_NAME_STR_LEN);
						p_GraphicsElementItem->p_QGroupBox->setTitle(p_GraphicsElementItem->oPSchElementBaseInt.m_chName);
						goto gN;
					}
				}
				// Не нашлось в имеющихся ID - ошибка.
				LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
gN:				if(oPSchElementName.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_ELEMENT_COLOR. ========
		case PROTO_O_SCH_ELEMENT_COLOR:
		{
			if(p_ReceivedData != 0)
			{
				PSchElementColor oPSchElementColor;
				GraphicsElementItem* p_GraphicsElementItem;
				//
				oPSchElementColor = *static_cast<PSchElementColor*>(p_ReceivedData);
				LOG_P_2(LOG_CAT_I, "{In} Element color.");
				for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++)
				{
					p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
					if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
							oPSchElementColor.ullIDInt)
					{
						// Установка нового цвета.
						p_GraphicsElementItem->oPSchElementBaseInt.uiObjectBkgColor = oPSchElementColor.uiObjectBkgColor;
						p_GraphicsElementItem->oQBrush.setColor(QRgb(p_GraphicsElementItem->oPSchElementBaseInt.uiObjectBkgColor));
						goto gNC;
					}
				}
				// Не нашлось в имеющихся ID - ошибка.
				LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
gNC:			if(oPSchElementColor.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_LINK_BASE. ========
		case PROTO_O_SCH_LINK_BASE:
		{
			if(p_ReceivedData != 0)
			{
				PSchLinkBase oPSchLinkBase;
				GraphicsLinkItem* p_GraphicsLinkItem;
				GraphicsLinkItem* p_WaitingHelper = nullptr;
				//
				oPSchLinkBase = *static_cast<PSchLinkBase*>(p_ReceivedData);
				for(int iF = 0; iF < SchematicWindow::vp_Links.count(); iF++)
				{
					p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iF);
					if((p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc ==
						oPSchLinkBase.oPSchLinkVars.ullIDSrc) &&
							(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
							 oPSchLinkBase.oPSchLinkVars.ullIDDst) &&
							(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort ==
							 oPSchLinkBase.oPSchLinkVars.ushiSrcPort) &&
							(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort ==
							 oPSchLinkBase.oPSchLinkVars.ushiDstPort))
					{
						LOG_P_0(LOG_CAT_W, "Duplicate link arrived, ignored.");
						goto gL;
					}
				}
				p_WidgetsThrAccess->p_PSchLinkBase = &oPSchLinkBase;
				p_WidgetsThrAccess->p_ConnGraphicsLinkItem = 0;
				ThrUiAccessET(p_WidgetsThrAccess, AddGraphicsLinkItem);
				p_WidgetsThrAccess->p_ConnGraphicsLinkItem = (GraphicsLinkItem*) -1;
				p_WaitingHelper = p_WidgetsThrAccess->p_ConnGraphicsLinkItem;
				while(p_WidgetsThrAccess->p_ConnGraphicsLinkItem == p_WaitingHelper)
				{
					MSleep(INTERFACE_RESPONSE_MS);
				}
				if (p_WidgetsThrAccess->p_ConnGraphicsLinkItem == nullptr)
				{
					LOG_P_0(LOG_CAT_W, "Incorrect link arrived, ignored.");
					goto gL;
				}
				p_GraphicsLinkItem = p_WidgetsThrAccess->p_ConnGraphicsLinkItem;
				LOG_P_2(LOG_CAT_I, "{In} Link base [" <<
						QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName).toStdString()
						<< "<>" << QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName).toStdString()
						<< "].");
				SchematicWindow::vp_Links.push_front(p_GraphicsLinkItem);
				SchematicView::UpdateLinkZPositionByElements(p_GraphicsLinkItem);
gL:				if(oPSchLinkBase.oPSchLinkVars.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_LINK_VARS. ========
		case PROTO_O_SCH_LINK_VARS:
		{
			if(p_ReceivedData != 0)
			{
				PSchLinkVars oPSchLinkVars;
				GraphicsLinkItem* p_GraphicsLinkItem;
				//
				oPSchLinkVars = *static_cast<PSchLinkVars*>(p_ReceivedData);
				for(int iF = 0; iF < SchematicWindow::vp_Links.count(); iF++)
				{
					p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iF);
					if((p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc ==
						oPSchLinkVars.ullIDSrc) &&
							(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
							 oPSchLinkVars.ullIDDst) &&
							(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort ==
							 oPSchLinkVars.ushiSrcPort) &&
							(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort ==
							 oPSchLinkVars.ushiDstPort))
					{
						// Установка новых параметров.
						LOG_P_2(LOG_CAT_I, "{In} Link vars [" <<
								QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName).toStdString() << "<>" <<
								QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName).toStdString()
								<< "].");
						MainWindow::IncomingUpdateLinkParameters(p_GraphicsLinkItem, oPSchLinkVars);
						goto gS;
					}
				}
				// Не нашлось в имеющихся ID - ошибка.
				LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
				//
gS:				if(oPSchLinkVars.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_GROUP_BASE. ========
		case PROTO_O_SCH_GROUP_BASE:
		{
			if(p_ReceivedData != 0)
			{
				PSchGroupBase oPSchGroupBase;
				GraphicsGroupItem* p_GraphicsGroupItem;
				GraphicsGroupItem* p_GraphicsGroupItemInt;
				GraphicsGroupItem* p_GraphicsGroupItemIntHelper;
				GraphicsElementItem* p_GraphicsElementItem;
				GraphicsElementItem* p_GraphicsElementItemInt;
				bool bGroupFounded = false;
				//
				oPSchGroupBase = *static_cast<PSchGroupBase*>(p_ReceivedData);
				LOG_P_2(LOG_CAT_I, "{In} Group base [" << QString(oPSchGroupBase.m_chName).toStdString() << "].");
				for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++)
				{
					if(SchematicWindow::vp_Groups.at(iF)->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt ==
							oPSchGroupBase.oPSchGroupVars.ullIDInt)
					{
						LOG_P_0(LOG_CAT_W, "Duplicate group arrived, ignored.");
						goto gG;
					}
				}
				p_WidgetsThrAccess->p_PSchGroupBase = &oPSchGroupBase;
				p_WidgetsThrAccess->p_ConnGraphicsGroupItem = 0;
				ThrUiAccessET(p_WidgetsThrAccess, AddGraphicsGroupItem);
				while(p_WidgetsThrAccess->p_ConnGraphicsGroupItem == 0)
				{
					MSleep(INTERFACE_RESPONSE_MS);
				}
				p_GraphicsGroupItem = p_WidgetsThrAccess->p_ConnGraphicsGroupItem;
				if(bBlockingGraphics)
				{
					SchematicView::SetGroupBlockingPattern(p_GraphicsGroupItem, BLOCKING_ON);
				}
				p_GraphicsGroupItem->setZValue(oPSchGroupBase.oPSchGroupVars.oSchEGGraph.dbObjectZPos);
				SchematicWindow::vp_Groups.push_front(p_GraphicsGroupItem);
				// Добавение в группу привязанных элементов.
				for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++) // По всем элементам.
				{
					p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
					if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup ==
							p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt) // Если элемент принадлежит группе...
					{
						bool bAlreadyPresent = false;
						for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++) // По всем вписанным элементам.
						{
							if(p_GraphicsGroupItem->vp_ConnectedElements.at(iF)->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
									p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt) // Если уже вписан - игнор.
							{
								bAlreadyPresent = true;
								break;
							}
						}
						if(bAlreadyPresent == false) // Если новый для группы...
						{
							p_GraphicsGroupItem->vp_ConnectedElements.push_front(p_GraphicsElementItem); // Добавление в группу.
							if(p_GraphicsElementItem->oPSchElementBaseInt.bRequestGroupUpdate)
							{
								SchematicView::UpdateGroupFrameByContentRecursivelyUpstream(p_GraphicsGroupItem);
							}
							p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
							// По всем элементам в ожидании групп.
							for(int iF = 0; iF < SchematicWindow::vp_LonelyElements.count(); iF++)
							{
								p_GraphicsElementItemInt = SchematicWindow::vp_LonelyElements.at(iF);
								if(p_GraphicsElementItemInt == p_GraphicsElementItem)
									// Если присоединённый элемент был в списке ожидающих...
								{
									// Удаление из ожидающих, счётчик на один назад.
									SchematicWindow::vp_LonelyElements.removeAt(iF);
									iF--;
								}
							}
						}
					}
				}
				// Добавление в остальные группы всех ожидающих привязанных элементов (подстраховка от рассинхрона).
				if(!SchematicWindow::vp_LonelyElements.isEmpty()) // Если вообще есть ожидающие...
				{
					for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++) // По всем группам...
					{
						p_GraphicsGroupItemInt = SchematicWindow::vp_Groups.at(iF);
						if(p_GraphicsGroupItemInt != p_GraphicsGroupItem) // ... кроме текущей.
						{
							// По всем элементам в ожидании групп (одиноким).
							for(int iF = 0; iF < SchematicWindow::vp_LonelyElements.count(); iF++)
							{
								p_GraphicsElementItem = SchematicWindow::vp_LonelyElements.at(iF);
								if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup ==
										p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt)
									// Если одинокий элемент принадлежит текущей группе...
								{
									bool bAlreadyPresent = false;
									for(int iF = 0; iF < p_GraphicsGroupItemInt->vp_ConnectedElements.count(); iF++)
										// По всем вписанным в текущую элементам.
									{
										if(p_GraphicsGroupItemInt->vp_ConnectedElements.at(iF)->
												oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
												p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
											// Если уже вписан - игнор.
										{
											bAlreadyPresent = true;
											break;
										}
									}
									if(bAlreadyPresent == false) // Если новый для группы...
									{
										// Добавление в группу.
										p_GraphicsGroupItemInt->vp_ConnectedElements.push_front(p_GraphicsElementItem);
										p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
										// Удаление из ожидающих, счётчик на один назад.
										SchematicWindow::vp_LonelyElements.removeAt(iF);
										iF--;
									}
								}
							}
						}
					}
				}
				// Добавение группы в группы.
				if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup != 0)
				{
					for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++) // По всем группам...
					{
						p_GraphicsGroupItemInt = SchematicWindow::vp_Groups.at(iF);
						if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup ==
						   p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt) // Если группа принадлежит группе...
						{
							p_GraphicsGroupItemInt->vp_ConnectedGroups.push_front(p_GraphicsGroupItem); // Добавление в группу.
							if(p_GraphicsGroupItem->oPSchGroupBaseInt.bRequestGroupUpdate)
							{
								SchematicView::UpdateGroupFrameByContentRecursivelyUpstream(p_GraphicsGroupItemInt);
							}
							p_GraphicsGroupItem->p_GraphicsGroupItemRel = p_GraphicsGroupItemInt;
							bGroupFounded = true;
							// По всем группам в ожидании групп.
							for(int iF = 0; iF < SchematicWindow::vp_LonelyGroups.count(); iF++)
							{
								p_GraphicsGroupItemInt = SchematicWindow::vp_LonelyGroups.at(iF);
								if(p_GraphicsGroupItem == p_GraphicsGroupItemInt)
									// Если присоединённая группа была в списке ожидающих...
								{
									// Удаление из ожидающих, счётчик на один назад.
									SchematicWindow::vp_LonelyGroups.removeAt(iF);
									iF--;
								}
							}
						}
					}
					if(!bGroupFounded)
					{
						SchematicWindow::vp_LonelyGroups.push_front(p_GraphicsGroupItem);
					}
				}

				// Добавление в остальные группы всех ожидающих привязанных групп (подстраховка от рассинхрона).
				if(!SchematicWindow::vp_LonelyGroups.isEmpty()) // Если вообще есть ожидающие...
				{
					for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++) // По всем группам...
					{
						p_GraphicsGroupItemInt =
								SchematicWindow::vp_Groups.at(iF); // В p_GraphicsGroupItemInt - текущая группа для сравнения с одинокой.
						// По всем группам в ожидании групп (одиноким).
						for(int iF = 0; iF < SchematicWindow::vp_LonelyGroups.count(); iF++)
						{
							p_GraphicsGroupItemIntHelper =
									SchematicWindow::vp_LonelyGroups.at(iF); // В p_GraphicsGroupItemIntHelper - текущая одинок.
							if(p_GraphicsGroupItemIntHelper->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup ==
							   p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt)
								// Если одинокая группа принадлежит текущей группе...
							{
								bool bAlreadyPresent = false;
								for(int iF = 0; iF < p_GraphicsGroupItemInt->vp_ConnectedGroups.count(); iF++)
									// По всем вписанным в текущую группу группам.
								{
									if(p_GraphicsGroupItemInt->vp_ConnectedGroups.at(iF)->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt ==
									   p_GraphicsGroupItemIntHelper->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt)
										// Если уже вписана - игнор.
									{
										bAlreadyPresent = true;
										break;
									}
								}
								if(bAlreadyPresent == false) // Если новый для группы...
								{
									// Добавление в группу.
									p_GraphicsGroupItemInt->vp_ConnectedGroups.push_front(p_GraphicsGroupItemIntHelper);
									p_GraphicsGroupItemIntHelper->p_GraphicsGroupItemRel = p_GraphicsGroupItemInt;
									// Удаление из ожидающих, счётчик на один назад.
									SchematicWindow::vp_LonelyGroups.removeAt(iF);
									iF--;
								}
							}
						}
					}
				}
				//
gG:				if(oPSchGroupBase.oPSchGroupVars.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_GROUP_VARS. ========
		case PROTO_O_SCH_GROUP_VARS:
		{
			if(p_ReceivedData != 0)
			{
				PSchGroupVars oPSchGroupVars;
				GraphicsGroupItem* p_GraphicsGroupItem;
				//
				oPSchGroupVars = *static_cast<PSchGroupVars*>(p_ReceivedData);
				for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++)
				{
					p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
					if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt ==
							oPSchGroupVars.ullIDInt)
					{
						// Установка новых параметров.
						LOG_P_2(LOG_CAT_I, "{In} Group vars [" <<
								QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "].");
						IncomingUpdateGroupParameters(p_GraphicsGroupItem, oPSchGroupVars);
						goto gI;
					}
				}
				// Не нашлось в имеющихся ID - ошибка.
				LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
				//
gI:				if(oPSchGroupVars.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_GROUP_NAME. ========
		case PROTO_O_SCH_GROUP_NAME:
		{
			if(p_ReceivedData != 0)
			{
				PSchGroupName oPSchGroupName;
				GraphicsGroupItem* p_GraphicsGroupItem;
				//
				oPSchGroupName = *static_cast<PSchGroupName*>(p_ReceivedData);
				LOG_P_2(LOG_CAT_I, "{In} Group name [" << QString(oPSchGroupName.m_chName).toStdString() << "].");
				for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++)
				{
					p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
					if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt ==
							oPSchGroupName.ullIDInt)
					{
						// Установка нового имени.
						CopyStrArray(oPSchGroupName.m_chName, p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName, SCH_OBJ_NAME_STR_LEN);
						p_GraphicsGroupItem->p_QLabel->setText(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName);
						goto gGN;
					}
				}
				// Не нашлось в имеющихся ID - ошибка.
				LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
gGN:			if(oPSchGroupName.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_GROUP_COLOR. ========
		case PROTO_O_SCH_GROUP_COLOR:
		{
			if(p_ReceivedData != 0)
			{
				PSchGroupColor oPSchGroupColor;
				GraphicsGroupItem* p_GraphicsGroupItem;
				//
				oPSchGroupColor = *static_cast<PSchGroupColor*>(p_ReceivedData);
				LOG_P_2(LOG_CAT_I, "{In} Group color.");
				for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++)
				{
					p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
					if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt ==
							oPSchGroupColor.ullIDInt)
					{
						// Установка нового цвета.
						p_GraphicsGroupItem->oPSchGroupBaseInt.uiObjectBkgColor = oPSchGroupColor.uiObjectBkgColor;
						p_GraphicsGroupItem->oQBrush.setColor(QRgb(p_GraphicsGroupItem->oPSchGroupBaseInt.uiObjectBkgColor));
						goto gGC;
					}
				}
				// Не нашлось в имеющихся ID - ошибка.
				LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
gGC:			if(oPSchGroupColor.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_ELEMENT_ERASE. ========
		case PROTO_O_SCH_ELEMENT_ERASE:
		{
			if(p_ReceivedData != 0)
			{
				PSchElementEraser oPSchElementEraser;
				GraphicsElementItem* p_GraphicsElementItem = nullptr;
				//
				oPSchElementEraser = *static_cast<PSchElementEraser*>(p_ReceivedData);
				// Работа с линками.
				EraseLinksFromElement(oPSchElementEraser.ullIDInt);
				// Работа с элементами.
				for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++) // Поиск по всем элементам...
				{
					p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
					if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
							oPSchElementEraser.ullIDInt) // Найден удаляемый элемент...
					{
						LOG_P_2(LOG_CAT_I, "{In} Erase element [" <<
								QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "].");
						// Работа с группой.
						if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
						{
							for(int iEGC = 0; iEGC < p_GraphicsElementItem->p_GraphicsGroupItemRel->vp_ConnectedElements.count(); iEGC++)
							{
								if(p_GraphicsElementItem->p_GraphicsGroupItemRel->vp_ConnectedElements.at(iEGC)->oPSchElementBaseInt.
										oPSchElementVars.ullIDInt == oPSchElementEraser.ullIDInt)
								{
									p_GraphicsElementItem->p_GraphicsGroupItemRel->vp_ConnectedElements.removeAt(iEGC);
									SchematicView::GroupCheckEmptyAndRemoveRecursively(p_GraphicsElementItem->p_GraphicsGroupItemRel);
									break;
								}
							}
						}
						// Удалене связанных граф. портов.
						SchematicView::RemovePortsByID(oPSchElementEraser.ullIDInt);
						// Удаление граф. элемента.
						MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem);
						SchematicWindow::vp_Elements.removeAt(iF);
						break;
					}
					p_GraphicsElementItem = nullptr;
				}
				if(p_GraphicsElementItem == nullptr)
				{
					LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
				}
				if(oPSchElementEraser.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_GROUP_ERASE. ========
		case PROTO_O_SCH_GROUP_ERASE:
		{
			if(p_ReceivedData != 0)
			{
				PSchGroupEraser oPSchGroupEraser;
				GraphicsGroupItem* p_GraphicsGroupItem = nullptr;
				GraphicsElementItem* p_GraphicsElementItem = nullptr;
				//
				oPSchGroupEraser = *static_cast<PSchGroupEraser*>(p_ReceivedData);
				// Поиск группы.
				for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++) // Поиск по всем группам...
				{
					p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
					if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt ==
							oPSchGroupEraser.ullIDInt) // Найдена удаляемая группа...
					{
						LOG_P_2(LOG_CAT_I, "{In} Erase group [" <<
								QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "].");
						// Удаление граф. группы.
						for(int iE = 0; iE < p_GraphicsGroupItem->vp_ConnectedElements.count(); iE++)
						{
							p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iE);
							EraseLinksFromElement(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt);
							SchematicWindow::vp_Elements.removeOne(p_GraphicsElementItem);
							MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem);
						}
						MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsGroupItem);
						SchematicWindow::vp_Groups.removeAt(iF);
						break;
					}
					p_GraphicsGroupItem = nullptr;
				}
				if(p_GraphicsGroupItem == nullptr)
				{
					LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
				}
				if(oPSchGroupEraser.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_PSEUDONYM. ========
		case PROTO_O_SCH_PSEUDONYM:
		{
			if(p_ReceivedData != 0)
			{
				PSchPseudonym oPSchPseudonym;
				//
				oPSchPseudonym = *static_cast<PSchPseudonym*>(p_ReceivedData);
				LOG_P_2(LOG_CAT_I, "{In} Pseudonym [" << QString(oPSchPseudonym.m_chName).toStdString() << "].");
				for(int iF = 0; iF < SchematicWindow::v_PSchPseudonyms.count(); iF++)
				{
					if(SchematicWindow::v_PSchPseudonyms.at(iF).ushiPort ==
							oPSchPseudonym.ushiPort)
					{
						SchematicWindow::v_PSchPseudonyms.removeAt(iF);
						LOG_P_0(LOG_CAT_W, "Pseudonym has been replaced by incoming.");
					}
				}
				SchematicWindow::v_PSchPseudonyms.append(oPSchPseudonym);
				if(oPSchPseudonym.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_GROUP_ERASE. ========
		case PROTO_O_SCH_PSEUDONYM_ERASE:
		{
			if(p_ReceivedData != 0)
			{
				PSchPseudonymEraser oPSchPseudonymEraser;
				const PSchPseudonym* p_PSchPseudonym = nullptr;
				//
				oPSchPseudonymEraser = *static_cast<PSchPseudonymEraser*>(p_ReceivedData);
				for(int iF = 0; iF < SchematicWindow::v_PSchPseudonyms.count(); iF++)
				{
					p_PSchPseudonym = &SchematicWindow::v_PSchPseudonyms.at(iF);
					if(p_PSchPseudonym->ushiPort == oPSchPseudonymEraser.ushiPort)
					{
						LOG_P_2(LOG_CAT_I, "{In} Erase pseudonym [" <<
								QString(p_PSchPseudonym->m_chName).toStdString() << "].");
						SchematicWindow::v_PSchPseudonyms.removeAt(iF);
						break;
					}
					p_PSchPseudonym = nullptr;
				}
				if(p_PSchPseudonym == nullptr)
				{
					LOG_P_0(LOG_CAT_W, m_chLogSyncFault);
				}
				if(oPSchPseudonymEraser.bLastInQueue)
				{
					p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
			bProcessed = true;
			break;
		}
		//========  Раздел PROTO_O_SCH_DATA. ========
		case PROTO_O_SCH_DATA:
		{
			if(p_ReceivedData != 0)
			{
				PSchData oPSchData;
				ElementData oElementData;
				int iPSchDataSize = sizeof(PSchData);
				int iPElementDataSize = sizeof(ElementData);
				char* p_chRD = static_cast<char*>(p_ReceivedData);
				GraphicsElementItem* p_GraphicsElementItem = nullptr;
				//
				oPSchData = *reinterpret_cast<PSchData*>(p_chRD);
				oElementData = *reinterpret_cast<ElementData*>(p_chRD + iPSchDataSize);
				oElementData.p_vData = p_chRD + iPSchDataSize + iPElementDataSize;
				LOG_P_0(LOG_CAT_I, "{In} Pocket arrived for element [" << oElementData.ullElementID << "] of type [" <<
						oElementData.ullID << "], size [" << oElementData.uiDataSize << "]");
				for(int iE = 0; iE != SchematicWindow::vp_Elements.count(); iE++)
				{
					GraphicsElementItem* p_GraphicsElementItemInt = SchematicWindow::vp_Elements.at(iE);
					//
					if(p_GraphicsElementItemInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt == oElementData.ullElementID)
					{
						p_GraphicsElementItem = p_GraphicsElementItemInt; // Найден запрашиваемый элемент.
						break;
					}
				}
				if(p_GraphicsElementItem == nullptr)
				{
					LOG_P_0(LOG_CAT_W, "Element requested by server is absent on client.");
					goto WS;
				}
				if(p_GraphicsElementItem->iLibraryNumber != NO_LIBRARY_CONNECTED)
				{
					const SchematicView::SchLibraryHub* p_SchLibraryHub =
							&p_SchematicWindow->p_SchematicView->v_SchLibraryHubs.at(p_GraphicsElementItem->iLibraryNumber);
					if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDDataType == p_SchLibraryHub->GetIDFromLibrary())
					{
						p_SchLibraryHub->ApplyDataFromServer(oElementData.ushiPort, oElementData.p_vData);
					}
					else
					{
						LOG_P_0(LOG_CAT_W, "Requested element can`t use received type of data.");
						goto WS;
					}
				}
				else
				{
					LOG_P_0(LOG_CAT_W, "Requested element don`t have visualizer library connected.");
					goto WS;
				}
			}
			else
			{
				LOG_P_0(LOG_CAT_W, m_chLogWrongData);
				p_SchematicWindow->p_MainWindow->RemoteUpdateSchViewAndSendRFrame();
			}
WS:			bProcessed = true;
			break;
		}
		// Следующий раздел...
	}
	//
	if(!bProcessed)
	{
		LOG_P_0(LOG_CAT_W, "Unknown data type from the server.");
	}
	LCHECK_INT(p_Client->ReleaseDataInPositionC(iPocket, DONT_TRY_LOCK));
	return;
}

// Загрузка конфигурации клиента.
bool MainWindow::LoadClientConfig()
{
	tinyxml2::XMLDocument xmlDocCConf;
	XMLError eResult;
	NetHub::IPPortPassword oIPPortPasswordHelper;
	char* p_chHelper = 0;
	static list<XMLNode*> l_pSelectedServer;
	static list<XMLNode*> l_pServers;
	NumericAddress oNumericAddress;
	//
	eResult = xmlDocCConf.LoadFile(C_CONF_PATH);
	if (eResult != XML_SUCCESS)
	{
		LOG_P_0(LOG_CAT_E, m_chLogCantOpenConfig << C_CONF_PATH << "].");
		return false;
	}
	else
	{
		LOG_P_1(LOG_CAT_I, "Configuration" << m_chLogIsLoaded);
	}
	LOG_P_1(LOG_CAT_I, "--- Current server info ---");
	if(!FindChildNodes(xmlDocCConf.LastChild(), l_pSelectedServer,
					   "Selected_server", FCN_ONE_LEVEL, FCN_FIRST_ONLY))
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Selected_server] node.");
		return false;
	}
	FIND_IN_CHILDLIST(l_pSelectedServer.front(), p_ListName, "Name",
					  FCN_ONE_LEVEL, p_NodeName)
	{
		p_chHelper = (char*)p_NodeName->FirstChild()->Value();
	} FIND_IN_CHILDLIST_END(p_ListName);
	if(p_chHelper != 0)
	{
		CopyStrArray(p_chHelper, oPServerName.m_chServerName, SERVER_NAME_STR_LEN);
		LOG_P_1(LOG_CAT_I, "Name: [" << oPServerName.m_chServerName << "].");
	}
	else
	{
		oPServerName.m_chServerName[0] = 0;
	}
	FIND_IN_CHILDLIST(l_pSelectedServer.front(), p_ListServerIP, "IP",
					  FCN_ONE_LEVEL, p_NodeServerIP)
	{
		p_chHelper = (char*)p_NodeServerIP->FirstChild()->Value();
	} FIND_IN_CHILDLIST_END(p_ListServerIP);
	if(p_chHelper != 0)
	{

		CopyStrArray(p_chHelper, m_chIPInt, IP_STR_LEN);
#ifndef LOG_LEVEL_0
		if(NetHub::CheckIPv4(p_chHelper))
		{
			LOG_P_1(LOG_CAT_I, "IPv4: [" << m_chIPInt << "].");
		}
		else
		{
			LOG_P_1(LOG_CAT_I, "IPv6: [" << m_chIPInt << "].");
		}
#endif
	}
	else
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Selected_server]->[IP] node.");
		return false;
	}
	FIND_IN_CHILDLIST(l_pSelectedServer.front(), p_ListPort, "Port",
					  FCN_ONE_LEVEL, p_NodePort)
	{
		p_chHelper = (char*)p_NodePort->FirstChild()->Value();
	} FIND_IN_CHILDLIST_END(p_ListPort);
	if(p_chHelper != 0)
	{
		CopyStrArray(p_chHelper, m_chPortInt, PORT_STR_LEN);
		LOG_P_1(LOG_CAT_I, "Port: " << m_chPortInt);
	}
	else
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Selected_server]->[Port] node.");
		return false;
	}
	FIND_IN_CHILDLIST(l_pSelectedServer.front(), p_ListPassword, "Password",
					  FCN_ONE_LEVEL, p_NodePassword)
	{
		p_chHelper = (char*)p_NodePassword->FirstChild()->Value();
	} FIND_IN_CHILDLIST_END(p_ListPassword);
	if(p_chHelper == 0)
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Selected_server]->[Password] node.");
		return false;
	}
	else
	{
		CopyStrArray(p_chHelper, m_chPasswordInt, AUTH_PASSWORD_STR_LEN);
	}
	FillNumericStructWithIPPortStrs(oNumericAddress, QString(m_chIPInt), QString(m_chPortInt));
	if(!oNumericAddress.bIsCorrect)
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "Incorrect main adress format.");
		return false;
	}
	else
	{
		SetServerLabelData(oNumericAddress.bIsIPv4);
	}
	if(!FindChildNodes(xmlDocCConf.LastChild(), l_pServers,
					   "Servers", FCN_ONE_LEVEL, FCN_FIRST_ONLY))
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Servers] node.");
		return false;
	}
	LOG_P_1(LOG_CAT_I, "--- Stored servers info ---");
	PARSE_CHILDLIST(l_pServers.front(), p_ListServers, "Server",
					FCN_ONE_LEVEL, p_NodeServer)
	{
		p_chHelper = 0;
		FIND_IN_CHILDLIST(p_NodeServer, p_ListName, "Name",
						  FCN_ONE_LEVEL, p_NodeName)
		{
			p_chHelper = (char*)p_NodeName->FirstChild()->Value();
		} FIND_IN_CHILDLIST_END(p_ListName);
		if(p_chHelper != 0)
		{
			LOG_P_1(LOG_CAT_I, "Name: [" << p_chHelper << "].");
		}
		oIPPortPasswordHelper.p_chIPNameBuffer = 0;
		FIND_IN_CHILDLIST(p_NodeServer, p_ListServerIP, "IP",
						  FCN_ONE_LEVEL, p_NodeServerIP)
		{
			oIPPortPasswordHelper.p_chIPNameBuffer = (char*)p_NodeServerIP->FirstChild()->Value();
		} FIND_IN_CHILDLIST_END(p_ListServerIP);
		if(oIPPortPasswordHelper.p_chIPNameBuffer != 0)
		{
#ifndef LOG_LEVEL_0
			if(NetHub::CheckIPv4(oIPPortPasswordHelper.p_chIPNameBuffer))
			{
				LOG_P_1(LOG_CAT_I, "Server IPv4: [" << oIPPortPasswordHelper.p_chIPNameBuffer << "].");
			}
			else
			{
				LOG_P_1(LOG_CAT_I, "Server IPv6: [" << oIPPortPasswordHelper.p_chIPNameBuffer << "].");
			}
#endif
		}
		else
		{
			LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Server]->[IP]" << m_chLogNodeInList);
			return false;
		}
		oIPPortPasswordHelper.p_chPortNameBuffer = 0;
		FIND_IN_CHILDLIST(p_NodeServer, p_ListPort, "Port",
						  FCN_ONE_LEVEL, p_NodePort)
		{
			oIPPortPasswordHelper.p_chPortNameBuffer = (char*)p_NodePort->FirstChild()->Value();
		} FIND_IN_CHILDLIST_END(p_ListPort);
		if(oIPPortPasswordHelper.p_chPortNameBuffer != 0)
		{
			LOG_P_1(LOG_CAT_I, "Port: " << oIPPortPasswordHelper.p_chPortNameBuffer);
		}
		else
		{
			LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Server]->[Port]" << m_chLogNodeInList);
			return false;
		}
		oIPPortPasswordHelper.p_chPasswordNameBuffer = 0;
		FIND_IN_CHILDLIST(p_NodeServer, p_ListPassword, "Password",
						  FCN_ONE_LEVEL, p_NodePassword)
		{
			oIPPortPasswordHelper.p_chPasswordNameBuffer = (char*)p_NodePassword->FirstChild()->Value();
		} FIND_IN_CHILDLIST_END(p_ListPassword);
		if(oIPPortPasswordHelper.p_chPasswordNameBuffer == 0)
		{
			LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No [Server]->[Password]" << m_chLogNodeInList);
			return false;
		}
		LOG_P_1(LOG_CAT_I, "--- ---");
		if(!oNumericAddress.bIsCorrect)
		{
			LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "Incorrect adress format.");
			return false;
		}
		else
		{
			p_ui->listWidget_Servers->addItem(new ServersListWidgetItem(&oIPPortPasswordHelper,
																		oNumericAddress.bIsIPv4, p_chHelper));
		}
	} PARSE_CHILDLIST_END(p_ListServers);
	oIPPortPassword.p_chIPNameBuffer = m_chIPInt;
	oIPPortPassword.p_chPortNameBuffer = m_chPortInt;
	oIPPortPassword.p_chPasswordNameBuffer = m_chPasswordInt;
	p_ui->listWidget_Servers->setCurrentRow(0);
	return true;
}

// Сохранение конфигурации клиента.
bool MainWindow::SaveClientConfig()
{
	XMLError eResult;
	tinyxml2::XMLDocument xmlDocCConf;
	XMLNode* p_NodeRoot;
	XMLNode* p_NodeSelectedServer;
	XMLNode* p_NodeName;
	XMLNode* p_NodeServerIP;
	XMLNode* p_NodePort;
	XMLNode* p_NodePassword;
	XMLNode* p_NodeServers;
	XMLNode* p_NodeServer;
	//
	xmlDocCConf.InsertEndChild(xmlDocCConf.NewDeclaration());
	p_NodeRoot = xmlDocCConf.InsertEndChild(xmlDocCConf.NewElement("Root"));
	//
	p_NodeSelectedServer = p_NodeRoot->InsertEndChild(xmlDocCConf.NewElement("Selected_server"));
	if(oPServerName.m_chServerName[0] != 0)
	{
		p_NodeName = p_NodeSelectedServer->InsertEndChild(xmlDocCConf.NewElement("Name"));
		p_NodeName->ToElement()->SetText(oPServerName.m_chServerName);
	}
	p_NodeServerIP = p_NodeSelectedServer->InsertEndChild(xmlDocCConf.NewElement("IP"));
	p_NodeServerIP->ToElement()->SetText(m_chIPInt);
	p_NodePort = p_NodeSelectedServer->InsertEndChild(xmlDocCConf.NewElement("Port"));
	p_NodePort->ToElement()->SetText(m_chPortInt);
	p_NodePassword = p_NodeSelectedServer->InsertEndChild(xmlDocCConf.NewElement("Password"));
	p_NodePassword->ToElement()->SetText(m_chPasswordInt);
	//
	p_NodeServers = p_NodeRoot->InsertEndChild(xmlDocCConf.NewElement("Servers"));
	for(int iC = p_ui->listWidget_Servers->count() - 1; iC != -1; iC--)
	{
		char* p_chHelper;
		//
		p_NodeServer = p_NodeServers->InsertEndChild(xmlDocCConf.NewElement("Server"));
		p_chHelper = (static_cast<ServersListWidgetItem*>(p_ui->listWidget_Servers->item(iC)))->GetName();
		if(p_chHelper != 0)
		{
			p_NodeName = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Name"));
			p_NodeName->ToElement()->SetText(p_chHelper);
		}
		p_NodeServerIP = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("IP"));
		p_NodeServerIP->ToElement()->SetText(
					(static_cast<ServersListWidgetItem*>(p_ui->listWidget_Servers->item(iC)))->m_chIP);
		p_NodePort = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Port"));
		p_NodePort->ToElement()->SetText(
					(static_cast<ServersListWidgetItem*>(p_ui->listWidget_Servers->item(iC)))->m_chPort);
		p_NodePassword = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Password"));
		p_NodePassword->ToElement()->SetText(
					(static_cast<ServersListWidgetItem*>(p_ui->listWidget_Servers->item(iC)))->m_chPassword);
	}
	eResult = xmlDocCConf.SaveFile(C_CONF_PATH);
	if (eResult != XML_SUCCESS)
	{
		LOG_P_0(LOG_CAT_E, m_chLogCantSave << "client configuration data.");
		return false;
	}
	else return true;
}

// Установка имени и подсказки для метки текущего сервера.
void MainWindow::SetServerLabelData(bool bIsIPv4)
{
	if(bIsIPv4)
	{
		if(oPServerName.m_chServerName[0] != 0)
		{
			p_ui->label_CurrentServer->setText(QString(oPServerName.m_chServerName));
			p_ui->label_CurrentServer->
					setToolTip(QString(oPServerName.m_chServerName) + " - " + QString(m_chIPInt) + ":" + QString(m_chPortInt));
		}
		else
		{
			p_ui->label_CurrentServer->setText(QString(m_chIPInt) + ':' + QString(m_chPortInt));
		}
	}
	else
	{
		if(oPServerName.m_chServerName[0] != 0)
		{
			p_ui->label_CurrentServer->setText(QString(oPServerName.m_chServerName));
			p_ui->label_CurrentServer->
					setToolTip(QString(oPServerName.m_chServerName) + " - [" + QString(m_chIPInt) + "]:" + QString(m_chPortInt));
		}
		else
		{
			p_ui->label_CurrentServer->setText("[" + QString(m_chIPInt) + "]:" + QString(m_chPortInt));
		}
	}
}

// Процедуры запуска клиента.
void MainWindow::ClientStartProcedures()
{
	chLastClientRequest = CLIENT_REQUEST_CONNECT;
	SetStatusBarText("Запуск клиента...");
	if(!p_Client->Start(&oIPPortPassword))
	{
		goto gCA;
	}
	for(unsigned char uchAtt = 0; uchAtt != CLIENT_WAITING_ATTEMPTS; uchAtt++)
	{
		if(p_Client->CheckServerAlive())
		{
			SchematicView::p_GraphicsPortItemActive = nullptr;
			return;
		}
		MSleep(USER_RESPONSE_MS);
	}
gCA:LOG_P_0(LOG_CAT_W, m_chLogCantStart << "client.");
	emit p_This->RemoteSlotMsgDialog(m_chMsgWarning, "Соединение невозможно.");
	SetStatusBarText(m_chStatusReady);
}

// Проверка на совпадение цифровых IP.
bool MainWindow::CheckEqualsNumbers(NumericAddress& oNumericAddressOne, NumericAddress& oNumericAddressTwo)
{
	unsigned char uchL;
	//
	if(oNumericAddressOne.bIsIPv4 != oNumericAddressTwo.bIsIPv4)
	{
		return false;
	}
	if(oNumericAddressOne.bIsIPv4)
	{
		uchL = 4;
	}
	else
	{
		uchL = 8;
	}
	for(unsigned char uchI = 0; uchI != uchL; uchI++)
	{
		if(oNumericAddressOne.iIPNrs[uchI] != oNumericAddressTwo.iIPNrs[uchI])
		{
			return false;
		}
	}
	if(oNumericAddressOne.iPort != oNumericAddressTwo.iPort)
	{
		return false;
	}
	return true;
}

// Установка текста строки статуса.
void MainWindow::SetStatusBarText(QString strMsg)
{
	p_QLabelStatusBarText->setText(strMsg);
	p_ui->statusBar->repaint();
}

// Обмен выбранного сервера списка серверов с текущим сервером.
void MainWindow::CurrentServerSwap(ServersListWidgetItem* p_ServersListWidgetItem)
{
	NetHub::IPPortPassword oIPPortPassword;
	char* p_chName;
	//
	p_ui->listWidget_Servers->setCurrentRow(-1);
	oIPPortPassword.p_chIPNameBuffer = m_chIPInt;
	oIPPortPassword.p_chPortNameBuffer = m_chPortInt;
	oIPPortPassword.p_chPasswordNameBuffer = m_chPasswordInt;
	p_ui->listWidget_Servers->addItem(new ServersListWidgetItem(
										  &oIPPortPassword, NetHub::CheckIPv4(oIPPortPassword.p_chIPNameBuffer),
										  oPServerName.m_chServerName)); // Добавили элемент с текущими данными.
	// Копия новых строк в текущие данные.
	p_chName = p_ServersListWidgetItem->GetName();
	CopyStrArray(p_ServersListWidgetItem->m_chIP, m_chIPInt, IP_STR_LEN);
	CopyStrArray(p_ServersListWidgetItem->m_chPort, m_chPortInt, PORT_STR_LEN);
	CopyStrArray(p_ServersListWidgetItem->m_chPassword, m_chPasswordInt, AUTH_PASSWORD_STR_LEN);
	if(p_chName != 0)
	{
		CopyStrArray(p_chName, oPServerName.m_chServerName, SERVER_NAME_STR_LEN);
	}
	else
	{
		oPServerName.m_chServerName[0] = 0;
	}
	SetServerLabelData(NetHub::CheckIPv4(m_chIPInt));
	delete p_ServersListWidgetItem;
}

// Удалённое (относительно потока) обновление граф. окна и отправка данных о рабочем фрейме.
void MainWindow::RemoteUpdateSchViewAndSendRFrame()
{
	QRectF oQRectF;
	//
	emit p_This->RemoteUpdateSchView();
	oQRectF = p_SchematicWindow->GetSchematicView()->GetVisibleRect();
	QRealToDbFrame(oQRectF, oPSchReadyFrame.oDbFrame);
	LCHECK_BOOL(p_Client->SendToServerImmediately(
					PROTO_O_SCH_READY, (char*)&oPSchReadyFrame, sizeof(PSchReadyFrame), RESET_POINTER, DONT_TRY_LOCK));
}

// Удаление линков у элемента.
void MainWindow::EraseLinksFromElement(unsigned long long ullIDInt)
{
	bool bToErase = false;
	int iLC = SchematicWindow::vp_Links.count();
	//
	for(int iL = 0; iL < iLC; iL++) // По всем линкам...
	{
		GraphicsLinkItem* p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iL);
		//
		if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc == ullIDInt)
		{
			bToErase = true; // Линк относится к удаляемому и будет тоже удалён.
		}
		else if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst == ullIDInt)
		{
			bToErase = true; // Линк относится к удаляемому и будет тоже удалён.
		}
		if(bToErase) // При устаонве на удаление...
		{
			MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsLinkItem); // Удаление из сцены.
			SchematicWindow::vp_Links.removeAt(iL); // Удаление указателя из списка.
			iLC--;
			iL--;
		}
	}
}

// Блокировка и разблокировка всех объектов сцены.
void MainWindow::BlockSchematic(bool bBlock)
{
	bBlockingGraphics = bBlock;
	for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++)
	{
		GraphicsElementItem* p_GraphicsElementItem;
		//
		p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
		if(bBlock)
		{
			SchematicView::SetElementBlockingPattern(p_GraphicsElementItem, BLOCKING_ON); // Если бл. схемой - блокировка в любом случае.
		}
		else
		{
			SchematicView::SetElementBlockingPattern(p_GraphicsElementItem, // Если разблокируется схемой - блокировка по состоянию bBusy.
													 p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
													 SCH_SETTINGS_EG_BIT_BUSY);
		}
	}
	for(int iF = 0; iF < SchematicWindow::vp_Groups.count(); iF++)
	{
		GraphicsGroupItem* p_GraphicsGroupItem;
		//
		p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
		if(bBlock)
		{
			SchematicView::SetGroupBlockingPattern(p_GraphicsGroupItem, BLOCKING_ON); // Если блокируется схемой - блокировка в любом случае.
		}
		else
		{
			SchematicView::SetGroupBlockingPattern(p_GraphicsGroupItem, // Если разблокируется схемой - блокировка по состоянию bBusy.
												   p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits &
												   SCH_SETTINGS_EG_BIT_BUSY);
		}
	}
	if(bBlock)
	{
		p_SchematicWindow->p_SchematicView->setBackgroundBrush(QBrush(SchBackgroundInactive, Qt::SolidPattern));
	}
	else
	{
		p_SchematicWindow->p_SchematicView->setBackgroundBrush(QBrush(SchBackgroundActive, Qt::SolidPattern));
	}
}

// Процедуры остановки клиента.
void MainWindow::SlotClientStopProcedures()
{
	BlockSchematic(BLOCKING_ON);
	chLastClientRequest = CLIENT_REQUEST_DISCONNECT;
	SetStatusBarText("Остановка клиента...");
	if(!p_Client->Stop())
	{
		goto gTS;
	}
	for(unsigned char uchAtt = 0; uchAtt != CLIENT_WAITING_ATTEMPTS; uchAtt++)
	{
		if(!p_Client->CheckServerAlive()) return;
		MSleep(USER_RESPONSE_MS);
	}
gTS:LOG_P_0(LOG_CAT_E, m_chLogCantStop << "client.");
	RETVAL_SET(RETVAL_ERR);
	emit p_This->RemoteSlotMsgDialog(m_chMsgError, "Программное разъединение невозможно.");
	SetStatusBarText(m_chStatusConnected);
}

// Вызов диалога сообщения.
void MainWindow::SlotMsgDialog(QString strCaption, QString strMsg)
{
	Message_Dialog* p_Message_Dialog;
	//
	p_Message_Dialog = new Message_Dialog(strCaption.toStdString().c_str(), strMsg.toStdString().c_str());
	p_Message_Dialog->exec();
	p_Message_Dialog->deleteLater();
}

// Установка кнопок соединения в позицию готовности.
void MainWindow::SlotSetConnectionButtonsState(bool bConnected)
{
	bBlockConnectionButtons = true;
	if(!p_Client->CheckConnectionSecured()) bConnected = false;
	MSleep(WAITING_FOR_INTERFACE);
	QCoreApplication::processEvents(QEventLoop::AllEvents);
	if(bConnected)
	{
		SetStatusBarText(m_chStatusConnected);
		p_ui->pushButton_Connect->setEnabled(false);
		p_ui->pushButton_Disconnect->setEnabled(true);
		p_ui->pushButton_Disconnect->setFocus();
	}
	else
	{
		SetStatusBarText(m_chStatusReady);
		p_ui->pushButton_Disconnect->setEnabled(false);
		p_ui->pushButton_Connect->setEnabled(true);
		p_ui->pushButton_Connect->setFocus();
	}
	bBlockConnectionButtons = false;
}

// При переключении кнопки 'Schematic'.
void MainWindow::on_action_Schematic_triggered(bool checked)
{
	QRectF oQRectF;
	bool bServerAlive;
	//
	bSchemaIsOpened = checked;
	p_UISettings->setValue("Schema", bSchemaIsOpened);
	bServerAlive = p_Client->CheckServerAlive();
	if(bSchemaIsOpened)
	{
		if(bServerAlive)
		{
			oQRectF = p_SchematicWindow->GetSchematicView()->GetVisibleRect();
			LCHECK_BOOL(p_Client->SendToServerImmediately(
							PROTO_O_SCH_READY, (char*)&oPSchReadyFrame, sizeof(PSchReadyFrame)));
		}
		p_SchematicWindow->show();
	}
	else
	{
		if(bServerAlive)
		{
			oQRectF.setRect(0, 0, 0, 0);
			LCHECK_BOOL(p_Client->SendToServerImmediately(
							PROTO_O_SCH_READY, (char*)&oPSchReadyFrame, sizeof(PSchReadyFrame)));
		}
		p_SchematicWindow->hide();
	}
	QRealToDbFrame(oQRectF, oPSchReadyFrame.oDbFrame);
	LCHECK_BOOL(p_Client->SendToServerImmediately(
					PROTO_O_SCH_READY, (char*)&oPSchReadyFrame, sizeof(PSchReadyFrame)));
}

// При переключении кнопки 'Соединение при включении'.
void MainWindow::on_action_ConnectAtStartup_triggered(bool checked)
{
	p_UISettings->setValue("AutoConnection", checked);
}

// При нажатии кнопки 'Соединить'.
void MainWindow::on_pushButton_Connect_clicked()
{
	if(!bBlockConnectionButtons) ClientStartProcedures();
}

// При нажатии кнопки 'Разъединить'.
void MainWindow::on_pushButton_Disconnect_clicked()
{
	if(!bBlockConnectionButtons) SlotClientStopProcedures();
}

// При нажатии ПКМ на элементе списка серверов.
void MainWindow::on_listWidget_Servers_customContextMenuRequested(const QPoint &pos)
{
	QPoint pntGlobalPos;
	SafeMenu oSafeMenu;
	ServersListWidgetItem* p_ServersListWidgetItem;
	//
	p_ServersListWidgetItem = static_cast<ServersListWidgetItem*>(p_ui->listWidget_Servers->itemAt(pos));
	if(p_ServersListWidgetItem != 0)
	{
		QAction* p_SelectedMenuItem;
		pntGlobalPos = QCursor::pos();
		oSafeMenu.addAction(m_chMenuDeleteItem)->setData(MENU_DELETE_ITEM);
		oSafeMenu.addAction(m_chMenuSetPassword)->setData(MENU_SET_PASSWORD);
		if(!p_Client->CheckServerAlive())
		{
			oSafeMenu.addAction(m_chMenuSetAsDefault)->setData(MENU_SET_AS_DEFAULT);
		}
		p_SelectedMenuItem = oSafeMenu.exec(pntGlobalPos);
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->data() == MENU_DELETE_ITEM)
			{
				delete p_ServersListWidgetItem;
				LCHECK_BOOL(SaveClientConfig());
			}
			else if(p_SelectedMenuItem->data() == MENU_SET_PASSWORD)
			{
				Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)m_chMsgServerPassword,
																			  p_ServersListWidgetItem->m_chPassword, AUTH_PASSWORD_STR_LEN);
				if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
				{
					LCHECK_BOOL(SaveClientConfig());
				}
				p_Set_Proposed_String_Dialog->deleteLater();
			}
			else if(p_SelectedMenuItem->data() == MENU_SET_AS_DEFAULT)
			{
				CurrentServerSwap(p_ServersListWidgetItem);
				LCHECK_BOOL(SaveClientConfig());
			}
		}
	}
}

// При нажатии ПКМ на метке с именем текущего сервера.
void MainWindow::on_label_CurrentServer_customContextMenuRequested(const QPoint &pos)
{
	pos.isNull(); // Заглушка.
	QPoint pntGlobalPos;
	SafeMenu oSafeMenu;
	QAction* p_SelectedMenuItem;
	//
	pntGlobalPos = QCursor::pos();
	oSafeMenu.addAction(m_chMenuSetPassword)->setData(MENU_SET_PASSWORD);
	p_SelectedMenuItem = oSafeMenu.exec(pntGlobalPos);
	if(p_SelectedMenuItem != 0)
	{
		if(p_SelectedMenuItem->data() == MENU_SET_PASSWORD)
		{
			Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog =
					new Set_Proposed_String_Dialog((char*)m_chMsgServerPassword, m_chPasswordInt, AUTH_PASSWORD_STR_LEN);
			//
			if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
			{
				LCHECK_BOOL(SaveClientConfig());
			}
			p_Set_Proposed_String_Dialog->deleteLater();
		}
	}
}

// При нажатии на кнопку 'Добавить' сервер.
void MainWindow::on_pushButton_Add_clicked()
{
	Set_Server_Dialog* p_Set_Server_Dialog;
	Message_Dialog* p_Message_Dialog;
	ServersListWidgetItem* p_ServersListWidgetItem;
	NetHub::IPPortPassword oIPPortPassword;
	NumericAddress oNumericAddress, oNumericAddressHelper;
	QString strHelper;
	//
gA: p_Set_Server_Dialog = new Set_Server_Dialog((char*)"127.0.0.1", (char*)"8877", (char*)"0");
	p_Set_Server_Dialog->deleteLater();
	if(p_Set_Server_Dialog->exec() == DIALOGS_ACCEPT)
	{
		FillNumericStructWithIPPortStrs(oNumericAddress, Set_Server_Dialog::oIPPortPasswordStrings.strIP,
										Set_Server_Dialog::oIPPortPasswordStrings.strPort);
		if(!oNumericAddress.bIsCorrect)
		{
			p_Message_Dialog = new Message_Dialog(m_chMsgError, m_chMsgWrongIPPort);
			p_Message_Dialog->exec();
			p_Message_Dialog->deleteLater();
			goto gA;
		}
		for(int iItem = 0; iItem < p_ui->listWidget_Servers->count(); iItem++)
		{
			p_ServersListWidgetItem = static_cast<ServersListWidgetItem*>(p_ui->listWidget_Servers->item(iItem));
			FillNumericStructWithIPPortStrs(oNumericAddressHelper,
											QString(p_ServersListWidgetItem->m_chIP), QString(p_ServersListWidgetItem->m_chPort));
			if(CheckEqualsNumbers(oNumericAddress, oNumericAddressHelper))
			{
				goto gIPE;
			}
		}
		FillNumericStructWithIPPortStrs(oNumericAddressHelper, QString(m_chIPInt), QString(m_chPortInt));
		if(CheckEqualsNumbers(oNumericAddress, oNumericAddressHelper))
		{
gIPE:       p_Message_Dialog = new Message_Dialog(m_chMsgError, "Комбинация адрес/порт уже в списке.");
			p_Message_Dialog->exec();
			p_Message_Dialog->deleteLater();
			goto gA;
		}
		strHelper.clear();
		if(oNumericAddress.bIsIPv4)
		{
			for(unsigned char uchI = 0; uchI != 4; uchI++)
			{
				strHelper += QString::number(oNumericAddress.iIPNrs[uchI]);
				if(uchI != 3)
				{
					strHelper += ".";
				}
			}
		}
		else
		{
			for(unsigned char uchI = 0; uchI != 8; uchI++)
			{
				strHelper += QString::number(oNumericAddress.iIPNrs[uchI], 16);
				if(uchI != 7)
				{
					strHelper += ":";
				}
			}
		}
		oIPPortPassword.p_chIPNameBuffer = (char*)strHelper.toStdString().c_str();
		strHelper = QString::number(oNumericAddress.iPort);
		oIPPortPassword.p_chPortNameBuffer = (char*)strHelper.toStdString().c_str();
		oIPPortPassword.p_chPasswordNameBuffer = (char*)Set_Server_Dialog::oIPPortPasswordStrings.strPassword.toStdString().c_str();
		p_ui->listWidget_Servers->addItem(new ServersListWidgetItem(&oIPPortPassword, oNumericAddress.bIsIPv4, 0));
		LCHECK_BOOL(SaveClientConfig());
	}
}

// При двойном клике на элементе лист-виджета.
void MainWindow::on_listWidget_Servers_itemDoubleClicked(QListWidgetItem* item)
{
	ServersListWidgetItem* p_ServersListWidgetItem;
	//
	if(p_ui->pushButton_Disconnect->isEnabled()) return;
	p_ServersListWidgetItem = static_cast<ServersListWidgetItem*>(item);
	CurrentServerSwap(p_ServersListWidgetItem);
	LCHECK_BOOL(SaveClientConfig());
}

// Входное обновление параметров элемента по структуре.
void MainWindow::IncomingUpdateElementParameters(GraphicsElementItem* p_GraphicsElementItem, PSchElementVars& a_SchElementVars)
{
	// Фрейм.
	if(a_SchElementVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_ELEMENT_BIT_FRAME)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] frame.");
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame =
				a_SchElementVars.oSchEGGraph.oDbFrame;
		SchematicView::UpdateSelectedInElement(p_GraphicsElementItem,
											  SCH_UPDATE_ELEMENT_FRAME | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN, 0, 0, IS_INCOMING);
	}
	// Отношение к группе.
	if(a_SchElementVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_ELEMENT_BIT_GROUP)
	{
		GraphicsGroupItem* p_GraphicsGroupItem;
		//
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] group.");
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0) // Если элемент в группе...
		{
			if(a_SchElementVars.ullIDGroup == 0) // И пришёл запрос на ОТСОЕДИНЕНИЕ от группы...
			{
				p_GraphicsElementItem->p_GraphicsGroupItemRel->vp_ConnectedElements.removeOne(p_GraphicsElementItem);
				SchematicView::GroupCheckEmptyAndRemoveRecursively(p_GraphicsElementItem->p_GraphicsGroupItemRel);
				p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = 0;
				// Элемент не поднимается автоматически.
			}
		}
		else // Если не в группе, ДОБАВЛЕНИЕ... (Непосредственный перенос пока не реализуем).
		{
			for(int iF = 0; iF != SchematicWindow::vp_Groups.count(); iF++)
			{
				p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
				if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt == a_SchElementVars.ullIDGroup)
				{
					p_GraphicsGroupItem->vp_ConnectedElements.append(p_GraphicsElementItem);
					p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = a_SchElementVars.ullIDGroup;
					SchematicView::UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_MAIN);
					goto gFn;
				}
			}
		}
	}
	// Z-позиция.
gFn:if(a_SchElementVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_ELEMENT_BIT_ZPOS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] z-pos.");
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos =
				a_SchElementVars.oSchEGGraph.dbObjectZPos;
		SchematicView::UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_ELEMENT_ZPOS | SCH_UPDATE_MAIN);
		if(SchematicView::dbObjectZPos <= p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos)
		{
			SchematicView::dbObjectZPos = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos +
					SCH_NEXT_Z_SHIFT;
		}
	}

	// Занятость.
	if(a_SchElementVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_ELEMENT_BIT_BUSY)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] busy.");
		CopyBits(a_SchElementVars.oSchEGGraph.uchSettingsBits,
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits,
				SCH_SETTINGS_EG_BIT_BUSY);
		SchematicView::SetElementBlockingPattern(p_GraphicsElementItem, p_GraphicsElementItem->
												 oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY);
	}
}

// Входное обновление параметров группы по структуре.
void MainWindow::IncomingUpdateGroupParameters(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupVars& a_SchGroupVars)
{
	if(a_SchGroupVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_GROUP_BIT_ELEMENTS_SHIFT)
	{
		double dbXShift = a_SchGroupVars.oSchEGGraph.oDbFrame.dbX -
				p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX;
		double dbYShift = a_SchGroupVars.oSchEGGraph.oDbFrame.dbY -
				p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY;
		//
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] elements shift.");
		for(int iE = 0; iE != p_GraphicsGroupItem->vp_ConnectedElements.count(); iE++)
		{
			GraphicsElementItem* p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iE);
			LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] shifting.");
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX += dbXShift;
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY += dbYShift;
			SchematicView::UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_ELEMENT_FRAME | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
			LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] free by shifting.");
			ResetBits(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits, SCH_SETTINGS_EG_BIT_BUSY);
			SchematicView::SetElementBlockingPattern(p_GraphicsElementItem, BLOCKING_OFF);
		}
	}
	// Фрейм.
	if(a_SchGroupVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_GROUP_BIT_FRAME)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] frame.");
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame =
				a_SchGroupVars.oSchEGGraph.oDbFrame;
		SchematicView::UpdateSelectedInGroup(p_GraphicsGroupItem, SCH_UPDATE_GROUP_FRAME | SCH_UPDATE_MAIN);
	}
	// Занятость.
	if(a_SchGroupVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_GROUP_BIT_BUSY)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] busy.");
		CopyBits(a_SchGroupVars.oSchEGGraph.uchSettingsBits, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits,
				SCH_SETTINGS_EG_BIT_BUSY);
		SchematicView::SetGroupBlockingPattern(p_GraphicsGroupItem,
											   p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits &
											   SCH_SETTINGS_EG_BIT_BUSY);
	}
	// Z-позиция.
	if(a_SchGroupVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_GROUP_BIT_ZPOS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] z-pos.");
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos = a_SchGroupVars.oSchEGGraph.dbObjectZPos;
		SchematicView::UpdateSelectedInGroup(p_GraphicsGroupItem, SCH_UPDATE_GROUP_ZPOS | SCH_UPDATE_MAIN);
		SchematicView::dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	// Группа (return на выход).
	if(a_SchGroupVars.oSchEGGraph.uchChangesBits & SCH_CHANGES_GROUP_BIT_GROUP)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] group.");
		if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup != 0) // Если группа в группе...
		{
			if(a_SchGroupVars.ullIDGroup == 0) // И пришёл запрос на ОТСОЕДИНЕНИЕ от группы...
			{
				p_GraphicsGroupItem->p_GraphicsGroupItemRel->vp_ConnectedGroups.removeOne(p_GraphicsGroupItem);
				SchematicView::GroupCheckEmptyAndRemoveRecursively(p_GraphicsGroupItem->p_GraphicsGroupItemRel);
				p_GraphicsGroupItem->p_GraphicsGroupItemRel = nullptr;
				p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup = 0;
			}
		}
		else // Если не в группе, ДОБАВЛЕНИЕ... (Непосредственный перенос пока не реализуем).
		{
			for(int iF = 0; iF != SchematicWindow::vp_Groups.count(); iF++)
			{
				GraphicsGroupItem* p_GraphicsGroupItemHelper = SchematicWindow::vp_Groups.at(iF);
				if(p_GraphicsGroupItemHelper->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt == a_SchGroupVars.ullIDGroup)
				{
					p_GraphicsGroupItemHelper->vp_ConnectedGroups.append(p_GraphicsGroupItem);
					p_GraphicsGroupItem->p_GraphicsGroupItemRel = p_GraphicsGroupItemHelper;
					p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup = a_SchGroupVars.ullIDGroup;
					SchematicView::UpdateGroupFrameByContentRecursivelyUpstream(p_GraphicsGroupItemHelper);
					return;
				}
			}
		}
	}
}

// Входное обновление параметров линка по структуре.
void MainWindow::IncomingUpdateLinkParameters(GraphicsLinkItem* p_GraphicsLinkItem, PSchLinkVars& a_SchLinkVars)
{
	// Позиция порта источника.
	if(a_SchLinkVars.oSchLGraph.uchChangesBits & SCH_CHANGES_LINK_BIT_SCR_PORT_POS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName).toStdString()
				<< "<>" << QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName).toStdString()
				<< "] scr port pos.");
		p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos =
				a_SchLinkVars.oSchLGraph.oDbSrcPortGraphPos;
		SchematicView::UpdateSelectedInElement(p_GraphicsLinkItem->p_GraphicsElementItemSrc,
												 SCH_UPDATE_PORT_SRC_POS | SCH_UPDATE_LINK_POS | SCH_UPDATE_MAIN,
												 p_GraphicsLinkItem->p_GraphicsPortItemSrc, p_GraphicsLinkItem);
	}
	// Позиция порта приёмника.
	if(a_SchLinkVars.oSchLGraph.uchChangesBits & SCH_CHANGES_LINK_BIT_DST_PORT_POS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName).toStdString()
				<< "<>" << QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName).toStdString()
				<< "] dst port pos.");
		p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos =
				a_SchLinkVars.oSchLGraph.oDbDstPortGraphPos;
		SchematicView::UpdateSelectedInElement(p_GraphicsLinkItem->p_GraphicsElementItemDst,
												 SCH_UPDATE_PORT_DST_POS | SCH_UPDATE_LINK_POS | SCH_UPDATE_MAIN,
												 p_GraphicsLinkItem->p_GraphicsPortItemDst, p_GraphicsLinkItem);
	}
}

//== Класс потоко-независимого доступа к интерфейсу.
// Конструктор.
WidgetsThrAccess::WidgetsThrAccess(Ui::MainWindow *p_ui)
{
	p_uiInt = p_ui;
}

// Добавление графического объекта элемента.
void WidgetsThrAccess::AddGraphicsElementItem()
{
	p_ConnGraphicsElementItem = new GraphicsElementItem(p_PSchElementBase);
	MainWindow::p_SchematicWindow->oScene.addItem(p_ConnGraphicsElementItem);
}

// Удаление графического объекта элемента.
void WidgetsThrAccess::RemoveGraphicsElementItem()
{
	MainWindow::p_SchematicWindow->oScene.removeItem(p_ConnGraphicsElementItem);
}

// Удаление графического объекта группы.
void WidgetsThrAccess::RemoveGraphicsGroupItem()
{
	MainWindow::p_SchematicWindow->oScene.removeItem(p_ConnGraphicsGroupItem);
}

// Добавление графического объекта линка.
void WidgetsThrAccess::AddGraphicsLinkItem()
{
	p_ConnGraphicsLinkItem = new GraphicsLinkItem(p_PSchLinkBase);
	if(p_PSchLinkBase->oPSchLinkVars.oSchLGraph.uchChangesBits != SCH_CHANGES_LINK_BIT_INIT_ERROR)
	{
		MainWindow::p_SchematicWindow->oScene.addItem(p_ConnGraphicsLinkItem);
	}
	else
	{
		delete p_ConnGraphicsLinkItem;
		p_ConnGraphicsLinkItem = nullptr;
	}
}

// Установка размеров групбокса элемента по указателю p_ConnGraphicsElementItem и размеру из oDbPointFrameSize.
void WidgetsThrAccess::ElementGroupBoxSizeSet()
{
	p_ConnGraphicsElementItem->p_QGroupBox->
			setFixedSize(p_ConnGraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW - 6,
						 p_ConnGraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH - 6);
}

// Установка ширины строки названия группы по указателю p_ConnGraphicsGroupItem.
void WidgetsThrAccess::GroupLabelWidthSet()
{
	p_ConnGraphicsGroupItem->p_QLabel->setFixedWidth(
				p_ConnGraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW - 6);
}

// Установка видиости названия группы по p_ConnGraphicsGroupItem.
void WidgetsThrAccess::GroupLabelsVisibilitySet()
{
	for(int iF = 0; iF != vp_ConnGraphicsGroupItems.count(); iF++)
	{
		GraphicsGroupItem* p_GraphicsGroupItem = vp_ConnGraphicsGroupItems.at(iF);
		//
		p_GraphicsGroupItem->p_QLabel->setVisible(!(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits &
													SCH_SETTINGS_EG_BIT_MIN));
	}
}

// Установка видиости групбокса элемента по p_ConnGraphicsElementItem.
void WidgetsThrAccess::ElementGroupboxesVisibilitySet()
{
	for(int iF = 0; iF != vp_ConnGraphicsElementItems.count(); iF++)
	{
		GraphicsElementItem* p_GraphicsElementItem = vp_ConnGraphicsElementItems.at(iF);
		//
		p_GraphicsElementItem->p_QGroupBox->setVisible(!(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
														 SCH_SETTINGS_EG_BIT_MIN));
	}
}

// Добавление графического объекта группы.
void WidgetsThrAccess::AddGraphicsGroupItem()
{
	p_ConnGraphicsGroupItem = new GraphicsGroupItem(p_PSchGroupBase);
	MainWindow::p_SchematicWindow->oScene.addItem(p_ConnGraphicsGroupItem);
}

//== Класс модифицированного меню.
// Переопределение функции обработки нажатия мыши.
void SafeMenu::mousePressEvent(QMouseEvent* p_Event)
{
	if((p_Event->button() == Qt::MouseButton::RightButton) &
	   ((p_Event->pos().x() > pos().x()) & (p_Event->pos().y() > pos().y())))
	{
	    return;
	}
	QMenu::mousePressEvent(p_Event);
}

// Переопределение функции обработки перемещения мыши.
void SafeMenu::mouseMoveEvent(QMouseEvent* p_Event)
{
	if(p_Event->button() == Qt::MouseButton::RightButton) return;
	QMenu::mouseMoveEvent(p_Event);
}

// Переопределение функции обработки отпускания мыши.
void SafeMenu::mouseReleaseEvent(QMouseEvent* p_Event)
{
	if(p_Event->button() == Qt::MouseButton::RightButton) return;
	QMenu::mouseReleaseEvent(p_Event);
}
