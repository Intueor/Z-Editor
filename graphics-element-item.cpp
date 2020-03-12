//== ВКЛЮЧЕНИЯ.
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QGraphicsSceneEvent>
#include <QBoxLayout>
#include "main-window.h"
#include "graphics-element-item.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс графического элемента.
// Конструктор.
GraphicsElementItem::GraphicsElementItem(PSchElementBase* p_PSchElementBase)
{
	int iR, iG, iB;
	QColor oQColorBkg;
	//
	setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_ELEMENT);
	p_GraphicsGroupItemRel = nullptr;
	memcpy(&oPSchElementBaseInt, p_PSchElementBase, sizeof(PSchElementBase));
	setFlag(ItemIsMovable);
	setAcceptHoverEvents(true);
	setCursor(Qt::CursorShape::PointingHandCursor);
	bSelected = false;
	//
	p_QGroupBox = new QGroupBox();
	QVBoxLayout* p_QVBoxLayout = new QVBoxLayout;
	p_QGroupBox->setLayout(p_QVBoxLayout);
	p_QGroupBox->setTitle(p_PSchElementBase->m_chName);
	p_QGroupBox->setAttribute(Qt::WA_TranslucentBackground);
	p_QGroupBox->setCursor(Qt::CursorShape::PointingHandCursor);
	oQColorBkg = QColor(QRgb(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uiObjectBkgColor));
	oQColorBkg.getRgb(&iR, &iG, &iB);
	if(((iR + iG + iB) / 3) > 128)
	{
		p_QGroupBox->setStyleSheet("QGroupBox { border:1px solid rgba(0, 0, 0, 255); border-radius: 3px; margin-top: 6px; } "
								   "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; "
								   "left: 4px; padding-top: 0px; }");
		oQPalette.setColor(QPalette::Foreground, QColor(Qt::black));
		bIsPositivePalette = false;
	}
	else
	{
		p_QGroupBox->setStyleSheet("QGroupBox { border:1px solid rgba(255, 255, 255, 255); border-radius: 3px; margin-top: 6px; } "
								   "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; "
								   "left: 4px; padding-top: 0px; }");
		oQPalette.setColor(QPalette::Foreground, QColor(Qt::white));
		bIsPositivePalette = true;
	}
	p_QGraphicsProxyWidget = MainWindow::p_SchematicWindow->GetSchematicView()->scene()->addWidget(p_QGroupBox); // Только так (на Linux).
	p_QGroupBox->move(3, 0);
	p_QGroupBox->setFixedSize(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW - 6,
							  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH - 3);
	p_QGraphicsProxyWidget->setFiltersChildEvents(true);
	p_QGraphicsProxyWidget->setParentItem(this);
	oQPalette.setBrush(QPalette::Background, oQBrush);
	p_QGroupBox->setPalette(oQPalette);
	//
	setPos(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX,
		   oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY);
	setZValue(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos);
	if(SchematicWindow::dbObjectZPos <= oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos)
	{
		SchematicWindow::dbObjectZPos = oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	oQBrush.setColor(QRgb(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uiObjectBkgColor));
	//
	p_GraphicsScalerItem = new GraphicsScalerItem(this);
	p_GraphicsScalerItem->setParentItem(this);
	p_GraphicsScalerItem->setPos(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW,
								 oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH);
	p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_ELEMENT, this);
	p_GraphicsFrameItem->hide();
	//
	SchematicView::SetElementBlockingPattern(this, oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy);
}

// Деструктор.
GraphicsElementItem::~GraphicsElementItem()
{
	delete p_QGroupBox;
	delete p_GraphicsFrameItem;
	delete p_GraphicsScalerItem;
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsElementItem::boundingRect() const
{
	return QRectF(0, 0,
				  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW,
				  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH);
}

// Переопределение функции рисования элемента.
void GraphicsElementItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
{
	// Заглушки.
	p_Option = p_Option;
	p_Widget = p_Widget;
	//
	p_Painter->setRenderHints(QPainter::SmoothPixmapTransform);
	p_Painter->setBrush(oQBrush);
	if(bIsPositivePalette)
	{
		p_Painter->setPen(SchematicWindow::oQPenWhite);
	}
	else
	{
		p_Painter->setPen(SchematicWindow::oQPenBlack);
	}
	p_Painter->drawRect(0, 0,
						oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW,
						oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH);
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
