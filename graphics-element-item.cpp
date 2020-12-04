//== ВКЛЮЧЕНИЯ.
#include <QApplication>
#include <QGraphicsSceneEvent>
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
	if(p_QGroupBox != nullptr) delete p_QGroupBox;
	delete p_GraphicsFrameItem;
	delete p_GraphicsScalerItem;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsElementItem::boundingRect() const
{
	if(oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		return QRectF(0, 0,
					  oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
					  oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW);
	}
	return QRectF(0, 0,
				  oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
				  oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH);
}

// Переопределение функции рисования элемента.
void GraphicsElementItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	SchematicView::ElementPaintHandler(this, p_Painter);
}

// Переопределение функции шага событий элемента.
void GraphicsElementItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
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
