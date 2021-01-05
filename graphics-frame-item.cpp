//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-frame-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения фрейма.
// Конструктор.
GraphicsFrameItem::GraphicsFrameItem(unsigned short ushKindOfItem,
									 GraphicsElementItem* p_ElementParent, GraphicsGroupItem* p_GroupParent, GraphicsPortItem* p_PortParent)
{
	SchematicView::FrameConstructorHandler(this, ushKindOfItem, p_ElementParent, p_GroupParent, p_PortParent);
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsFrameItem::boundingRect() const
{
	return SchematicView::FrameBoundingHandler(this);
}

// Переопределение функции рисования фрейма.
void GraphicsFrameItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	SchematicView::FramePaintHandler(this, p_Painter);
}

