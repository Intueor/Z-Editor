//== ВКЛЮЧЕНИЯ.
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneEvent>
#include "main-window.h"
#include "graphics-group-item.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"

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
	return QRectF(0, 0, oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW,
				  oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH);
}

// Переопределение функции рисования группы.
void GraphicsGroupItem::paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	SchematicView::GroupPaintHandler(this, p_Painter);
}

// Переопределение функции шага событий группы.
void GraphicsGroupItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
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
