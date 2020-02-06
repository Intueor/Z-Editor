//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "ui_main-window.h"
#include "../Z-Hub/Dialogs/message-dialog.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"

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

//== ФУНКЦИИ КЛАССОВ.
//== Класс добавки данных сервера к стандартному элементу лист-виджета.
// Конструктор.
ServersListWidgetItem::ServersListWidgetItem(NetHub::IPPortPassword* p_IPPortPassword,
											 bool bIsIPv4, char *p_chName, QListWidget* p_ListWidget) : QListWidgetItem(p_ListWidget)
{
	if(p_chName)
	{
		CopyStrArray(p_chName, m_chName, SERVER_NAME_STR_LEN);
	}
	else
	{
		m_chName[0] = 0;
	}
	CopyStrArray(p_IPPortPassword->p_chIPNameBuffer, m_chIP, IP_STR_LEN);
	CopyStrArray(p_IPPortPassword->p_chPortNameBuffer, m_chPort, PORT_STR_LEN);
	CopyStrArray(p_IPPortPassword->p_chPasswordNameBuffer, m_chPassword, AUTH_PASSWORD_STR_LEN);
	if(bIsIPv4)
	{
		if(m_chName[0] == 0)
		{
			this->setText(QString(m_chIP) + ":" + QString(m_chPort));
		}
		else
		{
			this->setText(QString(m_chName));
			this->setToolTip(QString(m_chName) + " - " + QString(m_chIP) + ":" + QString(m_chPort));
		}
	}
	else
	{
		if(m_chName[0] == 0)
		{
			this->setText("[" + QString(m_chIP) + "]:" + QString(m_chPort));
		}
		else
		{
			this->setText(QString(m_chName));
			this->setToolTip(QString(m_chName) + " - [" + QString(m_chIP) + "]:" + QString(m_chPort));
		}
	}
}

// Получение указателя строку с именем сервера или 0 при пустой строке.
char* ServersListWidgetItem::GetName()
{
	if(m_chName[0] != 0)
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
	p_QLabelStatusBarText = new QLabel(this);
	p_QLabelStatusBarText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	p_ui->statusBar->addWidget(p_QLabelStatusBarText);
	if(IsFileExists((char*)cp_chUISettingsName))
	{
		LOG_P_2(LOG_CAT_I, m_chLogRestoreUI);
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
	p_Client = new Client(LOG_MUTEX);
	p_Client->SetServerCommandArrivedCB(ServerCommandArrivedCallback);
	p_Client->SetServerDataArrivedCB(ServerDataArrivedCallback);
	p_Client->SetServerStatusChangedCB(ServerStatusChangedCallback);
	//
	p_ui->action_Schematic->setChecked(p_UISettings->value("Schema").toBool());
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
	//
#endif
}

// Деструктор.
MainWindow::~MainWindow()
{
	if(RETVAL == RETVAL_OK)
	{
		LOG_P_0(LOG_CAT_I, m_chLogExit)
	}
	else
	{
		LOG_P_0(LOG_CAT_E, m_chLogErrorExit << RETVAL);
	}
	LOG_CLOSE;
	delete p_ui;
}

// Процедуры при закрытии окна приложения.
void MainWindow::closeEvent(QCloseEvent *event)
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
	QMainWindow::closeEvent(event);
}

// Установка сигналов сообщения с граф. окном.
void MainWindow::SetSchWindowSignalConnections()
{
	connect(this, SIGNAL(RemoteUpdateSchView()), p_SchematicWindow, SLOT(UpdateScene()));
	connect(this, SIGNAL(RemoteClearScene()), p_SchematicWindow, SLOT(ClearScene()));
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
	LOG_P_2(LOG_CAT_I, "Server command: " << ushCommand);
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
		//========  Раздел PROTO_S_SERVER_NAME. ========
		case PROTO_S_SERVER_NAME:
		{
			if(p_ReceivedData != 0)
			{
				if(QString(oPServerName.m_chServerName) != QString((char*)p_ReceivedData))
				{
					memcpy(oPServerName.m_chServerName, p_ReceivedData, sizeof(PServerName)); // Загрузка в соотв. структуру.
					LCHECK_BOOL(SaveClientConfig());
				}
				LOG_P_2(LOG_CAT_I, "Server name: " << oPServerName.m_chServerName);
				p_ui->label_CurrentServer->setText(QString(oPServerName.m_chServerName));
			}
			else
			{
				LOG_P_0(LOG_CAT_W, "Wrong data in pocket.");
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
		LOG_P_0(LOG_CAT_E, m_chLogCantOpenConfig << C_CONF_PATH);
		return false;
	}
	else
	{
		LOG_P_2(LOG_CAT_I, "Configuration" << m_chLogIsLoaded);
	}
	LOG_P_2(LOG_CAT_I, "--- Current server info ---");
	if(!FindChildNodes(xmlDocCConf.LastChild(), l_pSelectedServer,
					   "Selected_server", FCN_ONE_LEVEL, FCN_FIRST_ONLY))
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No 'Selected_server' node.");
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
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No '(Selected_server)IP' node.");
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
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No '(Selected_server)Port' node.");
		return false;
	}
	FIND_IN_CHILDLIST(l_pSelectedServer.front(), p_ListPassword, "Password",
					  FCN_ONE_LEVEL, p_NodePassword)
	{
		p_chHelper = (char*)p_NodePassword->FirstChild()->Value();
	} FIND_IN_CHILDLIST_END(p_ListPassword);
	if(p_chHelper == 0)
	{
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No '(Server)Password' node.");
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
		LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No 'Servers' node.");
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
			LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No '(Server)IP'" << m_chLogNodeInList);
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
			LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No '(Server)Port'" << m_chLogNodeInList);
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
			LOG_P_0(LOG_CAT_E, m_chLogCorruptConf << "No '(Server)Password'" << m_chLogNodeInList);
			return false;
		}
		LOG_P_2(LOG_CAT_I, "--- ---");
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
	char* p_chHelper;
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
		p_NodeServer = p_NodeServers->InsertEndChild(xmlDocCConf.NewElement("Server"));
		p_chHelper = ((ServersListWidgetItem*)p_ui->listWidget_Servers->item(iC))->GetName();
		if(p_chHelper != 0)
		{
			p_NodeName = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Name"));
			p_NodeName->ToElement()->SetText(p_chHelper);
		}
		p_NodeServerIP = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("IP"));
		p_NodeServerIP->ToElement()->SetText(
					((ServersListWidgetItem*)p_ui->listWidget_Servers->item(iC))->m_chIP);
		p_NodePort = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Port"));
		p_NodePort->ToElement()->SetText(
					((ServersListWidgetItem*)p_ui->listWidget_Servers->item(iC))->m_chPort);
		p_NodePassword = p_NodeServer->InsertEndChild(xmlDocCConf.NewElement("Password"));
		p_NodePassword->ToElement()->SetText(
					((ServersListWidgetItem*)p_ui->listWidget_Servers->item(iC))->m_chPassword);
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
		if(p_Client->CheckServerAlive()) return;
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

// Процедуры остановки клиента.
void MainWindow::SlotClientStopProcedures()
{
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
	p_UISettings->setValue("Schema", checked);
	if(checked)
	{
		p_SchematicWindow->show();
	}
	else
	{
		p_SchematicWindow->hide();
	}
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
	QMenu oMenu;
	QAction* p_SelectedMenuItem;
	ServersListWidgetItem* p_ServersListWidgetItem;
	Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog;
	//
	p_ServersListWidgetItem = (ServersListWidgetItem*)p_ui->listWidget_Servers->itemAt(pos);
	if(p_ServersListWidgetItem != 0)
	{
		pntGlobalPos = QCursor::pos();
		oMenu.addAction(m_chMenuDelete);
		oMenu.addAction(m_chMenuSetPassword);
		if(!p_Client->CheckServerAlive())
		{
			oMenu.addAction(m_chMenuSetAsDefault);
		}
		p_SelectedMenuItem = oMenu.exec(pntGlobalPos);
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->text() == m_chMenuDelete)
			{
				delete p_ServersListWidgetItem;
				LCHECK_BOOL(SaveClientConfig());
			}
			else if(p_SelectedMenuItem->text() == m_chMenuSetPassword)
			{
				p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)m_chMsgServerPassword,
																			  p_ServersListWidgetItem->m_chPassword, AUTH_PASSWORD_STR_LEN);
				if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
				{
					LCHECK_BOOL(SaveClientConfig());
				}
				p_Set_Proposed_String_Dialog->deleteLater();
			}
			else if(p_SelectedMenuItem->text() == m_chMenuSetAsDefault)
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
	QMenu oMenu;
	QAction* p_SelectedMenuItem;
	Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog;
	//
	pntGlobalPos = QCursor::pos();
	oMenu.addAction(m_chMenuSetPassword);
	p_SelectedMenuItem = oMenu.exec(pntGlobalPos);
	if(p_SelectedMenuItem != 0)
	{
		if(p_SelectedMenuItem->text() == m_chMenuSetPassword)
		{
			p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)m_chMsgServerPassword, m_chPasswordInt, AUTH_PASSWORD_STR_LEN);
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
			p_ServersListWidgetItem = (ServersListWidgetItem*)(p_ui->listWidget_Servers->item(iItem));
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
	if(p_Client->CheckServerAlive()) return;
	p_ServersListWidgetItem = (ServersListWidgetItem*)item;
	CurrentServerSwap(p_ServersListWidgetItem);
	LCHECK_BOOL(SaveClientConfig());
}
