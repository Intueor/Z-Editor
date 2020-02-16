#ifndef SCHEMATICVIEW_H
#define SCHEMATICVIEW_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsView>
#include <QWheelEvent>

//== ОПРЕДЕЛЕНИЯ ТИПОВ.
typedef void (*CBSchematicViewFrameChanged)(QRectF oQRectFVisibleFrame);

//== ПРЕД-ДЕКЛАРАЦИИ.
class GraphicsElementItem;
class GraphicsGroupItem;

//== КЛАССЫ.
/// Класс виджета обзора.
class SchematicView : public QGraphicsView
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit SchematicView(QWidget* parent = nullptr);
							///< \param[in] parent Указатель на родительский виджет.
	/// Определение области видимости.
	QRectF GetVisibleRect();
							///< \return Структура определения прямоугольника области видимости.
	/// Установка указателя кэлбэка изменения окна обзора.
	static void SetSchematicViewFrameChangedCB(CBSchematicViewFrameChanged pf_CBSchematicViewFrameChanged);
								///< \param[in] pf_CBSchematicViewFrameChanged Указатель на пользовательскую функцию.
	/// Подготовка отсылки параметров и удаление группы.
	static void DeleteGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
	/// Подготовка отсылки параметров и удаление элемента (а так же добавка его группы в лист).
	static void DeleteElementAPFS(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	/// Остоединение выбранного.
	static bool DetachSelected();
							///< \return true, если добавлялись пакеты в буфер.
	/// Удаление выбранного и подготовка отправки по запросу.
	static void DeleteSelectedAPFS();
	/// Создание нового элемента и подготовка отсылки параметров.
	static GraphicsElementItem* CreateNewElementAPFS(QString& a_strNameBase, QPointF a_pntMapped, unsigned long long ullIDGroup = 0);
							///< \param[in] a_strNameBase Ссылка на строку с базой нового имени.
							///< \param[in] a_pntMapped Ссылка на координаты.
							///< \param[in] ullIDGroup ИД группы или ноль для отдельного элемента.
							///< \return Указатель на новый графический элемент.
	/// Обновление Z-позиции линков.
	static void UpdateLinksZPos();

protected:
	/// Переопределение функции обработки событий колёсика.
	void wheelEvent(QWheelEvent* p_Event);
							///< \param[in] p_Event Указатель на событие колёсика.
	/// Переопределение функции обработки нажатия на кнопку мыши.
	void mousePressEvent(QMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие колёсика.
	/// Переопределение функции обработки отпускания кнопки мыши.
	void mouseReleaseEvent(QMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие колёсика.
	/// Переопределение функции обработки перетаскивания вида.
	void scrollContentsBy(int iX, int iY);
							///< \param[in] iX Координаты X.
							///< \param[in] iY Координаты Y.
	/// Переопределение функции обработки нажатия на клавиши.
	void keyPressEvent(QKeyEvent* event);
							///< \param[in] p_Event Указатель на событие клавиши.

private:
	/// Подготовка удаления графического элемента из сцены и группы, возврат флага на удаление группы элемента.
	static bool PrepareForRemoveElementFromScene(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \return true, если был удалён пустой графический объект группы.
	/// Удаление графического элемента из группы и отправка её фрейма только на сервер.
	static bool PrepareChangingFrameAndRemoveElementFromGroup(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \return true, если группа осталась пуста.

private:
	static bool bLMousePressed; ///< Признак нажатия на ЛКМ.
	static int iXInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по X.
	static int iYInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по Y.
	static CBSchematicViewFrameChanged pf_CBSchematicViewFrameChangedInt; ///< Указатель на кэлбэк изменения окна обзора.
	qreal rScaleFactor; ///<
	qreal rFactor; ///<
};

#endif // SCHEMATICVIEW_H
