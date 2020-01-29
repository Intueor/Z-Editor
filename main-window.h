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
#include "logger.h"
#include "main-hub.h"
#include "parser-ext.h"
#include "schematic-window.h"

//== МАКРОСЫ.
#define LOG_DIR_PATH			"../Z-Editor/logs/"

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class MainWindow;
}

//== ПРЕД-ДЕКЛАРАЦИИ.
class SchematicWindow;

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

private slots:
	/// При переключении кнопки 'Schematic'.
	static void on_actionSchematic_triggered(bool checked);
							///< \param[in] checked Позиция переключателя.
signals:
	// Сигнал для удалённого (относительно потока) обновления граф. окна.
	void RemoteUpdateSchView();

public:
	static int iInitRes; ///< Результат инициализации.
	static SchematicWindow* p_SchematicWindow; ///< Указатель на класс окна схематического обзора.
	static bool bSchemaIsOpened; ///< Флаг открытого обзора схемы.

private:
	static Ui::MainWindow *p_ui; ///< Указатель на UI.
	static QSettings* p_UISettings; ///< Указатель на строку установок UI.
	static const char* cp_chUISettingsName; ///< Указатель на имя файла с установками UI.
	static QLabel* p_QLabelStatusBarText; ///< Указатель на объект метки статуса.
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD
};

#endif // MAINWINDOW_H
