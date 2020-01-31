//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "ui_main-window.h"
#include "../Z-Hub/Dialogs/message-dialog.h"

//== МАКРОСЫ.
#define LOG_NAME					"main-window"
#define MSG_WRONG_DATA				"Wrong data in pocket."
#define C_CONF_PATH					"../Z-Editor/settings/client.xml"
#define CLIENT_WAITING_ATTEMPTS		128
//
#define MessageDialog(caption, message)																\
	p_WidgetsThrAccess->oStrMsgDialogPair.strCaption = caption;                                     \
	p_WidgetsThrAccess->oStrMsgDialogPair.strMessage = message;                                     \
	ThrUiAccessET(p_WidgetsThrAccess, DoDialog);

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS(MainWindow)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(MainWindow)
int MainWindow::iInitRes;
Ui::MainWindow* MainWindow::p_ui = new Ui::MainWindow;
QSettings* MainWindow::p_UISettings = nullptr;
const char* MainWindow::cp_chUISettingsName = E_MAINWINDOW_UI_CONF_PATH;
QLabel* MainWindow::p_QLabelStatusBarText;
SchematicWindow* MainWindow::p_SchematicWindow = nullptr;
bool MainWindow::bSchemaIsOpened = false;
WidgetsThrAccess* MainWindow::p_WidgetsThrAccess;
Ui::MainWindow* WidgetsThrAccess::p_uiInt;
WidgetsThrAccess::StrMsgDialogPair WidgetsThrAccess::oStrMsgDialogPair;
Client* MainWindow::p_Client = nullptr;
PServerName MainWindow::oPServerName;
char MainWindow::m_chIPInt[IP_STR_LEN];
char MainWindow::m_chPortInt[PORT_STR_LEN];
char MainWindow::m_chPasswordInt[AUTH_PASSWORD_STR_LEN];
NetHub::IPPortPassword MainWindow::oIPPortPassword;

//== ФУНКЦИИ КЛАССОВ.
//== Класс главного окна.
// Конструктор.
MainWindow::MainWindow(QWidget* p_parent) :
	QMainWindow(p_parent)
{
	// Для избежания ошибки при доступе из другого потока.
	qRegisterMetaType<QVector<int>>("QVector<int>");
	//
	LOG_CTRL_INIT;
	LOG_P_0(LOG_CAT_I, "START.");
	iInitRes = RETVAL_OK;
	p_UISettings = new QSettings(cp_chUISettingsName, QSettings::IniFormat);
	p_ui->setupUi(this);
	p_QLabelStatusBarText = new QLabel(this);
	p_ui->statusBar->addWidget(p_QLabelStatusBarText);
	if(IsFileExists((char*)cp_chUISettingsName))
	{
		LOG_P_2(LOG_CAT_I, "Restore UI states.");
		// Splitters.

		// MainWidow.
		if(!restoreGeometry(p_UISettings->value("Geometry").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, "Can`t restore Geometry UI state.");
			RETVAL_SET(RETVAL_ERR);
		}
		if(!restoreState(p_UISettings->value("WindowState").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, "Can`t restore WindowState UI state.");
			RETVAL_SET(RETVAL_ERR);
		}
		bSchemaIsOpened = p_UISettings->value("Schema").toBool();
	}
	else
	{
		LOG_P_0(LOG_CAT_W, "mainwidow_ui.ini is missing and will be created by default at the exit from program.");
	}
	if(!LoadClientConfig()) goto gEI;
	p_Client = new Client(LOG_MUTEX);
	p_Client->SetServerCommandArrivedCB(ServerCommandArrivedCallback);
	p_Client->SetServerDataArrivedCB(ServerDataArrivedCallback);
	p_Client->SetServerStatusChangedCB(ServerStatusChangedCallback);
	if(!ClientStartProcedures())
	{
gEI:	iInitRes = RETVAL_ERR;
		RETVAL_SET(RETVAL_ERR);
		return;
	}
	//
	p_ui->actionSchematic->setChecked(bSchemaIsOpened);
	p_QLabelStatusBarText->setText(cstrStatusReady);
}

// Деструктор.
MainWindow::~MainWindow()
{
	if(RETVAL == RETVAL_OK)
	{
		LOG_P_0(LOG_CAT_I, "EXIT.")
	}
	else
	{
		LOG_P_0(LOG_CAT_E, "EXIT WITH ERROR: " << RETVAL);
	}
	LOG_CLOSE;
	delete p_ui;
}

// Процедуры при закрытии окна приложения.
void MainWindow::closeEvent(QCloseEvent *event)
{
	p_QLabelStatusBarText->setText(cstrStatusShutdown);
	p_ui->statusBar->repaint();
	if(p_Client)
	{
		LCHECK_BOOL(ClientStopProcedures());
		delete p_Client;
	}
	// Main.
	p_UISettings->setValue("Geometry", saveGeometry());
	p_UISettings->setValue("WindowState", saveState());
	p_UISettings->setValue("Schema", bSchemaIsOpened);
	// Splitters.

	// Other.
	if(p_SchematicWindow != 0)
	{
		if(p_SchematicWindow->isVisible())
			p_SchematicWindow->RefClose();
	}
	QMainWindow::closeEvent(event);
}

// Установка соединения сигнала на обновление граф. окна с граф. окном.
void MainWindow::SetSchViewSignalConnection()
{
	connect(this, SIGNAL(RemoteUpdateSchView()), p_SchematicWindow, SLOT(UpdateScene()));
}

// При переключении кнопки 'Schematic'.
void MainWindow::on_actionSchematic_triggered(bool checked)
{
	bSchemaIsOpened = checked;
	p_UISettings->setValue("Schema", bSchemaIsOpened);
	if(bSchemaIsOpened)
	{
		p_SchematicWindow->show();
	}
	else
	{
		p_SchematicWindow->hide();
	}
}

// Для внешнего отключения чекбокса кнопки 'Схема'.
void MainWindow::UncheckSchemaCheckbox()
{
	p_ui->actionSchematic->setChecked(false);
	bSchemaIsOpened = false;
	p_UISettings->setValue("Schema", bSchemaIsOpened);
}

// Кэлбэк обработки отслеживания статута сервера.
void MainWindow::ServerStatusChangedCallback(bool bStatus)
{

}

// Кэлбэк обработки прихода команд от сервера.
void MainWindow::ServerCommandArrivedCallback(unsigned short ushCommand)
{
	LOG_P_2(LOG_CAT_I, "Server command: " << ushCommand);
//	switch(chLastClientRequest)
//	{
//		case CLIENT_REQUEST_CONNECT:
//		{
//			switch(ushCommand)
//			{
//				case PROTO_S_PASSW_OK:
//				{
//					break;
//				}
//				case PROTO_S_PASSW_ERR:
//				{
//					LOG_P_0(LOG_CAT_W, "Server entry password is wrong.");
//					MessageDialog(tr("Ошибка"), tr("Неверный пароль к серверу"));
//					break;
//				}
//			}
//			break;
//		}
//		case CLIENT_REQUEST_DISCONNECT:
//		{
//			switch(ushCommand)
//			{
//				case PROTO_S_ACCEPT_LEAVING:
//				{
//					break;
//				}
//			}
//			break;
//		}
//		case CLIENT_REQUEST_UNDEFINED:
//		{
//			switch(ushCommand)
//			{
//				case PROTO_S_SHUTDOWN_INFO:
//				{
//					MessageDialog(tr("Информация"), tr("Сервер отключил клиенты"));
//					break;
//				}
//				case PROTO_S_KICK:
//				{
//					MessageDialog(tr("Внимание"), tr("Клиент отключен сервером"));
//					break;
//				}
//			}
//			break;
//		}
//	}
//	chLastClientRequest = CLIENT_REQUEST_UNDEFINED;
}

// Кэлбэк обработки прихода пакетов от сервера.
void MainWindow::ServerDataArrivedCallback(unsigned short ushType, void* p_ReceivedData, int iPocket)
{
	bool bProcessed = false;
	//
	switch(ushType)
	{
		//========  Раздел PROTO_S_SERVER_NAME. ========
		case PROTO_S_SERVER_NAME:
		{
			if(p_ReceivedData != 0)
			{
				if(QString(oPServerName.m_chServerName) != QString((char*)p_ReceivedData))
				{
					memcpy(oPServerName.m_chServerName, p_ReceivedData, sizeof(PServerName)); // Загрузка в соотв. структуру.
					//LCHECK_BOOL(SaveClientConfig());
				}
				LOG_P_2(LOG_CAT_I, "Server name: " << oPServerName.m_chServerName);
			}
			else
			{
				LOG_P_0(LOG_CAT_W, MSG_WRONG_DATA);
			}
			bProcessed = true;
			break;
		}
		// Следующий раздел...
	}
	//
	if(!bProcessed)
	{
		LOG_P_0(LOG_CAT_W, "Unknown data type from the server.");
	}
	LCHECK_INT(p_Client->ReleaseDataInPositionC(iPocket, false));
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
		LOG_P_0(LOG_CAT_E, "Can`t open configuration file:" << C_CONF_PATH);
		return false;
	}
	else
	{
		LOG_P_2(LOG_CAT_I, "Configuration is loaded.");
	}
	LOG_P_2(LOG_CAT_I, "--- Current server info ---");
	if(!FindChildNodes(xmlDocCConf.LastChild(), l_pSelectedServer,
					   "Selected_server", FCN_ONE_LEVEL, FCN_FIRST_ONLY))
	{
		LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No 'Selected_server' node.");
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
		LOG_P_2(LOG_CAT_I, "Name: " << oPServerName.m_chServerName);
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
		if(NetHub::CheckIPv4(p_chHelper))
		{
			LOG_P_2(LOG_CAT_I, "IP: " << m_chIPInt);
		}
		else
		{
			LOG_P_2(LOG_CAT_I, "IP: [" << m_chIPInt << "]");
		}
	}
	else
	{
		LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No '(Selected_server)IP' node.");
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
		LOG_P_2(LOG_CAT_I, "Port: " << m_chPortInt);
	}
	else
	{
		LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No '(Selected_server)Port' node.");
		return false;
	}
	FIND_IN_CHILDLIST(l_pSelectedServer.front(), p_ListPassword, "Password",
					  FCN_ONE_LEVEL, p_NodePassword)
	{
		p_chHelper = (char*)p_NodePassword->FirstChild()->Value();
	} FIND_IN_CHILDLIST_END(p_ListPassword);
	if(p_chHelper == 0)
	{
		LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No '(Server)Password' node.");
		return false;
	}
	else
	{
		CopyStrArray(p_chHelper, m_chPasswordInt, AUTH_PASSWORD_STR_LEN);
	}
	FillNumericStructWithIPPortStrs(oNumericAddress, QString(m_chIPInt), QString(m_chPortInt));
	if(!oNumericAddress.bIsCorrect)
	{
		LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! Incorrect main adress format.");
		return false;
	}
	if(!FindChildNodes(xmlDocCConf.LastChild(), l_pServers,
					   "Servers", FCN_ONE_LEVEL, FCN_FIRST_ONLY))
	{
		LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No 'Servers' node.");
		return false;
	}
	LOG_P_2(LOG_CAT_I, "--- Stored servers info ---");
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
			LOG_P_2(LOG_CAT_I, "Name: " << p_chHelper);
		}
		oIPPortPasswordHelper.p_chIPNameBuffer = 0;
		FIND_IN_CHILDLIST(p_NodeServer, p_ListServerIP, "IP",
						  FCN_ONE_LEVEL, p_NodeServerIP)
		{
			oIPPortPasswordHelper.p_chIPNameBuffer = (char*)p_NodeServerIP->FirstChild()->Value();
		} FIND_IN_CHILDLIST_END(p_ListServerIP);
		if(oIPPortPasswordHelper.p_chIPNameBuffer != 0)
		{
			if(NetHub::CheckIPv4(oIPPortPasswordHelper.p_chIPNameBuffer))
			{
				LOG_P_2(LOG_CAT_I, "Server IP: " << oIPPortPasswordHelper.p_chIPNameBuffer);
			}
			else
			{
				LOG_P_2(LOG_CAT_I, "Server IP: [" << oIPPortPasswordHelper.p_chIPNameBuffer << "]");
			}
		}
		else
		{
			LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No '(Server)IP' node in servers list.");
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
			LOG_P_2(LOG_CAT_I, "Port: " << oIPPortPasswordHelper.p_chPortNameBuffer);
		}
		else
		{
			LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No '(Server)Port' node in servers list.");
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
			LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! No '(Server)Password' node in servers list.");
			return false;
		}
		LOG_P_2(LOG_CAT_I, "--- ---");
		if(!oNumericAddress.bIsCorrect)
		{
			LOG_P_0(LOG_CAT_E, "Configuration file is corrupt! Incorrect adress format.");
			return false;
		}
	} PARSE_CHILDLIST_END(p_ListServers);
	oIPPortPassword.p_chIPNameBuffer = m_chIPInt;
	oIPPortPassword.p_chPortNameBuffer = m_chPortInt;
	oIPPortPassword.p_chPasswordNameBuffer = m_chPasswordInt;
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
//	XMLNode* p_NodeServer;
//	char* p_chHelper;
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
//	for(int iC=0; iC < p_ui->Servers_listWidget->count(); iC++)
//	{
//		p_NodeServer = p_NodeServers->InsertEndChild(xmlDocCConf.NewElement("Server"));
//		p_chHelper = ((ServersListWidgetItem*)p_ui->Servers_listWidget->item(iC))->GetName();
//		if(p_chHelper != 0)
//		{
//			p_NodeName = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Name"));
//			p_NodeName->ToElement()->SetText(p_chHelper);
//		}
//		p_NodeServerIP = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("IP"));
//		p_NodeServerIP->ToElement()->SetText(
//					((ServersListWidgetItem*)p_ui->Servers_listWidget->item(iC))->GetIPPortPassword().p_chIPNameBuffer);
//		p_NodePort = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Port"));
//		p_NodePort->ToElement()->SetText(
//					((ServersListWidgetItem*)p_ui->Servers_listWidget->item(iC))->GetIPPortPassword().p_chPortNameBuffer);
//		p_NodePassword = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Password"));
//		p_NodePassword->ToElement()->SetText(
//					((ServersListWidgetItem*)p_ui->Servers_listWidget->item(iC))->GetIPPortPassword().p_chPasswordNameBuffer);
//	}
	eResult = xmlDocCConf.SaveFile(C_CONF_PATH);
	if (eResult != XML_SUCCESS)
	{
		LOG_P_0(LOG_CAT_E, "Can`t save client configuration data.");
		return false;
	}
	else return true;
}

// Процедуры запуска клиента.
bool MainWindow::ClientStartProcedures()
{
	if(!p_Client->Start(&oIPPortPassword))
	{
		goto gCA;
	}
	for(unsigned char uchAtt = 0; uchAtt != 64; uchAtt++)
	{
		if(p_Client->CheckServerAlive())
		{
			p_QLabelStatusBarText->setText(cstrStatusReady);
			p_ui->statusBar->repaint();
			return true;
		}
		MSleep(USER_RESPONSE_MS);
	}
gCA:LOG_P_0(LOG_CAT_W, "Can`t start client.");
	MessageDialog("Warning", "Failed to connect");
	return false;
}

// Процедуры остановки клиента.
bool MainWindow::ClientStopProcedures()
{
	p_QLabelStatusBarText->setText(cstrStatusStopClient);
	p_ui->statusBar->repaint();
	if(!p_Client->Stop())
	{
		goto gTS;
	}
	for(unsigned char uchAtt = 0; uchAtt != CLIENT_WAITING_ATTEMPTS; uchAtt++)
	{
		if(!p_Client->CheckServerAlive())
		{
			p_QLabelStatusBarText->setText(cstrStatusReady);
			p_ui->statusBar->repaint();
			return true;
		}
		MSleep(USER_RESPONSE_MS);
	}
gTS:LOG_P_0(LOG_CAT_E, "Can`t stop client.");
	MessageDialog("Error", "Failed to disconnect");
	p_QLabelStatusBarText->setText(cstrStatusReady);
	p_ui->statusBar->repaint();
	return false;
}

//== Класс потоко-независимого доступа к интерфейсу.
// Конструктор.
WidgetsThrAccess::WidgetsThrAccess(Ui::MainWindow *p_ui)
{
	p_uiInt = p_ui;
}

// Создание, вызов и удаление диалога.
void WidgetsThrAccess::DoDialog()
{
	Message_Dialog* p_Message_Dialog;
	//
	p_Message_Dialog = new
			Message_Dialog(oStrMsgDialogPair.strCaption.toStdString().c_str(), oStrMsgDialogPair.strMessage.toStdString().c_str());
	p_Message_Dialog->exec();
	p_Message_Dialog->deleteLater();
}

// Очистка сцены.
void WidgetsThrAccess::ClearScene()
{
	MainWindow::p_SchematicWindow->oScene.clear();
}
