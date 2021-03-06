//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-element-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс графического элемента.
// Конструктор.
GraphicsElementItem::GraphicsElementItem(PSchElementBase* p_PSchElementBase)
{
	SchematicView::ElementConstructorHandler(this, p_PSchElementBase);
}

// Деструктор.
GraphicsElementItem::~GraphicsElementItem()
{
	SchematicView::ElementDestructorHandler(this);
	if(p_QGroupBox != nullptr) delete p_QGroupBox;
	delete p_GraphicsFrameItem;
	delete p_GraphicsScalerItem;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsElementItem::boundingRect() const
{
	return SchematicView::ElementBoundingHandler(this);
}

/// Переопределение функции определения формы.
QPainterPath GraphicsElementItem::shape() const
{
	return SchematicView::ElementShapeHandler(this);
}

// Переопределение функции рисования элемента.
void GraphicsElementItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem*, QWidget*)
{
	SchematicView::ElementPaintHandler(this, p_Painter);
}

// Переопределение функции обработки нажатия мыши.
void GraphicsElementItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::ElementMousePressEventHandler(this, p_Event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsElementItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::ElementMouseMoveEventHandler(this, p_Event);
}

// Переопределение функции обработки отпускания мыши.
void GraphicsElementItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	SchematicView::ElementMouseReleaseEventHandler(this, p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsElementItem::OBMousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mousePressEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsElementItem::OBMouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseMoveEvent(p_Event);
}

// Для внешнего вызова базового метода.
void GraphicsElementItem::OBMouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem::mouseReleaseEvent(p_Event);
}
