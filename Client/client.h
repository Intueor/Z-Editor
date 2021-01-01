#ifndef CLIENT_H
#define CLIENT_H

//== ВКЛЮЧЕНИЯ.
#ifdef WIN32
#define _WINSOCKAPI_
#define _TIMESPEC_DEFINED
#endif
#ifndef WIN32
#include <algorithm>
#endif
#include <signal.h>
#ifndef WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <termios.h>
#else
#include <WS2tcpip.h>
#endif
#include "../Z-Hub/Server/net-hub.h"

//== МАКРОСЫ.
#define CLIENT_WAITING_ATTEMPTS		128

//== ОПРЕДЕЛЕНИЯ ТИПОВ.
typedef void (*CBServerCommandArrived)(unsigned short ushCommand);
typedef void (*CBServerDataArrived)(unsigned short ushType, void *p_ReceivedData, int iPocket);
typedef void (*CBServerStatusChanged)(bool bConnected);

//== КЛАССЫ.
/// Класс клиента.
class Client
{
private:
	/// Структура принятых данных и описания соединения.
	struct ConversationData
	{
		int iConnection; ///< ИД соединения.
		NetHub::ReceivedData mReceivedPockets[C_MAX_STORED_POCKETS]; ///< Массив принятых пакетов.
		NetHub::ConnectionData oConnectionData; ///< Данные по соединению.
		char m_chData[MAX_DATA]; ///< Принятый пакет.
		int iCurrentFreePocket; ///< Текущий свободный пакет в массиве.
		bool bFullOnServer; ///< Флаг переполнения буфера на сервере.
		bool bFullOnClient; ///< Флаг переполнения буфера на клиенте.
		bool bSecured; ///< Флаг защищённого соединения.
	};

private:
	static bool bClientAlive; ///< Сигнал присутствия клиента.
	static bool bExitSignal; ///< Сигнал на общее завершение.
	static bool bExitAccepted; ///< Сигнал подтверждённого выхода.
	static bool bServerAlive; ///< Сигнал присутствия сервера.
	static bool bConnectionThrAlive; ///< Сигнал живого потока соединения.
	static pthread_mutex_t ptLogMutex; ///< Инициализатор мьютекса лога.
	static pthread_mutex_t ptConnMutex; ///< Инициализатор мьютекса соединений.
	static ConversationData oConvData; ///< Описания соединения.
	static ProtoParser oProtoParser; ///< Объект парсера.
	static pthread_t RecThr; ///< Идентификатор потока приёма.
	static pthread_t ConnectThr; ///< Идентификатор потока соединения.
	static pthread_t ClientThr; ///< Идентификатор потока клиента.
	static char* p_chSettingsPathInt; ///< Ссылка на строку с путём к установкам клиента.
	static CBServerCommandArrived pf_CBServerCommandArrivedInt; ///< Указатель на кэлбэк приёма команд.
	static CBServerDataArrived pf_CBServerDataArrivedInt; ///< Указатель на кэлбэк приёма пакетов.
	static CBServerStatusChanged pf_CBServerStatusChangedInt; ///< Указатель на кэлбэк отслеживания статута сервера.
	static NetHub::IPPortPassword* p_IPPortPasswordInt; ///< Внутреннй указатель на структуру со строками с установками клиента.
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD

public:
	static NetHub oInternalNetHub; ///< Внутренний объект NetHub.

public:
	/// Конструктор.
	Client(pthread_mutex_t ptLogMutex);
								///< \param[in] ptLogMutex Инициализатор мьютекса лога.
	/// Деструктор.
	~Client();
	/// Запрос запуска сервера.
	static bool Start(NetHub::IPPortPassword *p_IPPortPassword);
								///< \param[in] p_IPPortPassword ///< Указатель на строку с путём к установкам сервера.
								///< \return true, если удачно.
	/// Запрос остановки сервера.
	static bool Stop();
								///< \return True если процесс остановки понадобился.
	/// Запрос готовности.
	static bool CheckServerAlive();
								///< \return true - готов.
	/// Запрос статуса авторизации.
	static bool CheckConnectionSecured();
								///< \return true - авторизован.
	/// Отправка пакета на сервер немедленно.
	static bool SendToServerImmediately(unsigned short ushCommand, char* p_chBuffer, int iLength,
										bool bResetPointer = true, bool bTryLock = true);
								///< \param[in] ushCommand Код команды протокола.
								///< \param[in] p_chBuffer Указатель на буфер с данными для отправки.
								///< \param[in] iLength Длина буфера в байтах.
								///< \param[in] bResetPointer Сбрасывать ли указатель на начало буфера (для нового заполнения).
								///< \param[in] bTryLock Установить в false при использовании внутри кэлбэков.
								///< \return true, если удачно.
	/// Отправка буфера на сервер.
	static bool SendBufferToServer(bool bResetPointer = true, bool bTryLock = true);
								///< \param[in] bResetPointer Сбрасывать ли указатель на начало буфера (для нового заполнения).
								///< \param[in] bTryLock Установить в false при использовании внутри кэлбэков.
								///< \return true, если удачно.
	/// Добавление пакета в буфер отправки.
	static bool AddPocketToOutputBufferC(unsigned short ushCommand, char *p_chBuffer = nullptr, int iLength = 0, bool bTryLock = true);
								///< \param[in] ushCommand Команда, которая будет задана в начале пакета.
								///< \param[in] p_chBuffer Указатель на буффер с данными.
								///< \param[in] iLength Длина пакета в байтах.
								///< \param[in] bTryLock Установить в false при использовании внутри кэлбэков.
								///< \return true, при удаче.
	/// Доступ к первому элементу заданного типа из массива принятых пакетов от текущего клиента.
	static int AccessSelectedTypeOfDataC(void** pp_vDataBuffer, unsigned short ushType, bool bTryLock = true);
								///< \param[in,out] pp_vDataBuffer Указатель на указатель на буфер с данными.
								///< \param[in] ushType Тип искомого пакета.
								///< \param[in] bTryLock Установить в false при использовании внутри кэлбэков.
								///< \return Номер в массиве при удаче или DATA_NOT_FOUND соотв.
	/// Удаление выбранного элемента в массиве принятых пакетов.
	static int ReleaseDataInPositionC(uint uiPos, bool bTryLock = true);
								///< \param[in] uiPos Позиция в массиве.
								///< \param[in] bTryLock Установить в false при использовании внутри кэлбэков.
								///< \return RETVAL_OK, если удачно и NO_CONNECTION соотв.
	/// Установка указателя кэлбэка изменения статуса подключения сервера.
	static void SetServerCommandArrivedCB(CBServerCommandArrived pf_CBServerCommandArrived);
								///< \param[in] pf_CBServerCommandArrived Указатель на пользовательскую функцию.
	/// Установка указателя кэлбэка обработки принятых пакетов от сервера.
	static void SetServerDataArrivedCB(CBServerDataArrived pf_CBServerDataArrived);
								///< \param[in] pf_CBServerDataArrived Указатель на пользовательскую функцию.
	/// Установка указателя кэлбэка обработки отслеживания статута сервера.
	static void SetServerStatusChangedCB(CBServerStatusChanged pf_CBServerStatusChanged);
								///< \param[in] pf_CBServerStatusChanged Указатель на пользовательскую функцию.
private:
	/// Отправка пакета по соединению немедленно.
	static bool SendToConnectionImmediately(NetHub::ConnectionData& a_ConnectionData, unsigned short ushCommand, bool bFullFlag = false,
											char *p_chBuffer = nullptr, int iLength = 0, bool bResetPointer = true);
								///< \param[in] a_ConnectionData Ссылка структуру принятых данных и описания соединения.
								///< \param[in] ushCommand Код команды протокола.
								///< \param[in] bFullFlag Признак переполнения на сервере для фиктивной попытки отправки.
								///< \param[in] p_chBuffer Указатель на буфер с данными для отправки.
								///< \param[in] iLength Длина буфера в байтах.
								///< \param[in] bResetPointer Сбрасывать ли указатель на начало буфера (для нового заполнения).
								///< \return true, если удачно.
	/// Отправка буфера по соединению.
	static bool SendBufferToConnection(NetHub::ConnectionData& a_ConnectionData, bool bFullFlag = false, bool bResetPointer = true);
								///< \param[in] a_ConnectionData Ссылка структуру принятых данных и описания соединения.
								///< \param[in] bFullFlag Признак переполнения на сервере для фиктивной попытки отправки.
								///< \param[in] bResetPointer Сбрасывать ли указатель на начало буфера (для нового заполнения).
								///< \return true, если удачно.
	/// Взятие адреса на входе.
	static void* GetInAddr(sockaddr *p_SockAddr);
								///< \param[in] p_SockAddr Указатель на структуру описания адреса сокета.
	/// Поток приёма.
	static void* RecThread(void *p_vPlug);
								///< \param[in] p_vPlug Заглушка.
								///< \return Заглушка.
	/// Поток ожидания соединения с сервером.
	static void* ConnectionThread(void *p_vPlug);
								///< \param[in] p_vPlug Заглушка.
								///< \return Заглушка.
	/// Клиентский поток.
	static void* ClientThread(void *p_vPlug);
								///< \param[in] p_vPlug Заглушка.
								///< \return Заглушка.
};

#endif // CLIENT_H
