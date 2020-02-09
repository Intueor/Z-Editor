//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "graphics-link-item.h"

//== МАКРОСЫ.
#define LOG_NAME                                "Link"
#define LOG_DIR_PATH							"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS_MULTIOBJECT(GraphicsLinkItem)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(GraphicsLinkItem)

//== ФУНКЦИИ КЛАССОВ.
// Класс графического линка.
// Конструктор.
GraphicsLinkItem::GraphicsLinkItem(PSchLinkBase* p_PSchLinkBase)
{
	LOG_CTRL_INIT_MULTIOBJECT;
	//
	setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_LINK);
	memcpy(&oPSchLinkBaseInt, p_PSchLinkBase, sizeof(PSchLinkBase));
	p_GraphicsElementItemSrc = nullptr;
	p_GraphicsElementItemDst = nullptr;
	// Поиск присоединённых элементов.
	for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++)
	{
		GraphicsElementItem* p_GraphicsElementItem;
		//
		p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
		if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt == oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc) &&
		   (p_GraphicsElementItemSrc == nullptr))
		{
			p_GraphicsElementItemSrc = p_GraphicsElementItem;
		}
		if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt == oPSchLinkBaseInt.oPSchLinkVars.ullIDDst) &&
		   (p_GraphicsElementItemDst == nullptr))
		{
			p_GraphicsElementItemDst = p_GraphicsElementItem;
		}
		if((p_GraphicsElementItemSrc != nullptr) && (p_GraphicsElementItemDst != nullptr))
		{
			p_GraphicsElementItemSrc->AddPort(this, true, p_GraphicsElementItemSrc);
			p_GraphicsElementItemDst->AddPort(this, false, p_GraphicsElementItemDst);
			p_PSchElementVarsSrc = &p_GraphicsElementItemSrc->oPSchElementBaseInt.oPSchElementVars;
			p_PSchElementVarsDst = &p_GraphicsElementItemDst->oPSchElementBaseInt.oPSchElementVars;
			UpdatePosition(this);
			return;
		}
	}
	p_PSchLinkBase->oPSchLinkVars.oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_INIT_ERROR; // Отметка про сбой в поиске элементов.
}

// Деструктор.
GraphicsLinkItem::~GraphicsLinkItem()
{

}

// Входное обновление параметров линка по структуре.
void GraphicsLinkItem::IncomingUpdateLinkParameters(GraphicsLinkItem* p_GraphicsLinkItem, PSchLinkVars& a_SchLinkVars)
{
	// Позиция порта источника.
	if(a_SchLinkVars.oSchLinkGraph.uchChangesBits & SCH_LINK_BIT_SCR_PORT_POS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName).toStdString()
				<< "<>" << QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName).toStdString()
				<< "] scr port pos.");
		p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos =
				a_SchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos;
		p_GraphicsLinkItem->p_GraphicsElementItemSrc->UpdateSelected(p_GraphicsLinkItem->p_GraphicsElementItemSrc,
												 SCH_UPDATE_PORT_SRC_POS | SCH_UPDATE_LINK_POS | SCH_UPDATE_MAIN,
												 p_GraphicsLinkItem->p_GraphicsPortItemSrc, p_GraphicsLinkItem);
	}
	// Позиция порта приёмника.
	if(a_SchLinkVars.oSchLinkGraph.uchChangesBits & SCH_LINK_BIT_DST_PORT_POS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName).toStdString()
				<< "<>" << QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName).toStdString()
				<< "] dst port pos.");
		p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos =
				a_SchLinkVars.oSchLinkGraph.oDbDstPortGraphPos;
		p_GraphicsLinkItem->p_GraphicsElementItemDst->UpdateSelected(p_GraphicsLinkItem->p_GraphicsElementItemDst,
												 SCH_UPDATE_PORT_DST_POS | SCH_UPDATE_LINK_POS | SCH_UPDATE_MAIN,
												 p_GraphicsLinkItem->p_GraphicsPortItemDst, p_GraphicsLinkItem);
	}
}

// Вычисление точек портов.
GraphicsLinkItem::DbPointPair GraphicsLinkItem::CalcPortsCoords() const
{
	DbPointPair oDbPointPairResult;
	// Позиция элемента плюс фрейм.
	oDbPointPairResult.dbSrc.dbX = p_PSchElementVarsSrc->oSchElementGraph.oDbObjectPos.dbX +
			p_PSchElementVarsSrc->oSchElementGraph.oDbObjectFrame.dbX;
	oDbPointPairResult.dbSrc.dbY = p_PSchElementVarsSrc->oSchElementGraph.oDbObjectPos.dbY +
			p_PSchElementVarsSrc->oSchElementGraph.oDbObjectFrame.dbY;
	oDbPointPairResult.dbDst.dbX = p_PSchElementVarsDst->oSchElementGraph.oDbObjectPos.dbX +
			p_PSchElementVarsDst->oSchElementGraph.oDbObjectFrame.dbX;
	oDbPointPairResult.dbDst.dbY = p_PSchElementVarsDst->oSchElementGraph.oDbObjectPos.dbY +
			p_PSchElementVarsDst->oSchElementGraph.oDbObjectFrame.dbY;
	// Плюс порт.
	oDbPointPairResult.dbSrc.dbX += oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbX;
	oDbPointPairResult.dbSrc.dbY += oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbY;
	oDbPointPairResult.dbDst.dbX += oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos.dbX;
	oDbPointPairResult.dbDst.dbY += oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos.dbY;
	//
	return oDbPointPairResult;
}

// Вычисление квадрата и вместилища линии линка.
GraphicsLinkItem::CalcPortHelper GraphicsLinkItem::CalcLinkLineWidthHeight() const
{
	CalcPortHelper oRes;
	//
	oRes.oQRectF.setX(0);
	oRes.oQRectF.setY(0);
	oRes.oDbPointPair = CalcPortsCoords();
	//
	if(oRes.oDbPointPair.dbSrc.dbX >= oRes.oDbPointPair.dbDst.dbX) // Если X ист. больше X приёмника...
	{
		oRes.oQRectF.setWidth(oRes.oDbPointPair.dbSrc.dbX - oRes.oDbPointPair.dbDst.dbX);
	}
	else // Если X источника меньше X приёмника...
	{
		oRes.oQRectF.setWidth(oRes.oDbPointPair.dbDst.dbX - oRes.oDbPointPair.dbSrc.dbX);
	}
	if(oRes.oDbPointPair.dbSrc.dbY >= oRes.oDbPointPair.dbDst.dbY) // Если Y ист. больше Y приёмника...
	{
		oRes.oQRectF.setHeight(oRes.oDbPointPair.dbSrc.dbY - oRes.oDbPointPair.dbDst.dbY);
	}
	else // Если Y источника меньше Y приёмника...
	{
		oRes.oQRectF.setHeight(oRes.oDbPointPair.dbDst.dbY - oRes.oDbPointPair.dbSrc.dbY);
	}
	return oRes;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsLinkItem::boundingRect() const
{
	return CalcLinkLineWidthHeight().oQRectF;
}

// Переопределение функции рисования линка.
void GraphicsLinkItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	CalcPortHelper oC;
	DbPoint oDbPointMid;
	QPainterPath oQPainterPath;
	QPainterPathStroker oQPainterPathStroker;
	//
	oC = CalcLinkLineWidthHeight();
	oC.oQRectF.setWidth(oC.oQRectF.width() - 1);
	oC.oQRectF.setHeight(oC.oQRectF.height() - 1);
	// Нахождение центральной точки между источником и приёмником.
	if(oC.oDbPointPair.dbSrc.dbX >= oC.oDbPointPair.dbDst.dbX)
	{
		oDbPointMid.dbX = oC.oDbPointPair.dbDst.dbX + ((oC.oDbPointPair.dbSrc.dbX - oC.oDbPointPair.dbDst.dbX) / 2);
	}
	else
	{
		oDbPointMid.dbX = oC.oDbPointPair.dbSrc.dbX + ((oC.oDbPointPair.dbDst.dbX - oC.oDbPointPair.dbSrc.dbX) / 2);
	}
	if(oC.oDbPointPair.dbSrc.dbY >= oC.oDbPointPair.dbDst.dbY)
	{
		oDbPointMid.dbY = oC.oDbPointPair.dbDst.dbY + ((oC.oDbPointPair.dbSrc.dbY - oC.oDbPointPair.dbDst.dbY) / 2);
	}
	else
	{
		oDbPointMid.dbY = oC.oDbPointPair.dbSrc.dbY + ((oC.oDbPointPair.dbDst.dbY - oC.oDbPointPair.dbSrc.dbY) / 2);
	}
	// Сдвиг в координаты вокруг центральной точки.
	oC.oDbPointPair.dbSrc.dbX -= oDbPointMid.dbX;
	oC.oDbPointPair.dbSrc.dbY -= oDbPointMid.dbY;
	oC.oDbPointPair.dbDst.dbX -= oDbPointMid.dbX;
	oC.oDbPointPair.dbDst.dbY -= oDbPointMid.dbY;
	p_Painter->setPen(SchematicWindow::oQPenBlackTransparent);
	// Если источник по X и Y (обязательно вместе) меньше или больше нуля (левый верхний и нижний правый квадраты)...
	if(((oC.oDbPointPair.dbSrc.dbX <= 0) && (oC.oDbPointPair.dbSrc.dbY <= 0)) ||
			((oC.oDbPointPair.dbSrc.dbX >= 0) && (oC.oDbPointPair.dbSrc.dbY >= 0)))
	{
		// Рисование с левого верхнего угла в правый нижний.
		oQPainterPath.moveTo(oC.oQRectF.x(), oC.oQRectF.y());
		oQPainterPath.lineTo(oC.oQRectF.width() + 1, oC.oQRectF.height() + 1);
	}
	else // Иначе...
	{
		// Рисование с левого нижнего угла в правый верхний.
		oQPainterPath.moveTo(oC.oQRectF.x(), oC.oQRectF.height() + 1);
		oQPainterPath.lineTo(oC.oQRectF.width() + 1, oC.oQRectF.y());
	}
	oQPainterPathStroker.setCapStyle(Qt::RoundCap);
	oQPainterPathStroker.setJoinStyle(Qt::RoundJoin);
	oQPainterPathStroker.setWidth(2);
	QPainterPath oQPainterPathOutlined = oQPainterPathStroker.createStroke(oQPainterPath);
	p_Painter->drawPath(oQPainterPathOutlined);
	p_Painter->setPen(SchematicWindow::oQPenWhiteTransparent);
	p_Painter->drawPath(oQPainterPath);
}

// Переопределение функции шага событий линка.
void GraphicsLinkItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}

// Обновление Z-позиции по данным линка.
void GraphicsLinkItem::UpdateZPosition(GraphicsLinkItem* p_GraphicsLinkItem)
{
	if(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos >
			p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos)
	{
		p_GraphicsLinkItem->setZValue(p_GraphicsLinkItem->p_GraphicsElementItemSrc->
									  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos + SCH_LINK_Z_SHIFT);
	}
	else
	{
		p_GraphicsLinkItem->setZValue(p_GraphicsLinkItem->p_GraphicsElementItemDst->
									  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos + SCH_LINK_Z_SHIFT);
	}
}

// Обновление позиции по данным линка.
void GraphicsLinkItem::UpdatePosition(GraphicsLinkItem* p_GraphicsLinkItem)
{
	DbPoint oDbPoint;
	DbPointPair oP;
	//
	oP = p_GraphicsLinkItem->CalcPortsCoords();
	if(oP.dbSrc.dbX >= oP.dbDst.dbX)
	{
		oDbPoint.dbX = oP.dbDst.dbX;
	}
	else
	{
		oDbPoint.dbX = oP.dbSrc.dbX;
	}
	if(oP.dbSrc.dbY >= oP.dbDst.dbY)
	{
		oDbPoint.dbY = oP.dbDst.dbY;
	}
	else
	{
		oDbPoint.dbY = oP.dbSrc.dbY;
	}
	p_GraphicsLinkItem->setPos(999999999, 999999999); // Вынужденная мера.
	p_GraphicsLinkItem->setPos(oDbPoint.dbX, oDbPoint.dbY);
}
