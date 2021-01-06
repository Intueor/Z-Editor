//== ВКЛЮЧЕНИЯ.
#include <math.h>
#include "main-window.h"
#include "graphics-background-item.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
QPen GraphicsBackgroundItem::oQPenDarkGray;

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения скалера.
// Конструктор.
GraphicsBackgroundItem::GraphicsBackgroundItem()
{
	oQPenDarkGray.setColor(QColor(80, 80, 80, 255));
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
	return QRectF(0, 0, oQRectFVisible.width() + (SchematicView::dbSnapStep * 2.0f), oQRectFVisible.height() + (SchematicView::dbSnapStep *2.0f));
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
		p_Painter->setPen(oQPenDarkGray);
		for(double dbXS = 0.0f; dbXS < boundingRect().width(); dbXS += SchematicView::dbSnapStep)
		{
			p_Painter->drawLine(dbXS, 0, dbXS, boundingRect().height());
		}
		for(double dbYS = 0.0f; dbYS < boundingRect().height(); dbYS += SchematicView::dbSnapStep)
		{
			p_Painter->drawLine(0, dbYS, boundingRect().width(), dbYS);
		}
	}
}
