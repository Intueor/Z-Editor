//== ВКЛЮЧЕНИЯ.
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QGraphicsSceneEvent>
#include <QBoxLayout>
#include "main-window.h"
#include "graphics-element-item.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"

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
	bool bLastSt;
	//
	if(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{ //Если элемент блокирован занятостью, смещением выборки или главным окном - отказ.
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		// Создание нового порта.
		if(p_Event->modifiers() == Qt::AltModifier)
		{
			PSchLinkBase oPSchLinkBase;
			DbPoint oDbPointInitialClick;
			//
			oDbPointInitialClick.dbX = p_Event->scenePos().x();
			oDbPointInitialClick.dbY = p_Event->scenePos().y();
			//
			oPSchLinkBase.oPSchLinkVars.ullIDSrc = oPSchElementBaseInt.oPSchElementVars.ullIDInt;
			oPSchLinkBase.oPSchLinkVars.ullIDDst = oPSchLinkBase.oPSchLinkVars.ullIDSrc; // Временно, пока не перетянут на новый элемент.
			oPSchLinkBase.oPSchLinkVars.ushiSrcPort = DEFAULT_NEW_PORT;
			oPSchLinkBase.oPSchLinkVars.ushiDstPort = DEFAULT_NEW_PORT;
			// В координаты элемента.
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos = oDbPointInitialClick;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbX -=
					oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbY -=
					oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos = oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos = SchematicView::BindToInnerEdge(this, oDbPointInitialClick);
			// Создание замкнутого линка (пока что).
			SchematicView::p_GraphicsLinkItemNew = new GraphicsLinkItem(&oPSchLinkBase);
			if(oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.uchChangesBits != SCH_LINK_BIT_INIT_ERROR)
			{
				MainWindow::p_SchematicWindow->oScene.addItem(SchematicView::p_GraphicsLinkItemNew);
			}
			else
			{
				delete SchematicView::p_GraphicsLinkItemNew;
				SchematicView::p_GraphicsLinkItemNew = nullptr;
				goto gNL;
			}
			SchematicWindow::vp_Links.push_front(SchematicView::p_GraphicsLinkItemNew);
			SchematicView::UpdateLinkZPositionByElements(SchematicView::p_GraphicsLinkItemNew);
			MainWindow::p_This->RemoteUpdateSchView();
			SchematicView::p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mousePressEvent(p_Event);
			QGraphicsItem::mousePressEvent(p_Event);
			SchematicView::p_GraphicsLinkItemNew->p_GraphicsPortItemDst->p_GraphicsFrameItem->show(); // Зажигаем рамку.
			SchematicWindow::p_GraphicsFrameItemForPortFlash = SchematicView::p_GraphicsLinkItemNew->p_GraphicsPortItemDst->p_GraphicsFrameItem;
			return;
		}
		//==== РАБОТА С ВЫБОРКОЙ. ====
gNL:	bLastSt = bSelected; // Запоминаем предыдущее значение выбраности.
		if(p_Event->modifiers() == Qt::ControlModifier)
		{ // При удержании CTRL - инверсия флага выбраности.
			bSelected = !bSelected;
		}
		if(bSelected) // ВЫБРАЛИ...
			SchematicView::SelectElement(this, bLastSt);
		else // ОТМЕНИЛИ ВЫБОР...
			SchematicView::DeselectElement(this, bLastSt);
	}
	else if(p_Event->button() == Qt::MouseButton::RightButton)
	{
		if(SchematicWindow::p_SafeMenu == nullptr)
		{
			SchematicWindow::p_SafeMenu = new SafeMenu;
			SchematicWindow::p_SafeMenu->setStyleSheet("SafeMenu::separator {color: palette(link);}");
			//================= СОСТАВЛЕНИЕ ПУНКТОВ МЕНЮ. =================//
			// Объект.
			QString strCaption;
			bool bNoSelection = SchematicWindow::vp_SelectedElements.isEmpty();
			//
			if(bNoSelection)
			{
				strCaption = QString(m_chElement) +
						" [" + QString(oPSchElementBaseInt.m_chName) + "]";
			}
			else
			{
				strCaption = "Выборка элементов";
			}
			SchematicWindow::p_SafeMenu->setMinimumWidth(SchematicView::GetStringWidthInPixels(SchematicWindow::p_SafeMenu->font(), strCaption) + 34);
			SchematicWindow::p_SafeMenu->addSection(strCaption)->setDisabled(true);
			// Имя.
			if(bNoSelection)
			{
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuRename))->setData(MENU_RENAME);
			}
			else
			{
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuRenameSelection))->setData(MENU_RENAME_SELECTION);
			}
			// Удалить.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuDelete))->setData(MENU_DELETE);
			// Порты.
			for(int iF = 0; iF !=  SchematicWindow::vp_Ports.count(); iF++)
			{
				if(this == SchematicWindow::vp_Ports.at(iF)->p_ParentInt)
				{
					SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuPorts))->setData(MENU_PORTS);
					break;
				}
			}
			// Создать группу.
			if(oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
			{
				SchematicWindow::p_SafeMenu->addAction(m_chMenuCreateGroup)->setData(MENU_CREATE_GROUP);
			}
			// В группу.
			if(oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
			{
				if(SchematicWindow::vp_SelectedGroups.count() == 1)
				{
					SchematicWindow::p_SafeMenu->addAction(QString(QString(m_chMenuAddFreeSelected) +
																   " [" + QString(SchematicWindow::vp_SelectedGroups.at(0)->oPSchGroupBaseInt.m_chName)
																   + "]"))->setData(MENU_ADD_FREE_SELECTED);
				}
			}
			// Из группы.
			if(oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
			{
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuRemoveFromGroup))->setData(MENU_REMOVE_FROM_GROUP);
			}
			// Цвет фона.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuBackground))->setData(MENU_CHANGE_BACKGROUND);
		}
	}
	TrySendBufferToServer;
	QGraphicsItem::mousePressEvent(p_Event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsElementItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	QPointF oQPointFInit;
	QPointF oQPointFRes;
	//
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(SchematicView::p_GraphicsLinkItemNew != nullptr)
	{
		SchematicView::p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mouseMoveEvent(p_Event);
		return;
	}
	else if(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy) return;
	oQPointFInit = pos();
	QGraphicsItem::mouseMoveEvent(p_Event);
	oQPointFRes = pos();
	oQPointFRes.setX(oQPointFRes.x() - oQPointFInit.x());
	oQPointFRes.setY(oQPointFRes.y() - oQPointFInit.y());
	if(bSelected)
	{
		iC = SchematicWindow::vp_SelectedElements.count();
		for(int iE = 0; iE != iC; iE ++)
		{
			GraphicsElementItem* p_GraphicsElementItem;
			//
			p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iE);
			if(p_GraphicsElementItem != this)
			{
				p_GraphicsElementItem->setPos(p_GraphicsElementItem->x() + oQPointFRes.x(), p_GraphicsElementItem->y() + oQPointFRes.y());
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX += oQPointFRes.x();
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY += oQPointFRes.y();
				SchematicView::UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
			}
		}
	}
	oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX = x();
	oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY = y();
	SchematicView::UpdateSelectedInElement(this, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
}

// Переопределение функции обработки отпускания мыши.
void GraphicsElementItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	//
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(SchematicView::p_GraphicsLinkItemNew != nullptr)
	{
		SchematicView::p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mouseReleaseEvent(p_Event);
		SchematicView::p_GraphicsLinkItemNew = nullptr;
		QGraphicsItem::mouseReleaseEvent(p_Event);
		return;
	}
	else if(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy) return;
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(bSelected)
		{
			iC = SchematicWindow::vp_SelectedElements.count();
			for(int iE = 0; iE != iC; iE ++)
			{
				GraphicsElementItem* p_GraphicsElementItem;
				//
				p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iE);
				if(p_GraphicsElementItem != this)
				{
					SchematicView::ReleaseElementAPFS(p_GraphicsElementItem);
				}
			}
		}
		SchematicView::ReleaseElementAPFS(this);
		TrySendBufferToServer;
	}
	QGraphicsItem::mouseReleaseEvent(p_Event);
	if(SchematicWindow::p_SafeMenu != nullptr)
	{
		QAction* p_SelectedMenuItem;
		Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog;
		PSchElementName oPSchElementName;
		char m_chName[SCH_OBJ_NAME_STR_LEN];
		//================= ВЫПОЛНЕНИЕ ПУНКТОВ МЕНЮ. =================//
		p_SelectedMenuItem = SchematicWindow::p_SafeMenu->exec(QCursor::pos());
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->data() == MENU_RENAME)
			{
				CopyStrArray(oPSchElementBaseInt.m_chName, m_chName, SCH_OBJ_NAME_STR_LEN);
				p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Имя элемента", m_chName, SCH_OBJ_NAME_STR_LEN);
				if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
				{
					memset(&oPSchElementName, 0, sizeof(oPSchElementName));
					CopyStrArray(m_chName, oPSchElementName.m_chName, SCH_OBJ_NAME_STR_LEN);
					CopyStrArray(m_chName, oPSchElementBaseInt.m_chName, SCH_OBJ_NAME_STR_LEN);
					oPSchElementName.ullIDInt = oPSchElementBaseInt.oPSchElementVars.ullIDInt;
					MainWindow::p_Client->SendToServerImmediately(PROTO_O_SCH_ELEMENT_NAME, (char*)&oPSchElementName,
																  sizeof(oPSchElementName));
					p_QGroupBox->setTitle(oPSchElementName.m_chName);
					SchematicWindow::p_MainWindow->p_SchematicWindow->update();
				}
				p_Set_Proposed_String_Dialog->deleteLater();
			}
			else if(p_SelectedMenuItem->data() == MENU_DELETE)
			{
				if(!SchematicWindow::vp_SelectedElements.contains(this))
				{
					SchematicWindow::vp_SelectedElements.append(this);
				}
				SchematicView::DeleteSelectedAPFS();
			}
			else if(p_SelectedMenuItem->data() == MENU_PORTS)
			{

			}
			else if(p_SelectedMenuItem->data() == MENU_CREATE_GROUP)
			{
				bool bForceSelected = false;
				GraphicsGroupItem* p_GraphicsGroupItem;
				PSchGroupBase oPSchGroupBase;
				QString strName = QString(m_chNewGroup);
				GraphicsElementItem* p_GraphicsElementItem;
				unsigned char uchR = rand() % 255;
				unsigned char uchG = rand() % 255;
				unsigned char uchB = rand() % 255;
				SchematicView::vp_NewElementsForGroup = new QVector<GraphicsElementItem*>;
				//
				if(!SchematicWindow::vp_SelectedElements.contains(this))
				{
					SchematicWindow::vp_SelectedElements.append(this);
					bForceSelected = true;
				}
				oPSchGroupBase.oPSchGroupVars.ullIDInt = GenerateID();
				strName += ": " + QString::number(oPSchGroupBase.oPSchGroupVars.ullIDInt);
				CopyStrArray((char*)strName.toStdString().c_str(), oPSchGroupBase.m_chName, SCH_OBJ_NAME_STR_LEN);
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
				SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor = QColor(uchR, uchG, uchB).rgb();
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.bBusy = false;
				p_GraphicsGroupItem = new GraphicsGroupItem(&oPSchGroupBase);
				MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsGroupItem);
				SchematicWindow::vp_Groups.push_front(p_GraphicsGroupItem);
				for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
				{
					p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iF);
					if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
					{
						p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = oPSchGroupBase.oPSchGroupVars.ullIDInt;
						p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
						p_GraphicsGroupItem->vp_ConnectedElements.append(p_GraphicsElementItem);
						SchematicView::vp_NewElementsForGroup->append(p_GraphicsElementItem);
					}
				}
				SchematicView::UpdateGroupFrameByElements(p_GraphicsGroupItem);
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
						p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_GROUP_BASE, (char*)&oPSchGroupBase, sizeof(PSchGroupBase));
				SchematicView::SortGroupElementsToTopAPFS(p_GraphicsGroupItem, SEND_NEW_ELEMENTS_TO_GROUP, ADD_SEND_ZPOS,
														   nullptr, DONT_GET_SELECTED_ELEMENTS_UP, DONT_APPLY_BLOCKINGPATTERN, SEND_ELEMENTS);
				SchematicView::UpdateLinksZPos();
				if(bForceSelected)
				{
					SchematicWindow::vp_SelectedElements.removeOne(this);
				}
				delete SchematicView::vp_NewElementsForGroup;
				SchematicView::vp_NewElementsForGroup = nullptr;
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_FREE_SELECTED)
			{
				SchematicView::AddFreeSelectedElementsToGroupAPFS(SchematicWindow::vp_SelectedGroups.at(0), this);
			}
			else if(p_SelectedMenuItem->data() == MENU_REMOVE_FROM_GROUP)
			{
				bool bForceSelected = false;
				//
				if(!SchematicWindow::vp_SelectedElements.contains(this))
				{
					SchematicWindow::vp_SelectedElements.append(this);
					bForceSelected = true;
				}
				SchematicView::DetachSelectedAPFS();
				if(bForceSelected)
				{
					SchematicWindow::vp_SelectedElements.removeOne(this);
				}
			}
			else if(p_SelectedMenuItem->data() == MENU_CHANGE_BACKGROUND)
			{

			}
			TrySendBufferToServer;
		}
		SchematicWindow::ResetMenu();
	}
}
