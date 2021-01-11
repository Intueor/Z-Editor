//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-port-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс графического отображения порта.
// Конструктор.
GraphicsPortItem::GraphicsPortItem(GraphicsLinkItem* p_GraphicsLinkItem, bool bSrc, GraphicsElementItem* p_Parent)
{
	SchematicView::PortConstructorHandler(this, p_GraphicsLinkItem, bSrc, p_Parent);
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsPortItem::boundingRect() const
{
	return SchematicView::PortBoundingHandler();
}

// Переопределение функции рисования порта.
void GraphicsPortItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	SchematicView::PortPaintHandler(this, p_Painter);
}

// Переопределение функции обработки нажатия мыши.
void GraphicsPortItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::PortMousePressEventHandler(this, p_Event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::PortMouseMoveEventHandler(this, p_Event);
}

// Переопределение функции обработки отпускания мыши.
void GraphicsPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::PortMouseReleaseEventHandler(this, p_Event);
}

// Переопределение функции обработки нахождения курсора над портом.
void GraphicsPortItem::hoverEnterEvent(QGraphicsSceneHoverEvent* p_Event)
{
	SchematicView::PortHoverEnterEventHandler(this);
	QGraphicsItem::hoverEnterEvent(p_Event);
}
// Переопределение функции обработки ухода курсора с порта.
void GraphicsPortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* p_Event)
{
	SchematicView::PortHoverLeaveEventHandler(this);
	QGraphicsItem::hoverLeaveEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsPortItem::OBMousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mousePressEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsPortItem::OBMouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseMoveEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsPortItem::OBMouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseReleaseEvent(p_Event);
}
