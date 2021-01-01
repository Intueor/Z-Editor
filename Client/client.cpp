//== ВКЛЮЧЕНИЯ.
#include <Client/client.h>
#include <string.h>

//== МАКРОСЫ.
#define LOG_NAME				"client"
#define LOG_DIR_PATH			"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS(Client)
LOGDECL_INIT_PTHRD_INCLASS_EXT_ADD(Client)
bool Client::bClientAlive = false;
pthread_mutex_t Client::ptLogMutex;
pthread_mutex_t Client::ptConnMutex = PTHREAD_MUTEX_INITIALIZER;
bool Client::bExitSignal;
bool Client::bExitAccepted;
bool Client::bServerAlive;
bool Client::bConnectionThrAlive;
Client::ConversationData Client::oConvData;
ProtoParser Client::oProtoParser;
pthread_t Client::RecThr;
pthread_t Client::ConnectThr;
pthread_t Client::ClientThr;
char* Client::p_chSettingsPathInt = nullptr;
CBServerCommandArrived Client::pf_CBServerCommandArrivedInt = nullptr;
CBServerDataArrived Client::pf_CBServerDataArrivedInt = nullptr;
CBServerStatusChanged Client::pf_CBServerStatusChangedInt = nullptr;
NetHub Client::oInternalNetHub;
NetHub::IPPortPassword* Client::p_IPPortPasswordInt = nullptr;

//== ФУНКЦИИ КЛАССОВ.
//== Класс клиента.
// Конструктор.
Client::Client(pthread_mutex_t ptLogMutex)
{
	LOG_CTRL_BIND_EXT_MUTEX(ptLogMutex);
	LOG_CTRL_INIT;
}

// Деструктор.
Client::~Client()
{
	LOG_CLOSE;
}

// Запрос запуска клиента.
bool Client::Start(NetHub::IPPortPassword* p_IPPortPassword)
{
	if(!bClientAlive)
	{
		p_IPPortPasswordInt = p_IPPortPassword;
		bExitSignal = false;
		oConvData.bFullOnServer = false;
		bExitAccepted = false;
		bServerAlive = false;
		LOG_P_1(LOG_CAT_I, "Starting client thread.");
		pthread_create(&ClientThr, nullptr, ClientThread, nullptr);
		return true;
	}
	else
	{
		LOG_P_0(LOG_CAT_E, "Starting client failed - already started.");
	}
	return false;
}

// Запрос остановки клиента.
bool Client::Stop()
{
	bool bRetval = false;
	//
	if(oConvData.bFullOnClient == true)
	{
		oConvData.bFullOnClient = false;
		if(bServerAlive)
		{
			LCHECK_BOOL(SendToConnectionImmediately(oConvData.oConnectionData, PROTO_A_BUFFER_READY));
		}
	}
	if(bExitAccepted)
	{
		goto gES;
	}
	if(bServerAlive == false)
	{
		LOG_P_0(LOG_CAT_W, "No communication with server, force disconnect.");
		goto gNC;
	}
	LOG_P_1(LOG_CAT_I, "Sending leaving-request to server.");
	bRetval = true;
	SendToConnectionImmediately(oConvData.oConnectionData, PROTO_C_REQUEST_LEAVING);
	for(char chAttempt = 0; chAttempt != 4; chAttempt++)
	{
		MSleep(WAITING_FOR_SRV_STEP / 2);
		if(bExitSignal == true) goto gES;
		LOG_P_1(LOG_CAT_I, "Waiting for answer, attempt nr." << (chAttempt + 1));
	}
	LOG_P_0(LOG_CAT_W, "Server not response, force disconnect.");
gNC:bExitSignal = true;
	MSleep(WAITING_FOR_SRV_STEP);
gES:bExitSignal = false;
	return bRetval;
}

// Запрос готовности.
bool Client::CheckServerAlive()
{
	return bClientAlive;
}

// Запрос статуса авторизации.
bool Client::CheckConnectionSecured()
{
	return oConvData.bSecured;
}

// Отправка пакета по соединению немедленно.
bool Client::SendToConnectionImmediately(NetHub::ConnectionData &a_ConnectionData, unsigned short ushCommand, bool bFullFlag,
						  char *p_chBuffer, int iLength, bool bResetPointer)
{
	if(bFullFlag == false)
	{
		if(oInternalNetHub.AddPocketToOutputBuffer(ushCommand, p_chBuffer, iLength) == false)
		{
			LOG_P_0(LOG_CAT_E, "Pockets buffer is full.");
			return false;
		}
		if(oInternalNetHub.SendToAddress(a_ConnectionData, bResetPointer) == false)
		{
			LOG_P_0(LOG_CAT_E, "Socket error on sending data.");
			return false;
		}
	}
	else
	{
		LOG_P_0(LOG_CAT_E, "Server buffer is full.");
		return false;
	}
	return true;
}

// Отправка пакета на сервер немедленно.
bool Client::SendToServerImmediately(unsigned short ushCommand, char* p_chBuffer, int iLength, bool bResetPointer, bool bTryLock)
{
	bool bRes = false;
	TryMutexInit;
	//
	if(bTryLock) TryMutexLock(ptConnMutex);
	if((bExitSignal == false) && (bExitAccepted == false) && (bServerAlive == true)
	   && (oConvData.bSecured == true) && (oConvData.bFullOnServer == false))
	{
		if(SendToConnectionImmediately(oConvData.oConnectionData, ushCommand, false, p_chBuffer, iLength, bResetPointer))
			bRes = true;
	}
	if(bTryLock) TryMutexUnlock(ptConnMutex);
	if(bRes == false)
	{
		if(oConvData.bSecured == false)
		{
			LOG_P_0(LOG_CAT_W, "Sending rejected due not secured.");
			return true;
		}
		LOG_P_0(LOG_CAT_E, "Sending failed.");
	}
	return bRes;
}

// Отправка буфера на сервер.
bool Client::SendBufferToServer(bool bResetPointer, bool bTryLock)
{
	bool bRes = false;
	TryMutexInit;
	//
	if(bTryLock) TryMutexLock(ptConnMutex);
	if((bExitSignal == false) && (bExitAccepted == false) && (bServerAlive == true)
	   && (oConvData.bSecured == true) && (oConvData.bFullOnServer == false))
	{
		if(SendBufferToConnection(oConvData.oConnectionData, false, bResetPointer))
			bRes = true;
	}
	if(bTryLock) TryMutexUnlock(ptConnMutex);
	if(bRes == false)
	{
		if(oConvData.bSecured == false)
		{
			LOG_P_0(LOG_CAT_W, "Sending rejected due not secured.");
			return true;
		}
		LOG_P_0(LOG_CAT_E, "Sending failed.");
	}
	return bRes;
}

// Отправка буфера по соединению.
bool Client::SendBufferToConnection(NetHub::ConnectionData &a_ConnectionData, bool bFullFlag, bool bResetPointer)
{
	//
	if(bFullFlag == false)
	{
		if(oInternalNetHub.SendToAddress(a_ConnectionData, bResetPointer) == false)
		{
			LOG_P_0(LOG_CAT_E, "Socket error on sending data.");
			return false;
		}
	}
	else
	{
		LOG_P_0(LOG_CAT_E, "Server buffer is full.");
		return false;
	}
	return true;
}

// Установка указателя кэлбэка изменения статуса подключения сервера.
void Client::SetServerCommandArrivedCB(CBServerCommandArrived pf_CBServerCommandArrived)
{
	TryMutexInit;
	//
	TryMutexLock(ptConnMutex);
	pf_CBServerCommandArrivedInt = pf_CBServerCommandArrived;
	TryMutexUnlock(ptConnMutex);
}

// Установка указателя кэлбэка обработки принятых пакетов от сервера.
void Client::SetServerDataArrivedCB(CBServerDataArrived pf_CBServerDataArrived)
{
	TryMutexInit;
	//
	TryMutexLock(ptConnMutex);
	pf_CBServerDataArrivedInt = pf_CBServerDataArrived;
	TryMutexUnlock(ptConnMutex);
}

// Установка указателя кэлбэка обработки принятых пакетов от сервера.
void Client::SetServerStatusChangedCB(CBServerStatusChanged pf_CBServerStatusChanged)
{
	TryMutexInit;
	//
	TryMutexLock(ptConnMutex);
	pf_CBServerStatusChangedInt = pf_CBServerStatusChanged;
	TryMutexUnlock(ptConnMutex);
}

/// Удаление выбранного элемента в массиве принятых пакетов.
int Client::ReleaseDataInPositionC(uint uiPos, bool bTryLock)
{
	int iRes = DATA_NOT_FOUND;
	TryMutexInit;
	//
	if(bTryLock) TryMutexLock(ptConnMutex);
	if(oConvData.bFullOnClient == true)
	{
		oConvData.bFullOnClient = false;
		SendToConnectionImmediately(oConvData.oConnectionData, PROTO_A_BUFFER_READY);
	}
	iRes = oInternalNetHub.ReleaseDataInPosition(oConvData.mReceivedPockets, uiPos);
	if(bTryLock) TryMutexUnlock(ptConnMutex);
	if(iRes == DATA_NOT_FOUND)
	{
		LOG_P_0(LOG_CAT_E, "Trying to relese empty position.");
	}
	//else if(iRes == RETVAL_OK) LOG_P_1(LOG_CAT_I, "Position has been released.");
	return iRes;
}

// Добавление пакета в буфер отправки.
bool Client::AddPocketToOutputBufferC(unsigned short ushCommand, char *p_chBuffer, int iLength, bool bTryLock)
{
	bool bRes;
	TryMutexInit;
	//
	if(bTryLock) TryMutexLock(ptConnMutex);
	bRes = oInternalNetHub.AddPocketToOutputBuffer(ushCommand, p_chBuffer, iLength);
	if(bTryLock) TryMutexUnlock(ptConnMutex);
	return bRes;
}

// Доступ к первому элементу заданного типа из массива принятых пакетов от сервера.
int Client::AccessSelectedTypeOfDataC(void** pp_vDataBuffer, unsigned short ushType, bool bTryLock)
{
	int iRes;
	TryMutexInit;
	//
	if(bTryLock) TryMutexLock(ptConnMutex);
	iRes = oInternalNetHub.AccessSelectedTypeOfData(pp_vDataBuffer, oConvData.mReceivedPockets, ushType);
	if(bTryLock) TryMutexUnlock(ptConnMutex);
	return iRes;
}

// Поток приёма.
void* Client::RecThread(void *p_vPlug)
{
	ProtoParser::ParseResult oParsingResult;
	p_vPlug = p_vPlug;
	oConvData.bSecured = false;
	char* p_chData;
	int iLength;
	NetHub::ReceivedData* p_CurrentData;
	NetHub::ReceivedData oReceivedDataReserve;
	//
	bClientAlive = true;
	LOG_P_1(LOG_CAT_I, "Receiver thread has been started.");
	while(!bExitSignal)
	{
		oConvData.oConnectionData.iStatus = (int)recv(oConvData.oConnectionData.iSocket, oConvData.m_chData,
				sizeof(oConvData.m_chData), 0); // Принимаем пакет.
		pthread_mutex_lock(&ptConnMutex);
		if (oConvData.oConnectionData.iStatus > 0)
		{
			if(bServerAlive == false)
			{
				bServerAlive = true;
				if(pf_CBServerStatusChangedInt != nullptr)
				{
					// Вызов кэлбэка смены статуса сервера.
					pf_CBServerStatusChangedInt(true);
				}
			}
			p_chData = oConvData.m_chData;
			iLength = oConvData.oConnectionData.iStatus;
gDp:		oConvData.iCurrentFreePocket = oInternalNetHub.FindFreeReceivedPocketsPos(oConvData.mReceivedPockets);
			if(oConvData.iCurrentFreePocket != BUFFER_IS_FULL) p_CurrentData = &oConvData.mReceivedPockets[oConvData.iCurrentFreePocket];
			else p_CurrentData = &oReceivedDataReserve;
			oParsingResult = oProtoParser.ParsePocket(p_chData, iLength,
								p_CurrentData->oProtocolStorage, oConvData.bFullOnClient || (!oConvData.bSecured));
			switch(oParsingResult.iRes)
			{
				case PROTOPARSER_OK:
				{
					if(oParsingResult.bStored)
					{
						//LOG_P_1(LOG_CAT_I, "Received pocket nr." << (oConvData.iCurrentFreePocket + 1));
						oConvData.mReceivedPockets[oConvData.iCurrentFreePocket].bBusy = true;
					}
					else
					{
						if(pf_CBServerCommandArrivedInt != nullptr)
						{
							// Вызов кэлбэка принятия команды.
							pf_CBServerCommandArrivedInt(oParsingResult.ushTypeCode);
						}
					}
					// Блок взаимодействия.
					switch(oParsingResult.ushTypeCode)
					{
						case PROTO_S_PASSW_OK:
						{
							LOG_P_1(LOG_CAT_I, "Password accepted.");
							oConvData.bSecured = true;
							goto gI;
						}
						case PROTO_S_PASSW_ERR:
						{
							LOG_P_0(LOG_CAT_W, "Password rejected.");
							oConvData.bSecured = false;
							goto gI;
						}
						case PROTO_S_BUFFER_FULL:
						{
							LOG_P_1(LOG_CAT_W, "Buffer is full on server.");
							oConvData.bFullOnServer = true;
							goto gI;
						}
						case PROTO_A_BUFFER_READY:
						{
							LOG_P_1(LOG_CAT_I, "Buffer is ready on server.");
							oConvData.bFullOnServer = false;
							goto gI;
						}
						case PROTO_S_ACCEPT_LEAVING:
						{
							LOG_P_1(LOG_CAT_I, "Leaving accepted.");
gBE:						bExitAccepted = true;
							bExitSignal = true;
							pthread_mutex_unlock(&ptConnMutex);
							MSleep(USER_RESPONSE_MS);
							goto gCE;
						}
						case PROTO_S_SHUTDOWN_INFO:
						{
							LOG_P_1(LOG_CAT_I, "Server will be turned off.");
							goto gBE;
						}
						case PROTO_S_UNSECURED:
						{
							LOG_P_0(LOG_CAT_E, "Connection is unsecured.");
							goto gI;
						}
						case PROTO_S_BAN:
						{
							LOG_P_0(LOG_CAT_W, "Client IP address is banned.");
							goto gBE;
						}
						case PROTO_S_KICK:
						{
							LOG_P_0(LOG_CAT_W, "Client has been kicked out.");
							goto gBE;
						}
					}
					// Блок объектов.
					if(oParsingResult.bStored == true)
					{
						if(pf_CBServerDataArrivedInt != nullptr)
						{
							// Вызов кэлбэка прибытия данных.
							pf_CBServerDataArrivedInt(oParsingResult.ushTypeCode, oConvData.mReceivedPockets[oConvData.
									iCurrentFreePocket].oProtocolStorage.p_Data,
									oConvData.iCurrentFreePocket);
						}
					}
gI:					break;
				}
				case PROTOPARSER_UNKNOWN_COMMAND:
				{
					LOG_P_0(LOG_CAT_W, (char*)MSG_UNKNOWN_COMMAND << ": '" << oParsingResult.ushTypeCode << "'");
					break;
				}
				case PROTOPARSER_WRONG_FORMAT:
				{
					LOG_P_0(LOG_CAT_W, "Client" << (char*)MSG_WRONG_FORMAT);
					break;
				}
			}
			if(oInternalNetHub.FindFreeReceivedPocketsPos(oConvData.mReceivedPockets) == BUFFER_IS_FULL)
			{
				LOG_P_1(LOG_CAT_W, "Buffer is full on client.");
				oConvData.bFullOnClient = true;
				SendToConnectionImmediately(oConvData.oConnectionData, PROTO_C_BUFFER_FULL);
			}
			if(oParsingResult.p_chExtraData != 0)
			{
				p_chData = oParsingResult.p_chExtraData;
				iLength = oParsingResult.iExtraDataLength;
				goto gDp;
			}
		}
		else
		{
			if(bServerAlive == true)
			{
				LOG_P_0(LOG_CAT_W, "No connection with server.");
				pthread_mutex_unlock(&ptConnMutex);
				goto gCE;
			}
		}
		pthread_mutex_unlock(&ptConnMutex);
	}
gCE:LOG_P_1(LOG_CAT_I, "Exiting receiver thread.");
	if(pf_CBServerStatusChangedInt != 0)
	{
		pf_CBServerStatusChangedInt(false);
	}
	bServerAlive = false;
	RETURN_THREAD;
}

// Поток ожидания соединения с сервером.
void* Client::ConnectionThread(void* p_vPlug)
{
	p_vPlug = p_vPlug;
	//
	LOG_P_1(LOG_CAT_I, "Connection thread has been started.");
	LOG_P_1(LOG_CAT_I, "Connecting...");
	oConvData.oConnectionData.iStatus = connect(oConvData.oConnectionData.iSocket,
											(sockaddr*)oConvData.oConnectionData.ai_addr,
											(int)oConvData.oConnectionData.ai_addrlen);
#ifdef WIN32
	if(oConvData.oConnectionData.iStatus == SOCKET_ERROR)
		LOG_P_0(LOG_CAT_E, "Connection error: " << WSAGetLastError());
#else
	if(oConvData.oConnectionData.iStatus == -1)
	{
		LOG_P_0(LOG_CAT_W, "Connection error: " << strerror(errno));
	}
#endif
	LOG_P_1(LOG_CAT_I, "Exiting connection thread.");
	bConnectionThrAlive = false;
	RETURN_THREAD;
}

// Взятие адреса на входе.
void* Client::GetInAddr(sockaddr *p_SockAddr)
{
	if(p_SockAddr->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)p_SockAddr)->sin_addr);
	}
	return &(((struct sockaddr_in6 *)p_SockAddr)->sin6_addr);
}

// Клиентский поток.
void* Client::ClientThread(void *p_vPlug)
{
	TryMutexInit;
	//
	int iServerStatus;
	addrinfo o_Hints;
	addrinfo* p_Res = nullptr;
	p_vPlug = p_vPlug;
#ifdef WIN32
	WSADATA wsadata;
#endif
	//
	LOG_P_1(LOG_CAT_I, "Client thread has been started.");
	// Подготовка соединения клиента.
#ifdef WIN32
	if(WSAStartup(MAKEWORD(2, 2), &wsadata) != NO_ERROR)
	{
		LOG(LOG_CAT_E, "'WSAStartup' failed");
	}
#endif
	memset(&o_Hints, 0, sizeof o_Hints);
	o_Hints.ai_family = PF_UNSPEC;
	o_Hints.ai_socktype = SOCK_STREAM;
	o_Hints.ai_flags = AI_PASSIVE;
	o_Hints.ai_protocol = IPPROTO_TCP;
	iServerStatus = getaddrinfo(p_IPPortPasswordInt->p_chIPNameBuffer, p_IPPortPasswordInt->p_chPortNameBuffer, &o_Hints, &p_Res);
	if(iServerStatus != 0)
	{
		LOG_P_0(LOG_CAT_E, "'getaddrinfo': " << gai_strerror(iServerStatus));
		RETVAL_SET(RETVAL_ERR);
		goto ex;
	}
	oConvData.oConnectionData.iSocket =
			(int)socket(p_Res->ai_family, p_Res->ai_socktype, p_Res->ai_protocol); // Получение сокета для приёмника.
#ifdef WIN32
	if(oConvData.oConnectionData.iSocket == INVALID_SOCKET)
#else
	if(oConvData.oConnectionData.iSocket == -1)
#endif
	{
		LOG_P_0(LOG_CAT_E, "'socket': "  << gai_strerror(iServerStatus));
		RETVAL_SET(RETVAL_ERR);
		goto ex;
	}
	memcpy(&oConvData.oConnectionData.ai_addr, p_Res->ai_addr, sizeof(sockaddr));
	oConvData.oConnectionData.ai_addrlen = p_Res->ai_addrlen;
	oConvData.oConnectionData.iStatus = 1;
	LOG_P_1(LOG_CAT_I, "Starting connection thread.");
	bConnectionThrAlive = true;
	pthread_create(&ConnectThr, nullptr, ConnectionThread, nullptr); // Запуск потока ожидания соединения с сервером.
	for(unsigned char uchAttempt = 1; uchAttempt != 5; uchAttempt++)
	{
		for(unsigned char uchCAttempt = 0; uchCAttempt != 48; uchCAttempt++)
		{
			if(bConnectionThrAlive == false) goto gCO;
			MSleep(USER_RESPONSE_MS);
		}
		LOG_P_0(LOG_CAT_W, "Selected IP does not respond.");
		oConvData.oConnectionData.iStatus = -1;
		goto exr;
gCO:    if(uchAttempt == 4)
		{
			oConvData.oConnectionData.iStatus = -1;
			goto exr;
		}
		if(oConvData.oConnectionData.iStatus == 0)
			break;
		else if(oConvData.oConnectionData.iStatus == -1)
		{
			MSleep(WAITING_FOR_SRV_STEP);
			oConvData.oConnectionData.iStatus = 1;
			LOG_P_1(LOG_CAT_I, "Attempt nr." << (int)uchAttempt << ". Waiting for connection...");
			bConnectionThrAlive = true;
			pthread_create(&ConnectThr, nullptr, ConnectionThread, 0); // Запуск потока ожидания соединения с сервером.
		}
	}
	if(oConvData.oConnectionData.iStatus == -1)
	{
		LOG_P_0(LOG_CAT_W, "Connection failed.");
		goto exr;
	}
	LOG_P_1(LOG_CAT_I, "Connected with the server.");
	//
	SendToConnectionImmediately(oConvData.oConnectionData, PROTO_C_SEND_PASSW, false, p_IPPortPasswordInt->p_chPasswordNameBuffer,
								(int)strlen(p_IPPortPasswordInt->p_chPasswordNameBuffer));
	LOG_P_1(LOG_CAT_I, "Authentification is requested.");
	LOG_P_1(LOG_CAT_I, "Starting receiver thread.");
	pthread_create(&RecThr, nullptr, RecThread, nullptr); // Запуск потока приёма.
	//
	LOG_P_1(LOG_CAT_I, "Waiting for response.");
	for(unsigned char uchAtt = 0; uchAtt != CLIENT_WAITING_ATTEMPTS; uchAtt++)
	{
		if(bServerAlive)
		{
			LOG_P_1(LOG_CAT_I, "Ready for conversation.");
			goto gSA;
		}
		MSleep(USER_RESPONSE_MS);
	}
	LOG_P_0(LOG_CAT_W, "Server reject conversation.");
	pf_CBServerStatusChangedInt(false);
	goto gFN;
gSA:while(!bExitSignal)
	{
		if(!bServerAlive) goto gFN;
		MSleep(USER_RESPONSE_MS);
	}
	if(bExitAccepted)
	{
		LOG_P_1(LOG_CAT_I, "Regulated disconnection.")
	}
	else
	{
gFN:	LOG_P_1(LOG_CAT_W, "Cancelling receiver thread.");
		pthread_cancel(RecThr);
	}
	// Закрытие приёмника.
exr:LOG_P_1(LOG_CAT_I, "Closing socket...");
#ifndef WIN32
	shutdown(oConvData.oConnectionData.iSocket, SHUT_RDWR);
	close(oConvData.oConnectionData.iSocket);
#else
	closesocket(oConvData.oConnectionData.iSocket);
#endif
ex:
#ifdef WIN32
	WSACleanup();
#endif
	freeaddrinfo(p_Res);
	LOG_P_1(LOG_CAT_I, "Exiting client thread.");
	bClientAlive = false;
	TryMutexLock(ptConnMutex);
	for(uint uiPos = 0; uiPos < S_MAX_STORED_POCKETS; uiPos++)
	{
		oInternalNetHub.ReleaseDataInPosition(oConvData.mReceivedPockets, uiPos);
	}
	TryMutexUnlock(ptConnMutex);
	RETURN_THREAD;
}
