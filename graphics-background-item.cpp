//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-background-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения скалера.
// Конструктор.
GraphicsBackgroundItem::GraphicsBackgroundItem()
{
	SchematicView::p_GraphicsBackgroundItemInt = this;
}

// Деструктор.
GraphicsBackgroundItem::~GraphicsBackgroundItem()
{
	SchematicView::p_GraphicsBackgroundItemInt = nullptr;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsBackgroundItem::boundingRect() const
{
	return SchematicView::GetVisibleRect();
}

// Переопределение функции рисования скалера.
void GraphicsBackgroundItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	if(!SchematicView::bLoading)
	{
		p_Painter->drawEllipse(0, 0, 20, 20);
	}
}
