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
#include "../Z-Hub/logger.h"
#include "../Z-Hub/main-hub.h"
#include "z-editor-defs.h"
#include "../Z-Hub/parser-ext.h"
#include "Client/client.h"
#include "schematic-window.h"

//== МАКРОСЫ.
#define LOG_DIR_PATH			"../Z-Editor/logs/"

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
	/// Установка соединения сигнала на обновление граф. окна с граф. окном.
	void SetSchViewSignalConnection();
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
	/// Процедуры запуска клиента.
	static bool ClientStartProcedures();
							///< \return true - при удаче.
	/// Процедуры остановки клиента.
	static bool ClientStopProcedures();
							///< \return true - при удаче.
	/// Загрузка конфигурации клиента.
	static bool LoadClientConfig();
							///< \return true - при удаче.
	/// Сохранение конфигурации клиента.
	static bool SaveClientConfig();
							///< \return true - при удаче.

private slots:
	/// При переключении кнопки 'Schematic'.
	static void on_actionSchematic_triggered(bool checked);
							///< \param[in] checked Позиция переключателя.
private slots:
	/// При переключении кнопки 'Соединение при включении'.
	static void on_actionConnect_at_startup_triggered(bool checked);
							///< \param[in] checked Позиция переключателя.
signals:
	// Сигнал для удалённого (относительно потока) обновления граф. окна.
	void RemoteUpdateSchView();

public:
	static int iInitRes; ///< Результат инициализации.
	static SchematicWindow* p_SchematicWindow; ///< Указатель на класс окна схематического обзора.
	static bool bSchemaIsOpened; ///< Флаг открытого обзора схемы.
	static WidgetsThrAccess* p_WidgetsThrAccess; ///< Указатель на объект класса доступа к интерфейсу.

private:
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD
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

/// Класс потоко-независимого доступа к интерфейсу.
class WidgetsThrAccess : public QObject
{
	Q_OBJECT

public:
	/// Структура пары строк для вызова диалога собщения.
	struct StrMsgDialogPair
	{
		QString strCaption; ///< Строка с заголовком.
		QString strMessage; ///< Строка с сообщением.
	};

private:
	static Ui::MainWindow* p_uiInt; ///< Внутренний указатель на главное окно.

public:
	static StrMsgDialogPair oStrMsgDialogPair; ///< Объект для заполнения данными для вывода диалога сообщения.

public:
	/// Конструктор.
	WidgetsThrAccess(Ui::MainWindow* p_ui);
							///< \param[in] p_ui Указатель интерфейс.

public slots:
	/// Создание, вызов и удаление диалога.
	static void DoDialog();
	/// Очистка сцены.
	void ClearScene();
};

#endif // MAINWINDOW_H
