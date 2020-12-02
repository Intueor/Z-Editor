#ifndef GRAPHICSSCALERITEM_H
#define GRAPHICSSCALERITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>

//== КЛАССЫ.
/// Класс отображения скалера.
class GraphicsScalerItem : public QGraphicsItem
{
public:
	/// Конструктор.
	explicit GraphicsScalerItem(GraphicsElementItem* p_Parent);
							///< \param[in] p_Parent Указатель на родителя.
	/// Переопределение функции сообщения о вмещающем прямоугольнике.
	QRectF boundingRect() const override;
							///< \return Вмещающий прямоугольник.
	/// Переопределение функции рисования скалера.
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget) override;
							///< \param[in] p_Painter Указатель на отрисовщик.
							///< \param[in] p_Option Указатель на опции стиля.
							///< \param[in] p_Widget Указатель на виджет.
	/// Переопределение функции обработки нажатия мыши.
	void mousePressEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки перемещения мыши.
	void mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки отпускания мыши.
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Для внешнего вызова базового метода.
	void OBMousePressEvent(QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Для внешнего вызова базового метода.
	void OBMouseMoveEvent(QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Для внешнего вызова базового метода.
	void OBMouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
protected:
	/// Переопределение функции шага событий элемента.
	void advance(int iStep) override;
							///< \param[in] iStep Текущий шаг сцены.

public:
	GraphicsElementItem* p_ParentInt; ///< Внутренний указатель на родителя.
	SchEGGraph* p_SchEGGraph; ///< Указтель на граф. представление элемента.
};

#endif // GRAPHICSSCALERITEM_H
