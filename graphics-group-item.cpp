//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-group-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс графической группы.
// Конструктор.
GraphicsGroupItem::GraphicsGroupItem(PSchGroupBase* p_PSchGroupBase)
{
	SchematicView::GroupConstructorHandler(this, p_PSchGroupBase);
}

// Деструктор.
GraphicsGroupItem::~GraphicsGroupItem()
{
	delete p_GraphicsFrameItem;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsGroupItem::boundingRect() const
{
	return SchematicView::GroupBoundingHandler(this);
}

// Переопределение функции рисования группы.
void GraphicsGroupItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	SchematicView::GroupPaintHandler(this, p_Painter);
}

// Переопределение функции обработки нажатия мыши.
void GraphicsGroupItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::GroupMousePressEventHandler(this, p_Event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsGroupItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::GroupMouseMoveEventHandler(this, p_Event);
}

// Переопределение функции обработки отпускания мыши.
void GraphicsGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::GroupMouseReleaseEventHandler(this, p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsGroupItem::OBMousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mousePressEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsGroupItem::OBMouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseMoveEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsGroupItem::OBMouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseReleaseEvent(p_Event);
}
