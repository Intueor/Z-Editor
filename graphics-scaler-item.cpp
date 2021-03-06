//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-scaler-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения скалера.
// Конструктор.
GraphicsScalerItem::GraphicsScalerItem(GraphicsElementItem* p_Parent)
{
	SchematicView::ScalerConstructorHandler(this, p_Parent);
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsScalerItem::boundingRect() const
{
	return SchematicView::ScalerBoundingHandler(this);
}

/// Переопределение функции определения формы.
QPainterPath GraphicsScalerItem::shape() const
{
	return SchematicView::ScalerShapeHandler(this);
}

// Переопределение функции рисования скалера.
void GraphicsScalerItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	SchematicView::ScalerPaintHandler(this, p_Painter);
}

// Переопределение функции обработки нажатия мыши.
void GraphicsScalerItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::ScalerMousePressEventHandler(this, p_Event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsScalerItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::ScalerMouseMoveEventHandler(this, p_Event);
}

// Переопределение функции обработки отпускания мыши.
void GraphicsScalerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::ScalerMouseReleaseEventHandler(this, p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsScalerItem::OBMousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mousePressEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsScalerItem::OBMouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseMoveEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsScalerItem::OBMouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseReleaseEvent(p_Event);
}
