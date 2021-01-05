//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-background-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения скалера.
// Конструктор.
GraphicsBackgroundItem::GraphicsBackgroundItem()
{

}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsBackgroundItem::boundingRect() const
{
	return QRectF(0, 0, 0, 0);
}

// Переопределение функции рисования скалера.
void GraphicsBackgroundItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	p_Painter = p_Painter;
	//
}
