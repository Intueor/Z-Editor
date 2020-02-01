#ifndef SCHEMATICWINDOW_H
#define SCHEMATICWINDOW_H

//== ВКЛЮЧЕНИЯ.
#include <QMainWindow>
#include <QSettings>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include "main-window.h"
#include "schematic-view.h"

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class SchematicWindow;
}

//== ПРЕД-ДЕКЛАРАЦИИ.
class MainWindow;

//== КЛАССЫ.
/// Класс окна обзора.
class SchematicWindow : public QMainWindow
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit SchematicWindow(QWidget* p_parent = nullptr);
							///< \param[in] p_parent Указатель на родительский виджет.
	/// Деструктор.
	~SchematicWindow();
	/// Процедуры при закрытии окна обзора.
	void closeEvent(QCloseEvent* event);
							///< \param[in] event Указатель на событие.
	/// Закрытие с флагом внешнего управления.
	void RefClose();

public:
	static MainWindow* p_MainWindow; ///< Указатель на класс главного окна.
	QGraphicsScene oScene; ///< Объект сцены.
	static QGraphicsScene* p_QGraphicsScene; ///< Статический указатель на используемый нестатический объект сцены.
	static QPolygon oPolygonForScaler; ///< Объект полигона для отрисовки скалера.

public slots:
	/// Обновление сцены.
	static void UpdateScene();
	/// Обновление от таймера мерцания выбранных элементов.
	static void UpdateSelectionFlash();
	/// Очистка сцены.
	static void ClearScene();

private:
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD
	static Ui::SchematicWindow *p_ui; ///< Указатель на UI.
	static const char* cp_chUISettingsName; ///< Указатель на имя файла с установками UI.
	static QSettings* p_UISettings; ///< Указатель на строку установок UI.
	static QTimer oQTimerSelectionFlashing; ///< Таймер мерцания выбранных элементов.
	static bool bRefClose; ///< Признак закрытия окна снаружи.
};

#endif // SCHEMATICWINDOW_H
