#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//== ВКЛЮЧЕНИЯ.
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif
#include <QMainWindow>
#include <QSettings>
#include <QLabel>
#include <QVector>
#include <QListWidget>
#include "../Z-Hub/logger.h"
#include "../Z-Hub/main-hub.h"
#include "z-editor-defs.h"
#include "../Z-Hub/parser-ext.h"
#include "Client/client.h"
#include "schematic-window.h"

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class MainWindow;
}

//== ПРЕД-ДЕКЛАРАЦИИ.
class SchematicWindow;
class WidgetsThrAccess;

// Для избежания ошибки при доступе из другого потока.
Q_DECLARE_METATYPE(QVector<int>)
//

//== КЛАССЫ.
/// Класс главного окна.
class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit MainWindow(QWidget* p_parent = nullptr);
							///< \param[in] p_parent Указатель на родительский виджет.
	/// Деструктор.
	~MainWindow();
	/// Процедуры при закрытии окна приложения.
	void closeEvent(QCloseEvent* event);
							///< \param[in] event Указатель на событие.
	/// Установка сигналов сообщения с граф. окном.
	void SetSchWindowSignalConnections();
	/// Для внешнего переключения чекбокса кнопки 'Схема'.
	static void UncheckSchemaCheckbox();
private:
	/// Кэлбэк обработки отслеживания статута сервера.
	static void ServerStatusChangedCallback(bool bStatus);
							///< \param[in] a_NetHub Ссылка на текущий хаб.
							///< \param[in] bStatus Статус сервера.
	/// Кэлбэк обработки прихода команд от сервера.
	static void ServerCommandArrivedCallback(unsigned short ushCommand);
							///< \param[in] a_NetHub Ссылка на текущий хаб.
							///< \param[in] ushCommand Код команды.
	/// Кэлбэк обработки прихода пакетов от сервера.
	static void ServerDataArrivedCallback(unsigned short ushType, void *p_ReceivedData, int iPocket);
							///< \param[in] ushType Тип принятого.
							///< \param[in] p_ReceivedData Указатель на принятый пакет.
							///< \param[in] iPocket Номер пакета для освобождения с ReleaseDataInPositionC после использования.
	/// Загрузка конфигурации клиента.
	static bool LoadClientConfig();
							///< \return true - при удаче.
	/// Сохранение конфигурации клиента.
	static bool SaveClientConfig();
							///< \return true - при удаче.
	/// Установка имени и подсказки для метки текущего сервера.
	static void SetServerLabelData(bool bIsIPv4);
							///< \param[in] bIsIPv4 Признак протокола IPv4.
	/// Процедуры запуска клиента.
	static void ClientStartProcedures();
	/// Установка текста строки статуса.
	static void SetStatusBarText(QString strMsg);
							///< \param[in] strMsg Строка с сообщением.

private slots:
	/// Процедуры остановки клиента.
	static void ClientStopProcedures();
	/// Вызов диалога сообщения.
	static void MsgDialog(QString strCaption, QString strMsg);
							///< \param[in] strCaption Заголовок.
							///< \param[in] strMsg Сообщение.
	/// Установка кнопок соединения в позицию по состоянию.
	static void SetConnectionButtonsState(bool bConnected);
							///< \param[in] bConnected true, если подключено.
	/// При переключении кнопки 'Schematic'.
	static void on_actionSchematic_triggered(bool checked);
							///< \param[in] checked Позиция переключателя.
	/// При переключении кнопки 'Соединение при включении'.
	static void on_actionConnect_at_startup_triggered(bool checked);
							///< \param[in] checked Позиция переключателя.
	/// При нажатии кнопки 'Соединение'.
	static void on_pushButton_Connect_clicked();
	/// При нажатии кнопки 'Разъединить'.
	static void on_pushButton_Disconnect_clicked();

signals:
	/// Сигнал обновления граф. окна.
	void RemoteUpdateSchView();
	/// Сигнал установки блокировки кнопок подключения в позицию по состоянию.
	void RemoteSetConnectionButtonsState(bool bConnected);
							///< \param[in] bConnected true, если подключено.
	/// Сигнал вызова диалога с сообщения.
	void RemoteMsgDialog(QString strCaption, QString strMsg);
							///< \param[in] strCaption Заголовок.
							///< \param[in] strMsg Сообщение.
	/// Сигнал очистки сцены.
	void RemoteClearScene();
	/// Сигнал начала процедур остановки клиента.
	void RemoteClientStopProcedures();

public:
	static int iInitRes; ///< Результат инициализации.
	static SchematicWindow* p_SchematicWindow; ///< Указатель на класс окна схематического обзора.
	static bool bSchemaIsOpened; ///< Флаг открытого обзора схемы.
	static WidgetsThrAccess* p_WidgetsThrAccess; ///< Указатель на объект класса доступа к интерфейсу.

private:
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD
	static MainWindow* p_This; ///< Указатель на инициализированного себя.
	static Ui::MainWindow *p_ui; ///< Указатель на UI.
	static QSettings* p_UISettings; ///< Указатель на строку установок UI.
	static const char* cp_chUISettingsName; ///< Указатель на имя файла с установками UI.
	static QLabel* p_QLabelStatusBarText; ///< Указатель на объект метки статуса.
	static Client* p_Client; ///< Указатель на объект клиента.
	static PServerName oPServerName; ///< Объект структуры для заполнения именем сервера.
	static char m_chIPInt[IP_STR_LEN]; ///< Внутренний массив строки IP.
	static char m_chPortInt[PORT_STR_LEN]; ///< Внутренний массив порта.
	static char m_chPasswordInt[AUTH_PASSWORD_STR_LEN]; ///< Внутренний массив строки пароля.
	static NetHub::IPPortPassword oIPPortPassword; ///< Структура с указателями на IP, порт и пароль.
	static char chLastClientRequest; ///< Последний запрос клиента.
	static bool bAutoConnection; ///< Флаг автосоединения при запуске.
};

/// Класс добавки данных сервера к стандартному элементу лист-виджета.
class ServersListWidgetItem : public QObject, public QListWidgetItem
{
	Q_OBJECT

private:
	char m_chName[SERVER_NAME_STR_LEN]; // Массив имени сервера.
	char m_chIP[IP_STR_LEN]; // Массив строки IP.
	char m_chPort[PORT_STR_LEN]; // Массив строки порта.
	char m_chPassword[AUTH_PASSWORD_STR_LEN]; // Массив строки пароля.

public:
	/// Конструктор.
	ServersListWidgetItem(NetHub::IPPortPassword* p_IPPortPassword, bool bIsIPv4, char* p_chName,
						  QListWidget* p_ListWidget = nullptr);
							///< \param[in] p_IPPortPassword Указатель на структуру с указателями на массивы строк с описанием сервера.
							///< \param[in] bIsIPv4 Признак протокола IPv4.
							///< \param[in] p_chName Указатель на строку с именем сервера или 0 для пустой строки.
							///< \param[in] p_ListWidget Указатель на родительский лист-виджет.
	/// Получение структуры с указателями на строчные массивы типа IPPortPassword.
	NetHub::IPPortPassword GetIPPortPassword();
							///< \return Структура с указателями на сохранённые массивы строк с описанием сервера.
	/// Копирование массива строки с паролем во внутренний буфер.
	void SetPassword(char* p_chPassword);
							///< \param[in] p_chPassword Указатель на массив строки с паролем.
	/// Получение указателя строку с именем сервера или 0 при пустой строке.
	char* GetName();
							///< \return Получение структуры с указателями на сохранённые массивы строк с описанием сервера.
};
#endif // MAINWINDOW_H
