//== ВКЛЮЧЕНИЯ.
#include <QApplication>
#include <QGraphicsSceneEvent>
#include <QFontMetrics>
#include "main-window.h"
#include "graphics-port-item.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс графического отображения порта.
// Конструктор.
GraphicsPortItem::GraphicsPortItem(GraphicsLinkItem* p_GraphicsLinkItem, bool bSrc, GraphicsElementItem* p_Parent)
{
	SchematicView::PortConstructorHandler(this, p_GraphicsLinkItem, bSrc, p_Parent);
}

// Деструктор.
GraphicsPortItem::~GraphicsPortItem()
{
	delete p_GraphicsFrameItem;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsPortItem::boundingRect() const
{
	return QRectF(PORT_SHAPE);
}

// Переопределение функции рисования порта.
void GraphicsPortItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	SchematicView::PortPaintHandler(this, p_Painter);
}

// Переопределение функции шага событий порта.
void GraphicsPortItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
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
