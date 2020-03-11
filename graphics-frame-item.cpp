//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-frame-item.h"
#include "z-editor-defs.h"

//== МАКРОСЫ.
#define LOG_NAME                                "Frame"
#define LOG_DIR_PATH							"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS_MULTIOBJECT(GraphicsFrameItem)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(GraphicsFrameItem)

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения фрейма.
// Конструктор.
GraphicsFrameItem::GraphicsFrameItem(unsigned short ushKindOfItem,
									 GraphicsElementItem* p_ElementParent, GraphicsGroupItem* p_GroupParent, GraphicsPortItem* p_PortParent)
{
	LOG_CTRL_INIT_MULTIOBJECT;
	//
	p_ElementParentInt = p_ElementParent;
	p_GroupParentInt = p_GroupParent;
	ushKindOfItemInt = ushKindOfItem;
	setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_FRAME);
	if(ushKindOfItem == SCH_KIND_ITEM_ELEMENT)
	{
		setParentItem(p_ElementParentInt);
	}
	else if(ushKindOfItem == SCH_KIND_ITEM_GROUP)
	{
		setParentItem(p_GroupParentInt);
	}
	else if(ushKindOfItem == SCH_KIND_ITEM_PORT)
	{
		setParentItem(p_PortParent);
	}
	setZValue(OVERMIN_NUMBER);
	setAcceptedMouseButtons(0);
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsFrameItem::boundingRect() const
{
	if(ushKindOfItemInt == SCH_KIND_ITEM_ELEMENT)
	{
		return QRectF(-3,
					  -3,
					  p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW + 6,
					  p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH + 6);
	}
	else if(ushKindOfItemInt == SCH_KIND_ITEM_GROUP)
	{
		return QRectF(-3,
					  -3,
					  p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW + 6,
					  p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH + 6);
	}
	else if(ushKindOfItemInt == SCH_KIND_ITEM_PORT)
	{
		return QRectF(-5,
					  -5,
					  10,
					  10);
	}
	return QRectF(0, 0, 0, 0);
}

// Переопределение функции рисования фрейма.
void GraphicsFrameItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	p_Painter->setBrush(Qt::NoBrush);
	if(ushKindOfItemInt == SCH_KIND_ITEM_ELEMENT)
	{
		p_Painter->setPen(SchematicWindow::oQPenElementFrameFlash);
		p_Painter->drawRect(-2,
							-2,
							p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW + 4,
							p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH + 4);
	}
	else if(ushKindOfItemInt == SCH_KIND_ITEM_GROUP)
	{
		p_Painter->setPen(SchematicWindow::oQPenGroupFrameFlash);
		p_Painter->drawRect(-2,
							-2,
							p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW + 4,
							p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH + 4);
	}
	else if(ushKindOfItemInt == SCH_KIND_ITEM_PORT)
	{
		p_Painter->setPen(SchematicWindow::oQPenPortFrameFlash);
		p_Painter->drawEllipse(-4, -4, 8, 8);
	}
}

// Переопределение функции шага событий фрейма.
void GraphicsFrameItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}
