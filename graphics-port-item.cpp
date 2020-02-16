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

//== ФУНКЦИИ КЛАССОВ.
//== Класс графического отображения порта.
// Конструктор.
GraphicsPortItem::GraphicsPortItem(GraphicsLinkItem* p_GraphicsLinkItem, bool bSrc, GraphicsElementItem* p_Parent)
{
	LOG_CTRL_INIT_MULTIOBJECT;
	//
	p_ParentInt = p_Parent;
	bIsSrc = bSrc;
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
void GraphicsPortItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	if(p_SchElementGraph->bBusy|| MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(event->button() == Qt::MouseButton::LeftButton)
	{
		if(p_ParentInt->p_GraphicsGroupItemRel != nullptr)
		{
			p_ParentInt->p_GraphicsGroupItemRel->GroupToTop(p_ParentInt->p_GraphicsGroupItemRel);
		}
		GraphicsElementItem::ElementToTop(p_ParentInt);
	}
	QGraphicsItem::mousePressEvent(event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsPortItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	DbPoint oDbPointRB;
	DbPoint oDbPointCurrent;
	DbPoint oDbPointOld;
	bool bXInside = false;
	bool bYInside = false;
	//
	if(p_SchElementGraph->bBusy|| MainWindow::bBlockingGraphics)
	{
		return;
	}
	oDbPointOld.dbX = pos().x(); // Исходный X.
	oDbPointOld.dbY = pos().y(); // Исходный Y.
	oDbPointRB.dbX = p_SchElementGraph->oDbObjectFrame.dbX + p_SchElementGraph->oDbObjectFrame.dbW; // Крайняя правая точка.
	oDbPointRB.dbY = p_SchElementGraph->oDbObjectFrame.dbY + p_SchElementGraph->oDbObjectFrame.dbH; // Крайняя нижняя точка.
	QGraphicsItem::mouseMoveEvent(event); // Даём мышке уйти.
	oDbPointCurrent.dbX = pos().x(); // Текущий X.
	oDbPointCurrent.dbY = pos().y(); // Текущий Y.
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

// Переопределение функции обработки отпускания мыши.
void GraphicsPortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if(p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(event->button() == Qt::MouseButton::LeftButton)
	{
		if(bIsSrc)
		{
			p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX = x() - p_SchElementGraph->oDbObjectFrame.dbX;
			p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY = y() - p_SchElementGraph->oDbObjectFrame.dbY;
			p_PSchLinkVarsInt->oSchLinkGraph.uchChangesBits |= SCH_LINK_BIT_SCR_PORT_POS;
		}
		else
		{
			p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX = x() - p_SchElementGraph->oDbObjectFrame.dbX;
			p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY = y() - p_SchElementGraph->oDbObjectFrame.dbY;
			p_PSchLinkVarsInt->oSchLinkGraph.uchChangesBits |= SCH_LINK_BIT_DST_PORT_POS;
		}
		MainWindow::p_Client->AddPocketToOutputBufferC(
					PROTO_O_SCH_LINK_VARS, (char*)p_PSchLinkVarsInt, sizeof(PSchLinkVars));
		p_ParentInt->SetBlockingPattern(p_ParentInt, false);
		p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uchChangesBits = SCH_ELEMENT_BIT_BUSY;
		p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy = false;
		MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_ELEMENT_VARS,
													   (char*)&p_ParentInt->oPSchElementBaseInt.oPSchElementVars,
													   sizeof(PSchElementVars));
		if(p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
		{
			if(p_ParentInt->p_GraphicsGroupItemRel != nullptr)
			{
				p_ParentInt->p_GraphicsGroupItemRel->ReleaseGroupAndPrepareForSending(p_ParentInt->p_GraphicsGroupItemRel);
			}
		}
		TrySendBufferToServer;
	}
	QGraphicsItem::mouseReleaseEvent(event);
}
