//== ВКЛЮЧЕНИЯ.
#include <math.h>
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
	QRectF oQRectFVisible = SchematicView::GetVisibleRect();
	//
	return QRectF(0, 0, oQRectFVisible.width() + 20.0f, oQRectFVisible.height() + 20.0f);
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
		for(double dbXS = 0.0f; dbXS < boundingRect().width(); dbXS += 10.0f)
		{
			p_Painter->drawLine(dbXS, 0, dbXS, boundingRect().height());
		}
		for(double dbYS = 0.0f; dbYS < boundingRect().height(); dbYS += 10.0f)
		{
			p_Painter->drawLine(0, dbYS, boundingRect().width(), dbYS);
		}
	}
}
