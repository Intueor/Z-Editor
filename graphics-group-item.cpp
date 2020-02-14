//== ВКЛЮЧЕНИЯ.
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneEvent>
#include "main-window.h"
#include "graphics-group-item.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"

//== МАКРОСЫ.
#define LOG_NAME                                "Group"
#define LOG_DIR_PATH							"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS_MULTIOBJECT(GraphicsGroupItem)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(GraphicsGroupItem)

//== ФУНКЦИИ КЛАССОВ.
//== Класс графической группы.
// Конструктор.
GraphicsGroupItem::GraphicsGroupItem(PSchGroupBase* p_PSchGroupBase)
{
	LOG_CTRL_INIT_MULTIOBJECT;
	//
	int iR, iG, iB;
	QColor oQColorBkg;
	//
	setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_GROUP);
	memcpy(&oPSchGroupBaseInt, p_PSchGroupBase, sizeof(PSchGroupBase));
	setFlag(ItemIsMovable);
	setAcceptHoverEvents(true);
	setCursor(Qt::CursorShape::PointingHandCursor);
	bSelected = false;
	//
	oQColorBkg = QColor(QRgb(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor));
	oQColorBkg.getRgb(&iR, &iG, &iB);
	if(((iR + iG + iB) / 3) > 128)
	{
		oQPalette.setColor(QPalette::Foreground, QColor(Qt::black));
		bIsPositivePalette = false;
	}
	else
	{
		oQPalette.setColor(QPalette::Foreground, QColor(Qt::white));
		bIsPositivePalette = true;
	}
	//
	setPos(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX,
		   oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY);
	setZValue(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos);
	if(SchematicWindow::dbObjectZPos <= oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos)
	{
		SchematicWindow::dbObjectZPos = oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	oQBrush.setColor(QRgb(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor));
	//
	p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_GROUP, 0, this);
	p_GraphicsFrameItem->hide();
	//
	p_QLabel = new QLabel(QString(oPSchGroupBaseInt.m_chName));
	p_QLabel->setAttribute(Qt::WA_TranslucentBackground);
	p_QLabel->setCursor(Qt::CursorShape::PointingHandCursor);
	p_QLabel->move(3, 3);
	p_QLabel->setFixedSize(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW - 6, 13);
	p_QGraphicsProxyWidget = MainWindow::p_SchematicWindow->oScene.addWidget(p_QLabel);
	p_QGraphicsProxyWidget->setFiltersChildEvents(true);
	p_QGraphicsProxyWidget->setParentItem(this);
	p_QLabel->setPalette(oQPalette);
	//
	SetBlockingPattern(this, oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy);
}

// Деструктор.
GraphicsGroupItem::~GraphicsGroupItem()
{
	delete p_GraphicsFrameItem;
}

// Установка параметров группы по структуре.
void GraphicsGroupItem::SetGroupParameters(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupBase& a_PSchGroupBase)
{
	p_GraphicsGroupItem->oPSchGroupBaseInt = a_PSchGroupBase;
}

// Входное обновление параметров группы по структуре.
void GraphicsGroupItem::IncomingUpdateGroupParameters(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupVars& a_SchGroupVars)
{
	if(a_SchGroupVars.oSchGroupGraph.uchChangesBits & SCH_GROUP_BIT_ELEMENTS_SHIFT)
	{
		double dbXShift = a_SchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX -
				p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX;
		double dbYShift = a_SchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY -
				p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY;
		GraphicsElementItem* p_GraphicsElementItem;
		//
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] elements shift.");
		for(int iE = 0; iE != p_GraphicsGroupItem->vp_ConnectedElements.count(); iE++)
		{
			p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iE);
			LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] shifting.");
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX += dbXShift;
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY += dbYShift;
			p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem, SCH_UPDATE_ELEMENT_POS | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
			LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] free by shifting.");
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy = false;
			p_GraphicsElementItem->SetBlockingPattern(p_GraphicsElementItem, false);
		}
	}
	// Фрейм.
	if(a_SchGroupVars.oSchGroupGraph.uchChangesBits & SCH_GROUP_BIT_FRAME)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] frame.");
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
				a_SchGroupVars.oSchGroupGraph.oDbObjectFrame;
		UpdateSelected(p_GraphicsGroupItem, SCH_UPDATE_GROUP_FRAME | SCH_UPDATE_MAIN);
	}
	// Цвет подложки.
	if(a_SchGroupVars.oSchGroupGraph.uchChangesBits & SCH_GROUP_BIT_BKG_COLOR)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] color.");
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor =
				a_SchGroupVars.oSchGroupGraph.uiObjectBkgColor;
		p_GraphicsGroupItem->oQBrush.setColor(
					QRgb(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor));
	}
	// Занятость.
	if(a_SchGroupVars.oSchGroupGraph.uchChangesBits & SCH_GROUP_BIT_BUSY)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] busy.");
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy = a_SchGroupVars.oSchGroupGraph.bBusy;
		p_GraphicsGroupItem->SetBlockingPattern(
					p_GraphicsGroupItem, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy);
	}
	// Z-позиция.
	if(a_SchGroupVars.oSchGroupGraph.uchChangesBits & SCH_GROUP_BIT_ZPOS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName).toStdString() << "] z-pos.");
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos = a_SchGroupVars.oSchGroupGraph.dbObjectZPos;
		UpdateSelected(p_GraphicsGroupItem, SCH_UPDATE_GROUP_ZPOS | SCH_UPDATE_MAIN);
		SchematicWindow::dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
		SortGroupElementsToTop(p_GraphicsGroupItem, nullptr, false, false, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy);
	}
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsGroupItem::boundingRect() const
{
	return QRectF(0, 0, oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW,
				  oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH);
}

// Переопределение функции рисования группы.
void GraphicsGroupItem::paint(QPainter *p_Painter, const QStyleOptionGraphicsItem *p_Option, QWidget *p_Widget)
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
	p_Painter->drawRect(0, 0, oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW,
						oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH);
	p_Painter->drawLine(1, 18, oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW - 1, 18);
	//////////////////////////////
	p_Painter->drawText(170, 15, QString::number(this->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos));
	//////////////////////////////
}

// Переопределение функции шага событий группы.
void GraphicsGroupItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}

// Сортировка вектора элементов по Z-позиции с приоритетом по выборке.
void GraphicsGroupItem::SortElementsByZPos(QVector<GraphicsElementItem*>& avp_Elements, GraphicsElementItem* p_GraphicsElementItemExclude,
										   QVector<GraphicsElementItem*>* pvp_SortedElements, QVector<GraphicsElementItem*>* pvp_SelectionSortedElements)
{
	GraphicsElementItem* p_GraphicsElementItem;
	QVector<GraphicsElementItem*> vp_Elements;
	//
	// Копия для работы.
	for(int iF = 0; iF < avp_Elements.count(); iF++)
	{
		p_GraphicsElementItem = avp_Elements.at(iF);
		if(p_GraphicsElementItem != p_GraphicsElementItemExclude)
		{
			vp_Elements.append(p_GraphicsElementItem);
		}
	}
	// Сортировка.
	while(!vp_Elements.isEmpty())
	{
		int iElement;
		qreal dbZ = 999999999999;
		for(int iF = 0; iF < vp_Elements.count(); iF++)
		{
			p_GraphicsElementItem = vp_Elements.at(iF);
			if(dbZ > p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos)
			{
				dbZ = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos;
				iElement = iF;
			}
		}
		p_GraphicsElementItem = vp_Elements.at(iElement);
		if(pvp_SelectionSortedElements != nullptr)
		{
			if(SchematicWindow::vp_SelectedElements.contains(p_GraphicsElementItem))
			{
				pvp_SelectionSortedElements->append(p_GraphicsElementItem);
			}
			else
			{
				goto gSE;
			}
		}
		else
		{
gSE:			pvp_SortedElements->append(p_GraphicsElementItem);
		}
		vp_Elements.removeAt(iElement);
	}
}

// Сортировка вектора групп по Z-позиции с приоритетом по выборке.
void GraphicsGroupItem::SortGroupsByZPos(QVector<GraphicsGroupItem*>& avp_Groups, GraphicsGroupItem* p_GraphicsGroupItemExclude,
										 QVector<GraphicsGroupItem*>* pvp_SortedGroups, QVector<GraphicsGroupItem*>* pvp_SelectionSortedGroups)
{
	GraphicsGroupItem* p_GraphicsGroupItem;
	QVector<GraphicsGroupItem*> vp_Groups;
	//
	// Копия для работы.
	for(int iF = 0; iF < avp_Groups.count(); iF++)
	{
		p_GraphicsGroupItem = avp_Groups.at(iF);
		if(p_GraphicsGroupItem != p_GraphicsGroupItemExclude)
		{
			vp_Groups.append(p_GraphicsGroupItem);
		}
	}
	// Сортировка.
	while(!vp_Groups.isEmpty())
	{
		int iGroup;
		qreal dbZ = 999999999999;
		for(int iF = 0; iF < vp_Groups.count(); iF++)
		{
			p_GraphicsGroupItem = vp_Groups.at(iF);
			if(dbZ > p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos)
			{
				dbZ = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos;
				iGroup = iF;
			}
		}
		p_GraphicsGroupItem = vp_Groups.at(iGroup);
		if(pvp_SelectionSortedGroups != nullptr)
		{
			if(SchematicWindow::vp_SelectedGroups.contains(p_GraphicsGroupItem))
			{
				pvp_SelectionSortedGroups->append(p_GraphicsGroupItem);
			}
			else
			{
				goto gSE;
			}
		}
		else
		{
gSE:			pvp_SortedGroups->append(p_GraphicsGroupItem);
		}
		vp_Groups.removeAt(iGroup);
	}
}

// Подъём элементов группы на первый план с сортировкой.
void GraphicsGroupItem::SortGroupElementsToTop(GraphicsGroupItem* p_GraphicsGroupItem,
										  GraphicsElementItem* p_GraphicsElementItemExclude, bool bWithSelectedDiff,
																		   bool bSend, bool bBlokingPatterns)
{
	QVector<GraphicsElementItem*> vp_SortedElements;
	QVector<GraphicsElementItem*> vp_SelectionSortedElements;
	QVector<GraphicsElementItem*>* pvp_SelectionSortedElements;
	//
	if(bWithSelectedDiff) pvp_SelectionSortedElements = &vp_SelectionSortedElements; else pvp_SelectionSortedElements = nullptr;
	SortElementsByZPos(p_GraphicsGroupItem->vp_ConnectedElements, p_GraphicsElementItemExclude, &vp_SortedElements, pvp_SelectionSortedElements);
	for(int iF = 0; iF != vp_SortedElements.count(); iF++)
	{
		GraphicsElementItem::ElementToTop(vp_SortedElements.at(iF), bBlokingPatterns, bSend);
	}
	if(bWithSelectedDiff)
	{
		for(int iF = vp_SelectionSortedElements.count() - 1; iF != -1; iF--)
		{
			GraphicsElementItem::ElementToTop(vp_SelectionSortedElements.at(iF), bBlokingPatterns, bSend);
		}
	}
	SchematicView::UpdateLinksZPos();
}

// Поднятие группы на первый план.
void GraphicsGroupItem::GroupToTop(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend,
														 GraphicsElementItem* p_GraphicsElementItemExclude, bool bBlokingPatterns, bool bSendElements)
{
	PSchGroupVars oPSchGroupVars;
	//
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
	oPSchGroupVars.oSchGroupGraph.dbObjectZPos =
			p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos;
	p_GraphicsGroupItem->setZValue(SchematicWindow::dbObjectZPos);
	SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
	p_GraphicsGroupItem->update();
	if(bSend)
	{
		p_GraphicsGroupItem->SetBlockingPattern(p_GraphicsGroupItem, true);
		oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
		oPSchGroupVars.oSchGroupGraph.bBusy = true;
		oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_ZPOS | SCH_GROUP_BIT_BUSY;
		MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars,
													  sizeof(PSchGroupVars));
	}
	if(bSend == false) bSendElements = false;
	SortGroupElementsToTop(p_GraphicsGroupItem, p_GraphicsElementItemExclude, true, bSendElements, bBlokingPatterns);
}

// Переопределение функции обработки нажатия мыши.
void GraphicsGroupItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	bool bLastSt;
	//
	if(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(event->button() == Qt::MouseButton::LeftButton)
	{
		bLastSt = bSelected;
		if(event->modifiers() == Qt::ControlModifier)
		{
			bSelected = !bSelected;
		}
		if(bSelected)
		{
			if(bLastSt != bSelected)
			{
				SchematicWindow::vp_SelectedGroups.push_front(this);
				p_GraphicsFrameItem->show();
			}
			QVector<GraphicsGroupItem*> vp_SortedGroups;
			//
			SortGroupsByZPos(SchematicWindow::vp_SelectedGroups, this, &vp_SortedGroups); // Сортировка групп в выборке.
			for(int iE = 0; iE != vp_SortedGroups.count(); iE ++)
			{
				GraphicsGroupItem* p_GraphicsGroupItem;
				//
				p_GraphicsGroupItem = vp_SortedGroups.at(iE);
				if(p_GraphicsGroupItem != this)
				{
					GroupToTop(p_GraphicsGroupItem, SEND_GROUP, nullptr, ELEMENTS_BLOCKING_PATTERN_ON, DONT_SEND_ELEMENTS);
				}
			}
		}
		else
		{
			if(bLastSt != bSelected)
			{
				int iN;
				//
				iN = SchematicWindow::vp_SelectedGroups.indexOf(this);
				if(iN != -1)
				{
					SchematicWindow::vp_SelectedGroups.removeAt(iN);
					p_GraphicsFrameItem->hide();
				}
			}
		}
		GroupToTop(this, SEND_GROUP, nullptr, ELEMENTS_BLOCKING_PATTERN_ON, DONT_SEND_ELEMENTS);
	}
	else if(event->button() == Qt::MouseButton::RightButton)
	{
		if(SchematicWindow::p_Menu == nullptr)
		{
			SchematicWindow::p_Menu = new QMenu;
			//================= СОСТАВЛЕНИЕ ПУНКТОВ МЕНЮ. =================//
			// Объект.
			SchematicWindow::p_Menu->addAction(QString(m_chGroup) +
											   " [" + QString(this->oPSchGroupBaseInt.m_chName) + "]")->setDisabled(true);
			// Имя.
			SchematicWindow::p_Menu->addAction(QString(m_chMenuRename));
			// Удалить.
			SchematicWindow::p_Menu->addAction(QString(m_chDelete));
			// Расформировать.
			SchematicWindow::p_Menu->addAction(QString(m_chDisband));
			// Создать элемент в группе.
			SchematicWindow::p_Menu->addAction(QString(m_chAddElement));
			// Добавить выбранные свободные объекты.
			if(!SchematicWindow::vp_SelectedElements.isEmpty())
			{
				SchematicWindow::vp_SelectedFreeElements.clear();
				for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
				{
					GraphicsElementItem* p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iF);
					if(p_GraphicsElementItem->p_GraphicsGroupItemRel == nullptr)
					{
						SchematicWindow::vp_SelectedFreeElements.append(p_GraphicsElementItem);
					}
				}
				if(!SchematicWindow::vp_SelectedFreeElements.isEmpty())
				{
					SchematicWindow::p_Menu->addAction(QString(m_chAddFreeSelected));
				}
			}
			// Цвет фона.
			SchematicWindow::p_Menu->addAction(QString(m_chBackground));
		}
	}
	if(!MainWindow::p_Client->oInternalNetHub.CheckIsBufferFree())
	{
		MainWindow::p_Client->SendBufferToServer();
	}
	QGraphicsItem::mousePressEvent(event);
}

// Перемещение группы.
void GraphicsGroupItem::MoveGroup(GraphicsGroupItem* p_GraphicsGroupItem, QPointF& a_QPointFRes, bool bMoveBody)
{
	GraphicsElementItem* p_GraphicsElementItem;
	//
	if(bMoveBody)
	{
		p_GraphicsGroupItem->setPos(p_GraphicsGroupItem->x() + a_QPointFRes.x(), p_GraphicsGroupItem->y() + a_QPointFRes.y());
	}
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX += a_QPointFRes.x();
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY += a_QPointFRes.y();
	p_GraphicsGroupItem->UpdateSelected(p_GraphicsGroupItem, SCH_UPDATE_MAIN);
	//
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++)
	{
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iF);
		p_GraphicsElementItem->setPos(p_GraphicsElementItem->x() + a_QPointFRes.x(), p_GraphicsElementItem->y() + a_QPointFRes.y());
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX += a_QPointFRes.x();
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY += a_QPointFRes.y();
		p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
	}
}

// Переопределение функции обработки перемещения мыши.
void GraphicsGroupItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	int iC;
	QPointF oQPointFInit;
	QPointF oQPointFRes;
	//
	if(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	oQPointFInit = pos();
	QGraphicsItem::mouseMoveEvent(event);
	oQPointFRes = pos();
	oQPointFRes.setX(oQPointFRes.x() - oQPointFInit.x()); // Смещение по X.
	oQPointFRes.setY(oQPointFRes.y() - oQPointFInit.y()); // Смещение по Y.
	if(bSelected)
	{
		iC = SchematicWindow::vp_SelectedGroups.count();
		for(int iE = 0; iE != iC; iE ++)
		{
			GraphicsGroupItem* p_GraphicsGroupItem;
			//
			p_GraphicsGroupItem = SchematicWindow::vp_SelectedGroups.at(iE);
			if(p_GraphicsGroupItem != this)
			{
				MoveGroup(p_GraphicsGroupItem, oQPointFRes);
			}
		}
	}
	MoveGroup(this, oQPointFRes, false);
}

// Отпускание группы.
void GraphicsGroupItem::ReleaseGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bWithElements)
{
	GraphicsElementItem* p_GraphicsElementItem;
	PSchGroupVars oPSchGroupVars;
	//
	if(p_GraphicsGroupItem->oQBrush.style() == Qt::SolidPattern)
		return;
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX = p_GraphicsGroupItem->x();
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY = p_GraphicsGroupItem->y();
	oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
			p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
	oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
	p_GraphicsGroupItem->SetBlockingPattern(p_GraphicsGroupItem, false);
	oPSchGroupVars.oSchGroupGraph.dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos;
	oPSchGroupVars.oSchGroupGraph.bBusy = false;
	oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_FRAME | SCH_GROUP_BIT_BUSY;
	if(!bWithElements) // Если без элементов - смещаться будут на клиентах.
	{
		oPSchGroupVars.oSchGroupGraph.uchChangesBits |= SCH_GROUP_BIT_ELEMENTS_SHIFT;
	}
	else // Если с элементами - отправка Z-позиции.
	{
		oPSchGroupVars.oSchGroupGraph.uchChangesBits |= SCH_GROUP_BIT_ZPOS;
	}
	MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars,
												  sizeof(PSchGroupVars));
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++)
	{
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iF);
		p_GraphicsElementItem->ReleaseElement(p_GraphicsElementItem, false); // Если без элементов, то только на сервер при отпусткании.
	}
}

// Переопределение функции обработки отпускания мыши.
void GraphicsGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	if(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy || MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(event->button() == Qt::MouseButton::LeftButton)
	{
		if(bSelected)
		{
			QVector<GraphicsGroupItem*> vp_SortedGroups;
			//
			SortGroupsByZPos(SchematicWindow::vp_SelectedGroups, this, &vp_SortedGroups); // Сортировка групп в выборке.
			for(int iE = 0; iE != vp_SortedGroups.count(); iE ++)
			{
				GraphicsGroupItem* p_GraphicsGroupItem;
				//
				p_GraphicsGroupItem = vp_SortedGroups.at(iE);
				if(p_GraphicsGroupItem != this)
				{
					ReleaseGroup(p_GraphicsGroupItem, false);
				}
			}
		}
		ReleaseGroup(this, false);
		MainWindow::p_Client->SendBufferToServer();
	}
	QGraphicsItem::mouseReleaseEvent(event);
	if(SchematicWindow::p_Menu != nullptr)
	{
		QAction* p_SelectedMenuItem;
		Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog;
		PSchGroupName oPSchGroupName;
		char m_chName[SCH_OBJ_NAME_STR_LEN];
		//================= ВЫПОЛНЕНИЕ ПУНКТОВ МЕНЮ. =================//
		p_SelectedMenuItem = SchematicWindow::p_Menu->exec(QCursor::pos());
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->text() == QString(m_chMenuRename))
			{
				CopyStrArray(oPSchGroupBaseInt.m_chName, m_chName, SCH_OBJ_NAME_STR_LEN);
				p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Имя группы", m_chName, SCH_OBJ_NAME_STR_LEN);
				if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
				{
					memset(&oPSchGroupName, 0, sizeof(oPSchGroupName));
					CopyStrArray(m_chName, oPSchGroupName.m_chName, SCH_OBJ_NAME_STR_LEN);
					oPSchGroupName.ullIDInt = oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
					MainWindow::p_Client->SendToServerImmediately(PROTO_O_SCH_GROUP_NAME, (char*)&oPSchGroupName,
																  sizeof(oPSchGroupName));
					this->p_QLabel->setText(oPSchGroupName.m_chName);
					SchematicWindow::p_MainWindow->p_SchematicWindow->update();
				}
				p_Set_Proposed_String_Dialog->deleteLater();
			}
			else if(p_SelectedMenuItem->text() == QString(m_chDelete))
			{
				if(!SchematicWindow::vp_SelectedGroups.contains(this))
				{
					SchematicWindow::vp_SelectedGroups.append(this);
				}
				SchematicView::DeleteSelected();
			}
			else if(p_SelectedMenuItem->text() == QString(m_chPorts))
			{

			}
			else if(p_SelectedMenuItem->text() == QString(m_chAddToGroup))
			{

			}
			else if(p_SelectedMenuItem->text() == QString(m_chRemoveFromGroup))
			{

			}
			else if(p_SelectedMenuItem->text() == QString(m_chAddElement))
			{
				GraphicsElementItem* p_GraphicsElementItem;
				QString strHelper = QString(m_chNewElement);
				//
				p_GraphicsElementItem = SchematicView::CreateNewElementAndPrepareForSending(strHelper,
																  this->mapToScene(event->pos()), this->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt);
				vp_ConnectedElements.push_front(p_GraphicsElementItem);
				p_GraphicsElementItem->p_GraphicsGroupItemRel = this;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = this->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
				GraphicsElementItem::UpdateGroupFrameByElements(this);
				GraphicsElementItem::UpdateGroupAndAffectedElements(this);
				MainWindow::p_Client->SendBufferToServer();
				SchematicView::UpdateLinksZPos();
			}
			else if(p_SelectedMenuItem->text() == QString(m_chAddFreeSelected))
			{
				if(GraphicsElementItem::AddFreeSelectedElementsToGroup(this))
				{
					MainWindow::p_Client->SendBufferToServer();
				};
			}
			else if(p_SelectedMenuItem->text() == QString(m_chBackground))
			{

			}
		}
		delete SchematicWindow::p_Menu;
		SchematicWindow::p_Menu = nullptr;
	}
}

// Установка блокировки на группу.
void GraphicsGroupItem::SetBlockingPattern(GraphicsGroupItem* p_GraphicsGroupItem, bool bValue)
{
	if(bValue)
	{
		p_GraphicsGroupItem->oQBrush.setStyle(Qt::Dense6Pattern);
	}
	else
	{
		p_GraphicsGroupItem->oQBrush.setStyle(Qt::SolidPattern);
	}
	p_GraphicsGroupItem->UpdateSelected(p_GraphicsGroupItem, SCH_UPDATE_MAIN);
}

// Обновление выбранных граф. объектов.
void GraphicsGroupItem::UpdateSelected(GraphicsGroupItem* p_GraphicsGroupItem, unsigned short ushBits)
{
	if(ushBits & SCH_UPDATE_GROUP_ZPOS)
	{
		p_GraphicsGroupItem->setZValue(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos);
		SchematicView::UpdateLinksZPos();
	}
	if(ushBits & SCH_UPDATE_GROUP_FRAME)
	{
		p_GraphicsGroupItem->setPos(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX,
			   p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY);
		WidgetsThrAccess::p_ConnGraphicsGroupItem = p_GraphicsGroupItem;
		ThrUiAccessET(MainWindow::p_WidgetsThrAccess, GroupLabelWidthSet);
		SchematicWindow::p_QGraphicsScene->update();
	}
	if(ushBits & SCH_UPDATE_MAIN)
	{
		p_GraphicsGroupItem->update();
	}
}
