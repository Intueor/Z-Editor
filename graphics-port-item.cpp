//== ВКЛЮЧЕНИЯ.
#include <QGraphicsSceneEvent>
#include "main-window.h"
#include "graphics-port-item.h"

//== МАКРОСЫ.
#define LOG_NAME                                "Port"
#define LOG_DIR_PATH							"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS_MULTIOBJECT(GraphicsPortItem)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(GraphicsPortItem)
bool GraphicsPortItem::bAltPressed;

//== ФУНКЦИИ КЛАССОВ.
//== Класс графического отображения порта.
// Конструктор.
GraphicsPortItem::GraphicsPortItem(GraphicsLinkItem* p_GraphicsLinkItem, bool bSrc, GraphicsElementItem* p_Parent)
{
	LOG_CTRL_INIT_MULTIOBJECT;
	//
	p_ParentInt = p_Parent;
	bIsSrc = bSrc;
	bAltPressed = false;
	p_PSchLinkVarsInt = &p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars;
	p_GraphicsLinkItemInt = p_GraphicsLinkItem;
	setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_PORT);
	setFlag(ItemIsMovable);
	setAcceptHoverEvents(true);
	setCursor(Qt::CursorShape::PointingHandCursor);
	setParentItem(p_Parent);
	p_SchElementGraph = &p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph;
	if(bIsSrc)
	{
		setPos(p_SchElementGraph->oDbObjectFrame.dbX + p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX,
			   p_SchElementGraph->oDbObjectFrame.dbY + p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY);
		uiPortInt = p_PSchLinkVarsInt->ushiSrcPort;
		p_GraphicsLinkItemInt->p_GraphicsPortItemSrc = this;
	}
	else
	{
		setPos(p_SchElementGraph->oDbObjectFrame.dbX + p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX,
			   p_SchElementGraph->oDbObjectFrame.dbY + p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY);
		uiPortInt = p_PSchLinkVarsInt->ushiSrcPort;
		p_GraphicsLinkItemInt->p_GraphicsPortItemDst = this;
	}
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsPortItem::boundingRect() const
{
	return QRectF(PORT_SHAPE);
}

// Переопределение функции рисования порта.
void GraphicsPortItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	SchematicWindow::SetTempBrushesStyle(p_ParentInt->oQBrush.style());
	if(bIsSrc)
	{
		p_Painter->setBrush(SchematicWindow::oQBrushLight);
	}
	else
	{
		p_Painter->setBrush(SchematicWindow::oQBrushDark);
	}
	p_Painter->setPen(SchematicWindow::oQPenWhite);
	p_Painter->drawEllipse(PORT_SHAPE);
	SchematicWindow::RestoreBrushesStyles();
}

// Переопределение функции шага событий порта.
void GraphicsPortItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}

// Переопределение функции обработки нажатия мыши.
void GraphicsPortItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	if(p_SchElementGraph->bBusy|| MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(p_ParentInt->p_GraphicsGroupItemRel != nullptr)
		{
			p_ParentInt->p_GraphicsGroupItemRel->GroupToTopAPFS(p_ParentInt->p_GraphicsGroupItemRel, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
																ADD_SEND_BUSY, DONT_ADD_SEND_FRAME, p_ParentInt, APPLY_BLOCKINGPATTERN, SEND_ELEMENTS);
		}
		GraphicsElementItem::ElementToTopAPFS(p_ParentInt, DONT_SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_BUSY, APPLY_BLOCKINGPATTERN, SEND_ELEMENT);
		TrySendBufferToServer;
		if(p_Event->modifiers() == Qt::AltModifier)
		{
			oDbPointOld.dbX = pos().x(); // Исходный X.
			oDbPointOld.dbY = pos().y(); // Исходный Y.
			bAltPressed = true;
		}
		else
		{
			bAltPressed = false;
		}
	}
	QGraphicsItem::mousePressEvent(p_Event);
}

// Коррекция точки порта по краю элемента.
void GraphicsPortItem::BindToEdge()
{
	bool bXInside = false;
	bool bYInside = false;
	if(oDbPointCurrent.dbX <= p_SchElementGraph->oDbObjectFrame.dbX) // Если текущий X меньше левого края элемента...
	{
		oDbPointCurrent.dbX = p_SchElementGraph->oDbObjectFrame.dbX; // Установка на левый край.
		goto gY;
	}
	if(oDbPointCurrent.dbX >= oDbPointRB.dbX) // Если текущий X больше правого края элемента...
	{
		oDbPointCurrent.dbX = oDbPointRB.dbX; // Установка на правый край.
	}
	else
	{
		bXInside = true; // Признак нахождения в диапазоне элемента по X.
	}
gY: if(oDbPointCurrent.dbY <= p_SchElementGraph->oDbObjectFrame.dbY)
	{
		oDbPointCurrent.dbY = p_SchElementGraph->oDbObjectFrame.dbY;
		goto gI;
	}
	if(oDbPointCurrent.dbY >= oDbPointRB.dbY)
	{
		oDbPointCurrent.dbY = oDbPointRB.dbY;
	}
	else
	{
		bYInside = true; // Признак нахождения в диапазоне элемента по Y.
	}
gI: if(bXInside && bYInside)
	{
		// Если прошлый X был на краю...
		if((oDbPointOld.dbX == p_SchElementGraph->oDbObjectFrame.dbX) || (oDbPointOld.dbX == oDbPointRB.dbX))
		{
			oDbPointCurrent.dbX = oDbPointOld.dbX;
		}
		if((oDbPointOld.dbY == p_SchElementGraph->oDbObjectFrame.dbY) || (oDbPointOld.dbY == oDbPointRB.dbY))
		{
			oDbPointCurrent.dbY = oDbPointOld.dbY;
		}
	}
}

// Установка порта в позицию.
void GraphicsPortItem::SetToPos()
{
	setPos(oDbPointCurrent.dbX, oDbPointCurrent.dbY);
	if(bIsSrc)
	{
		p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX = x() - p_SchElementGraph->oDbObjectFrame.dbX;
		p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY = y() - p_SchElementGraph->oDbObjectFrame.dbY;
	}
	else
	{
		p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX = x() - p_SchElementGraph->oDbObjectFrame.dbX;
		p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY = y() - p_SchElementGraph->oDbObjectFrame.dbY;
	}
	p_ParentInt->UpdateSelected(p_ParentInt, SCH_UPDATE_LINK_POS | SCH_UPDATE_MAIN, nullptr, p_GraphicsLinkItemInt);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	//
	if(p_SchElementGraph->bBusy|| MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(!bAltPressed)
	{
		oDbPointOld.dbX = pos().x(); // Исходный X.
		oDbPointOld.dbY = pos().y(); // Исходный Y.
		oDbPointRB.dbX = p_SchElementGraph->oDbObjectFrame.dbX + p_SchElementGraph->oDbObjectFrame.dbW; // Крайняя правая точка.
		oDbPointRB.dbY = p_SchElementGraph->oDbObjectFrame.dbY + p_SchElementGraph->oDbObjectFrame.dbH; // Крайняя нижняя точка.
	}
	QGraphicsItem::mouseMoveEvent(p_Event); // Даём мышке уйти.
	oDbPointCurrent.dbX = pos().x(); // Текущий X.
	oDbPointCurrent.dbY = pos().y(); // Текущий Y.
	if(!bAltPressed)
	{
		BindToEdge();
	}
	SetToPos();
}

// Переопределение функции обработки отпускания мыши.
void GraphicsPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem* p_QGraphicsItem;
	QGraphicsItem* p_QGraphicsItemFounded = nullptr;
	GraphicsElementItem* p_GraphicsElementItemToChange = nullptr;
	double dbZ = -999999;
	DbPoint oDbMapped;
	//
	if(p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(bAltPressed)
	{
		QList<QGraphicsItem*> lp_QGraphicsItems = MainWindow::p_SchematicWindow->oScene.items();
		QVector<QGraphicsItem*> vp_QGraphicsItemsOnPosition;
		//
		oDbMapped.dbX = p_Event->scenePos().x();
		oDbMapped.dbY = p_Event->scenePos().y();
		for(int iF = 0; iF != lp_QGraphicsItems.count(); iF++)
		{
			p_QGraphicsItem = lp_QGraphicsItems.at(iF);
			if(p_QGraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI)
			{
				if(p_QGraphicsItem->data(SCH_KIND_OF_ITEM) != SCH_KIND_ITEM_PORT)
				{
					double dbX, dbY, dbXR, dbYD;
					dbX = p_QGraphicsItem->pos().x() + p_QGraphicsItem->boundingRect().topLeft().x();
					dbY = p_QGraphicsItem->pos().y() + p_QGraphicsItem->boundingRect().topLeft().y();
					dbXR = dbX + p_QGraphicsItem->boundingRect().bottomRight().x();
					dbYD = dbY + p_QGraphicsItem->boundingRect().bottomRight().y();
					if(((oDbMapped.dbX > dbX) & (oDbMapped.dbX < dbXR)) & ((oDbMapped.dbY > dbY) & (oDbMapped.dbY < dbYD)))
					{
						vp_QGraphicsItemsOnPosition.append(p_QGraphicsItem);
					}
				}
			}
		}
		for(int iF = 0; iF != vp_QGraphicsItemsOnPosition.count(); iF++)
		{
			double dbItemZ;
			//
			p_QGraphicsItem = vp_QGraphicsItemsOnPosition.at(iF);
			dbItemZ = p_QGraphicsItem->zValue();
			if(dbItemZ > dbZ)
			{
				dbZ = dbItemZ;
				p_QGraphicsItemFounded = p_QGraphicsItem;
			}
		}
		if(p_QGraphicsItemFounded)
		{
			if(p_QGraphicsItemFounded->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_ELEMENT)
			{
				p_GraphicsElementItemToChange = (GraphicsElementItem*) p_QGraphicsItemFounded;
				//
				char* p_chType;
				//
				if(bIsSrc)
				{
					p_chType = (char*)m_chLogSource;
				}
				else
				{
					p_chType = (char*)m_chLogDestination;
				}
				LOG_P_2(LOG_CAT_I, "Move" << p_chType << "port from element [" << p_ParentInt->oPSchElementBaseInt.m_chName << "]" <<
						" to element [" << p_GraphicsElementItemToChange->oPSchElementBaseInt.m_chName << "]");
				goto gF;
			}
		}
		BindToEdge();
		SetToPos();
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(bIsSrc)
		{
			p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX = x() - p_SchElementGraph->oDbObjectFrame.dbX;
			p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY = y() - p_SchElementGraph->oDbObjectFrame.dbY;
			p_PSchLinkVarsInt->oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_SCR_PORT_POS;
		}
		else
		{
			p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX = x() - p_SchElementGraph->oDbObjectFrame.dbX;
			p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY = y() - p_SchElementGraph->oDbObjectFrame.dbY;
			p_PSchLinkVarsInt->oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_DST_PORT_POS;
		}
		MainWindow::p_Client->AddPocketToOutputBufferC(
					PROTO_O_SCH_LINK_VARS, (char*)p_PSchLinkVarsInt, sizeof(PSchLinkVars));
gF:		if(p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
		{
			if(p_ParentInt->p_GraphicsGroupItemRel != nullptr)
			{
				p_ParentInt->p_GraphicsGroupItemRel->ReleaseGroupAPFS(p_ParentInt->p_GraphicsGroupItemRel, p_ParentInt, WITHOUT_FRAME,
																	  WITHOUT_ELEMENTS_POSITION);
			}
		}
		GraphicsElementItem::ReleaseElementAPFS(p_ParentInt, WITHOUT_GROUP, WITHOUT_POSITION);
		TrySendBufferToServer;
	}
	QGraphicsItem::mouseReleaseEvent(p_Event);
	if(p_GraphicsElementItemToChange)
	{

		SchematicView::ReplaceLink(p_GraphicsLinkItemInt, p_GraphicsElementItemToChange, bIsSrc, oDbMapped);
	}
}
