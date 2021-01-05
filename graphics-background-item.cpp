//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-background-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения скалера.
// Конструктор.
GraphicsBackgroundItem::GraphicsBackgroundItem()
{
	SchematicView::BackgroundConstructorHandler(this);
}

// Деструктор.
GraphicsBackgroundItem::~GraphicsBackgroundItem()
{
	SchematicView::BackgroundDestructorHandler();
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsBackgroundItem::boundingRect() const
{
	return QRectF(-20, -20, 40, 40);
}

// Переопределение функции рисования скалера.
void GraphicsBackgroundItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	SchematicView::BackgroundPaintHandler(p_Painter);
}
