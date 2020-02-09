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
#include "../Z-Hub/Dialogs/set-server-dialog.h"

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class MainWindow;
}

//== ПРЕД-ДЕКЛАРАЦИИ.
class SchematicWindow;
class WidgetsThrAccess;
class ServersListWidgetItem;
class GraphicsElementItem;
class GraphicsLinkItem;
class GraphicsGroupItem;

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
	/// Проверка на совпадение цифровых IP.
	static bool CheckEqualsNumbers(NumericAddress& oNumericAddressOne, NumericAddress& oNumericAddressTwo);
							///< \param[in] oNumericAddressOne Ссылка на первый объект структуры цифрового адреса.
							///< \param[in] oNumericAddressTwo Ссылка на второй объект структуры цифрового адреса.
							///< \return true - при совпадении.
	/// Обмен выбранного сервера списка серверов с текущим сервером.
	static void CurrentServerSwap(ServersListWidgetItem* p_ServersListWidgetItem);
							///< \param[in] p_ServersListWidgetItem Указатель на строку списка для обмена.
	/// Удалённое (относительно потока) обновление граф. окна и отправка данных о рабочем фрейме.
	static void RemoteUpdateSchViewAndSendRFrame();
	/// Удаление линков у элемента.
	static void EraseLinksFromElement(unsigned long long ullIDInt);
							///< \param[in] ullIDInt ИД элемента.
private slots:
	/// Процедуры остановки клиента.
	static void SlotClientStopProcedures();
	/// Вызов диалога сообщения.
	static void SlotMsgDialog(QString strCaption, QString strMsg);
							///< \param[in] strCaption Заголовок.
							///< \param[in] strMsg Сообщение.
	/// Установка кнопок соединения в позицию по состоянию.
	static void SlotSetConnectionButtonsState(bool bConnected);
							///< \param[in] bConnected true, если подключено.
	/// При переключении кнопки 'Schematic'.
	static void on_action_Schematic_triggered(bool checked);
							///< \param[in] checked Позиция переключателя.
	/// При переключении кнопки 'Соединение при включении'.
	static void on_action_ConnectAtStartup_triggered(bool checked);
							///< \param[in] checked Позиция переключателя.
	/// При нажатии кнопки 'Соединение'.
	static void on_pushButton_Connect_clicked();
	/// При нажатии кнопки 'Разъединить'.
	static void on_pushButton_Disconnect_clicked();
	/// При нажатии ПКМ на элементе списка серверов.
	static void on_listWidget_Servers_customContextMenuRequested(const QPoint &pos);
							///< \param[in] pos Ссылка на координаты точки указателя в виджете.
	/// При нажатии ПКМ на метке с именем текущего сервера.
	static void on_label_CurrentServer_customContextMenuRequested(const QPoint &pos);
							///< \param[in] pos Ссылка на координаты точки указателя в виджете.
	/// При нажатии на кнопку 'Добавить' сервер.
	static void on_pushButton_Add_clicked();
	/// При двойном клике на элементе лист-виджета.
	static void on_listWidget_Servers_itemDoubleClicked(QListWidgetItem* item);
							///< \param[in] QListWidgetItem Указатель на элемент.

signals:
	/// Обновление граф. окна.
	void RemoteUpdateSchView();
	/// Установка блокировки кнопок подключения в позицию по состоянию.
	void RemoteSlotSetConnectionButtonsState(bool bConnected);
							///< \param[in] bConnected true, если подключено.
	/// Вызоа диалога с сообщения.
	void RemoteSlotMsgDialog(QString strCaption, QString strMsg);
							///< \param[in] strCaption Заголовок.
							///< \param[in] strMsg Сообщение.
	/// Очистка сцены.
	void RemoteClearScene();
	/// Начало процедур остановки клиента.
	void RemoteSlotClientStopProcedures();

public:
	static int iInitRes; ///< Результат инициализации.
	static SchematicWindow* p_SchematicWindow; ///< Указатель на класс окна схематического обзора.
	static WidgetsThrAccess* p_WidgetsThrAccess; ///< Указатель на объект класса доступа к интерфейсу.
	static QSettings* p_UISettings; ///< Указатель на строку установок UI.
	static Client* p_Client; ///< Указатель на объект клиента.
	static bool bBlockingGraphics; ///< Признак блокировки главным окном.
	static bool bFrameRequested; ///< Признак запрошенного кадра с сервера.

private:
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD
	static MainWindow* p_This; ///< Указатель на инициализированного себя.
	static Ui::MainWindow *p_ui; ///< Указатель на UI.
	static const char* cp_chUISettingsName; ///< Указатель на имя файла с установками UI.
	static QLabel* p_QLabelStatusBarText; ///< Указатель на объект метки статуса.
	static PServerName oPServerName; ///< Объект структуры для заполнения именем сервера.
	static char m_chIPInt[IP_STR_LEN]; ///< Внутренний массив строки IP.
	static char m_chPortInt[PORT_STR_LEN]; ///< Внутренний массив порта.
	static char m_chPasswordInt[AUTH_PASSWORD_STR_LEN]; ///< Внутренний массив строки пароля.
	static NetHub::IPPortPassword oIPPortPassword; ///< Структура с указателями на IP, порт и пароль.
	static char chLastClientRequest; ///< Последний запрос клиента.
	static bool bBlockConnectionButtons; ///< Флаг блокировки кнопок соединения от повторного нажатия при программных переключениях.
	static PSchReadyFrame oPSchReadyFrame; ///< Объект параметров запрашиваемого окна схемы.
};

/// Класс добавки данных сервера к стандартному элементу лист-виджета.
class ServersListWidgetItem : public QObject, public QListWidgetItem
{
	Q_OBJECT

public:
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
							///< \return Структура с указателями на сохранённые массивы строк с описанием сервера.
	/// Получение указателя строку с именем сервера или 0 при пустой строке.
	char* GetName();
							///< \return Получение структуры с указателями на сохранённые массивы строк с описанием сервера.
};

/// Класс потоко-независимого доступа к интерфейсу.
class WidgetsThrAccess : public QObject
{
	Q_OBJECT

private:
	static Ui::MainWindow* p_uiInt; ///< Внутренний указатель на главное окно.

public:
	static GraphicsElementItem* p_ConnGraphicsElementItem; ///< Обменный ук. на графический объект элемента.
	static GraphicsLinkItem* p_ConnGraphicsLinkItem; ///< Обменный ук. на графический объект линка.
	static GraphicsGroupItem* p_ConnGraphicsGroupItem; ///< Обменный ук. на графический объект группы.
	//
	static PSchElementBase* p_PSchElementBase; ///< Указатель на данные по элементу схемы.
	static PSchLinkBase* p_PSchLinkBase; ///< Указатель на данные по линку схемы.
	static PSchGroupBase* p_PSchGroupBase; ///< Указатель на данные по группе схемы.

public:
	/// Конструктор.
	WidgetsThrAccess(Ui::MainWindow* p_ui);
							///< \param[in] p_ui Указатель интерфейс.

public slots:
	/// Добавление графического объекта элемента.
	static void AddGraphicsElementItem();
	/// Удаление графического объекта элемента.
	static void RemoveGraphicsElementItem();
	/// Удаление графического объекта группы.
	static void RemoveGraphicsGroupItem();
	/// Добавление графического объекта линка.
	static void AddGraphicsLinkItem();
	/// Добавление графического объекта группы.
	static void AddGraphicsGroupItem();
	/// Установка ширины строки названия группы по указателю p_ConnGraphicsGroupItem.
	static void GroupLabelWidthSet();
	/// Установка геометрии групбокса элемента по указателю p_ConnGraphicsElementItem и размеру из oDbPointFrameSize.
	static void ElementGroupBoxSizeSet();
	/// Очистка сцены.
	void ClearScene();
};

#endif // MAINWINDOW_H
