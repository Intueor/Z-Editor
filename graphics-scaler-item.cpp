//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-scaler-item.h"
#include <QGraphicsSceneEvent>

//== МАКРОСЫ.
#define LOG_NAME                                "Scaler"
#define LOG_DIR_PATH							"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS_MULTIOBJECT(GraphicsScalerItem)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(GraphicsScalerItem)

//== ФУНКЦИИ КЛАССОВ.
//== Класс отображения скалера.
// Конструктор.
GraphicsScalerItem::GraphicsScalerItem(GraphicsElementItem* p_Parent)
{
	LOG_CTRL_INIT_MULTIOBJECT;
	//
	p_ParentInt = p_Parent;
	setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_SCALER);
	setFlag(ItemIsMovable);
	setAcceptHoverEvents(true);
	setCursor(Qt::CursorShape::SizeFDiagCursor);
	setParentItem(p_Parent);
	p_SchElementGraph = &p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsScalerItem::boundingRect() const
{
	return QRectF(-SCALER_DIM, -SCALER_DIM, SCALER_DIM, SCALER_DIM);
}

// Переопределение функции рисования скалера.
void GraphicsScalerItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	if(p_ParentInt->bIsPositivePalette)
	{
		p_Painter->setPen(SchematicWindow::oQPenWhite);
	}
	else
	{
		p_Painter->setPen(SchematicWindow::oQPenBlack);
	}
	p_Painter->setBrush(p_ParentInt->oQBrush);
	p_Painter->drawPolygon(SchematicWindow::oPolygonForScaler);
}

// Переопределение функции шага событий скалера.
void GraphicsScalerItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}

// Переопределение функции обработки нажатия мыши.
void GraphicsScalerItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	if(p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(p_ParentInt->p_GraphicsGroupItemRel != nullptr)
		{
			p_ParentInt->p_GraphicsGroupItemRel->GroupToTopAPFS(p_ParentInt->p_GraphicsGroupItemRel, true, p_ParentInt);
		}
		GraphicsElementItem::ElementToTopAPFS(p_ParentInt);
		TrySendBufferToServer;
	}
	QGraphicsItem::mousePressEvent(p_Event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsScalerItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	DbPoint oDbPointPos;
	//
	if(p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	oDbPointPos.dbX = this->pos().x();
	oDbPointPos.dbY = this->pos().y();
	QGraphicsItem::mouseMoveEvent(p_Event); // Даём мышке уйти.
	setPos(QPointF((int)pos().x(), (int)pos().y()));
	if(p_Event->scenePos().x() < p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX +
			p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX + ELEMENT_MIN_X)
	{
		setX(oDbPointPos.dbX);
	}
	if(p_Event->scenePos().y() < p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY +
			p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY + ELEMENT_MIN_Y)
	{
		setY(oDbPointPos.dbY);
	}
	oDbPointPos.dbX = this->pos().x() - oDbPointPos.dbX;
	oDbPointPos.dbY = this->pos().y() - oDbPointPos.dbY;
	p_ParentInt->oDbPointDimIncrements = oDbPointPos;
	p_ParentInt->UpdateSelected(p_ParentInt, SCH_UPDATE_ELEMENT_FRAME | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
	if(p_ParentInt->p_GraphicsGroupItemRel != nullptr) // По группе элемента без выборки, если она есть.
	{
		if(!p_ParentInt->p_GraphicsGroupItemRel->vp_ConnectedElements.isEmpty())
		{
			GraphicsElementItem::UpdateGroupFrameByElements(p_ParentInt->p_GraphicsGroupItemRel);
		}
	}
}

// Переопределение функции обработки отпускания мыши.
void GraphicsScalerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	if(p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		// Ищем линки к элементу-родителю скалера...
		for(int iF = 0; iF < SchematicWindow::vp_Links.count(); iF++)
		{
			GraphicsLinkItem* p_GraphicsLinkItem;
			//
			p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iF);
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc == p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_SCR_PORT_POS;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_LINK_VARS, (char*)&p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars, sizeof(PSchLinkVars));
			}
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst == p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_DST_PORT_POS;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_LINK_VARS, (char*)&p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars, sizeof(PSchLinkVars));
			}
		}
		if(p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
		{
			if(p_ParentInt->p_GraphicsGroupItemRel != nullptr)
			{
				p_ParentInt->p_GraphicsGroupItemRel->ReleaseGroupAPFS(p_ParentInt->p_GraphicsGroupItemRel, p_ParentInt, WITH_FRAME,
																	  WITHOUT_ELEMENTS_POSITION);
			}
		}
		GraphicsElementItem::ReleaseElementAPFS(p_ParentInt, WITHOUT_GROUP, WITHOUT_POSITION, WITH_FRAME);
		TrySendBufferToServer;
	}
	QGraphicsItem::mouseReleaseEvent(p_Event);
}
