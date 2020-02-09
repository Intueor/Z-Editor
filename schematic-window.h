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
	/// Получение указателя на окно обзора.
	static SchematicView* GetSchematicView();
							///< \return Указатель на окно обзора.
	/// Установка временного стиля кистей общего пользования.
	static void SetTempBrushesStyle(Qt::BrushStyle iStyle);
							///< \param[in] iStyle Стиль.
	/// Отмена временного стиля кистей общего пользования.
	static void RestoreBrushesStyles();

public:
	static MainWindow* p_MainWindow; ///< Указатель на класс главного окна.
	QGraphicsScene oScene; ///< Объект сцены.
	static QGraphicsScene* p_QGraphicsScene; ///< Статический указатель на используемый нестатический объект сцены.
	static QPolygon oPolygonForScaler; ///< Объект полигона для отрисовки скалера.
	static QBrush oQBrushDark; ///< Чёрная кисть общего пользования.
	static QBrush oQBrushLight; ///< Белая кисть общего пользования.
	static QPen oQPenWhite; ///< Белый карандаш общего пользования.
	static QPen oQPenBlack; ///< Чёрный карандаш общего пользования.
	static QPen oQPenWhiteTransparent; ///< Белый полупрозрачный карандаш общего пользования.
	static QPen oQPenBlackTransparent; ///< Чёрный полупрозрачный карандаш общего пользования.
	static QPen oQPenElementFrameFlash; ///< Карандаш мерцания рамки элемента общего пользования.
	static QPen oQPenGroupFrameFlash; ///< Карандаш мерцания рамки группы общего пользования.
	static QVector<GraphicsElementItem*> vp_SelectedElements; ///< Вектор с указателями на выбранные граф. элементы.
	static QVector<GraphicsElementItem*> vp_SelectedFreeElements; ///< Вектор с указателями на выбранные свободные граф. элементы (врем.).
	static QVector<GraphicsGroupItem*> vp_SelectedGroups; ///< Вектор с указателями на выбранные граф. группы.
	//
	static QVector<GraphicsElementItem*> vp_Elements; ///< Вектор с указателями на граф. элементы.
	static QVector<GraphicsGroupItem*> vp_Groups; ///< Вектор с указателями на граф. группы.
	static QVector<GraphicsLinkItem*> vp_Links; ///< Вектор с указателями на граф. линки.
	static QVector<GraphicsPortItem*> vp_Ports; ///< Вектор с указателями на граф. порты.
	//
	static QVector<GraphicsElementItem*> vp_LonelyElements; ///< Вектор с указателями на граф. элементы, ожидающие группу.
	//
	static unsigned char uchElementSelectionFlashCounter; ///< Счётчик таймера мерцания выбранных элементов.
	static unsigned char uchGroupSelectionFlashCounter; ///< Счётчик таймера мерцания выбранных групп.
	static qreal dbObjectZPos; ///< Крайняя Z-позиция.
	static QMenu* p_Menu; ///< Указатель на меню для общего пользования граф. классами.

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
	static GraphicsElementItem* p_GraphicsElementItem; ///< Указатель на графический элемент для передачи в виджет.
	static Qt::BrushStyle iLStyle, iDStyle; ///< Внутренние переменные для хранения основных стилей кистей при замене на временные.
};

#endif // SCHEMATICWINDOW_H
