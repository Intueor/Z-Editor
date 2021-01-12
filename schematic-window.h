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
#include "graphics-element-item.h"
#include "graphics-background-item.h"

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class SchematicWindow;
}

//== ПРЕД-ДЕКЛАРАЦИИ.
class MainWindow;
class SafeMenu;

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
	void closeEvent(QCloseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Закрытие с флагом внешнего управления.
	void RefClose();
	/// Получение указателя на окно обзора.
	static SchematicView* GetSchematicView();
							///< \return Указатель на окно обзора.
	/// Закрытие и сброс меню.
	static void ResetMenu();

private:
	// Кэлбэк обработки изменения окна обзора от класса вида.
	static void SchematicViewFrameChangedCallback(QRectF oQRectFVisibleFrame);
							///< \param[in] oQRectFVisibleFrame Структура определения прямоугольника области видимости.

public:
	static MainWindow* p_MainWindow; ///< Указатель на класс главного окна.
	QGraphicsScene oScene; ///< Объект сцены.
	static QGraphicsScene* p_QGraphicsScene; ///< Статический указатель на используемый нестатический объект сцены.
	static SchematicView* p_SchematicView; ///< Статический указатель на объект схематического вида.
	static QVector<GraphicsElementItem*> vp_SelectedElements; ///< Вектор с указателями на выбранные граф. элементы.
	static QVector<GraphicsGroupItem*> vp_SelectedGroups; ///< Вектор с указателями на выбранные граф. группы.
	static QVector<GraphicsElementItem*> vp_Elements; ///< Вектор с указателями на граф. элементы.
	static QVector<GraphicsGroupItem*> vp_Groups; ///< Вектор с указателями на граф. группы.
	static QVector<GraphicsLinkItem*> vp_Links; ///< Вектор с указателями на граф. линки.
	static QVector<GraphicsPortItem*> vp_Ports; ///< Вектор с указателями на граф. порты.
	static QVector<PSchPseudonym> vp_PSchPseudonyms; ///< Вектор с псевдонимами.
	static QVector<GraphicsElementItem*> vp_LonelyElements; ///< Вектор с указателями на граф. элементы, ожидающие группу.
	static QVector<GraphicsGroupItem*> vp_LonelyGroups; ///< Вектор с указателями на граф. группы, ожидающие группу.
	static SafeMenu* p_SafeMenu; ///< Указатель на меню для общего пользования граф. классами.
	static bool bCleaningSceneNow; ///< Сцена заблокирована.

public slots:
	/// Обновление сцены.
	static void UpdateScene();
	/// Очистка сцены.
	static void ClearScene();

private:
	LOGDECL
	LOGDECL_PTHRD_INCLASS_ADD
	static Ui::SchematicWindow *p_ui; ///< Указатель на UI.
	static const char* cp_chUISettingsName; ///< Указатель на имя файла с установками UI.
	static QSettings* p_UISettings; ///< Указатель на строку установок UI.
	static bool bRefClose; ///< Признак закрытия окна снаружи.
	static GraphicsElementItem* p_GraphicsElementItem; ///< Указатель на графический элемент для передачи в виджет.
};

#endif // SCHEMATICWINDOW_H
