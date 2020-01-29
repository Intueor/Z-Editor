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

//== МАКРОСЫ.
#define LOG_DIR_PATH			"../Z-Editor/logs/"

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class MainWindow;
}

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

private:

private slots:

public:
	static int iInitRes; ///< Результат инициализации.

private:
	static Ui::MainWindow *p_ui; ///< Указатель на UI.
	static QSettings* p_UISettings; ///< Указатель на строку установок UI.
	static const char* cp_chUISettingsName; ///< Указатель на имя файла с установками UI.
	static QLabel* p_QLabelStatusBarText; ///< Указатель на объект метки статуса.
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD
};

#endif // MAINWINDOW_H
