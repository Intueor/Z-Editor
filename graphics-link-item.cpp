//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-link-item.h"

//== ФУНКЦИИ КЛАССОВ.
// Класс графического линка.
// Конструктор.
GraphicsLinkItem::GraphicsLinkItem(PSchLinkBase* p_PSchLinkBase)
{
	SchematicView::LinkConstructorHandler(this, p_PSchLinkBase);
}

// Деструктор.
GraphicsLinkItem::~GraphicsLinkItem()
{

}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsLinkItem::boundingRect() const
{
	return SchematicView::CalcLinkLineWidthHeight((GraphicsLinkItem*)this).oQRectF;
}

// Переопределение функции рисования линка.
void GraphicsLinkItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	SchematicView::LinkPaintHandler(this, p_Painter);
}

// Переопределение функции шага событий линка.
void GraphicsLinkItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}
