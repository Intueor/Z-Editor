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
	p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_GROUP, nullptr, this);
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
	SchematicView::SetGroupBlockingPattern(this, oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy);
}

// Деструктор.
GraphicsGroupItem::~GraphicsGroupItem()
{
	delete p_GraphicsFrameItem;
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
}

// Переопределение функции шага событий группы.
void GraphicsGroupItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}

// Переопределение функции обработки нажатия мыши.
void GraphicsGroupItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	bool bLastSt;
	//
	if(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{ //Если группа блокирована занятостью, смещением выборки или главным окном - отказ.
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		//==== РАБОТА С ВЫБОРКОЙ. ====
		bLastSt = bSelected; // Запоминаем предыдущее значение выбраности.
		if(p_Event->modifiers() == Qt::ControlModifier)
		{ // При удержании CTRL - инверсия флага выбраности.
			bSelected = !bSelected;
		}
		if(bSelected)// ВЫБРАЛИ...
			SchematicView::SelectGroup(this, bLastSt);
		else // ОТМЕНИЛИ ВЫБОР...
			SchematicView::DeselectGroup(this, bLastSt);
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
			bool bNoSelection = SchematicWindow::vp_SelectedGroups.isEmpty();
			//
			if(bNoSelection)
			{
				strCaption = QString(m_chGroup) +
						" [" + QString(oPSchGroupBaseInt.m_chName) + "]";
			}
			else
			{
				strCaption = "Выборка групп";
			}
			SchematicWindow::p_SafeMenu->setMinimumWidth(SchematicView::GetStringWidthInPixels(SchematicWindow::p_SafeMenu->font(), strCaption) + 50);
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
			// Расформировать.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuDisband))->setData(MENU_DISBAND);
			// Создать элемент в группе.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuAddElement))->setData(MENU_ADD_ELEMENT);
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
					SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuAddFreeSelected))->setData(MENU_ADD_FREE_SELECTED);
				}
			}
			// Цвет фона.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuBackground))->setData(MENU_CHANGE_BACKGROUND);
		}
	}
	TrySendBufferToServer;
	QGraphicsItem::mousePressEvent(p_Event);
}

// Переопределение функции обработки перемещения мыши.
void GraphicsGroupItem::mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	QPointF oQPointFInit;
	QPointF oQPointFRes;
	//
	if(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	oQPointFInit = pos();
	QGraphicsItem::mouseMoveEvent(p_Event);
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
				SchematicView::MoveGroup(p_GraphicsGroupItem, oQPointFRes);
			}
		}
	}
	SchematicView::MoveGroup(this, oQPointFRes, false);
}

// Переопределение функции обработки отпускания мыши.
void GraphicsGroupItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	if(oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(bSelected)
		{
			QVector<GraphicsGroupItem*> vp_SortedGroups;
			//
			SchematicView::SortGroupsByZPos(SchematicWindow::vp_SelectedGroups, this, &vp_SortedGroups); // Сортировка групп в выборке.
			for(int iE = 0; iE != vp_SortedGroups.count(); iE ++)
			{
				GraphicsGroupItem* p_GraphicsGroupItem;
				//
				p_GraphicsGroupItem = vp_SortedGroups.at(iE);
				if(p_GraphicsGroupItem != this)
				{
					SchematicView::ReleaseGroupAPFS(p_GraphicsGroupItem);
				}
			}
		}
		SchematicView::ReleaseGroupAPFS(this);
		TrySendBufferToServer;
	}
	QGraphicsItem::mouseReleaseEvent(p_Event);
	if(SchematicWindow::p_SafeMenu != nullptr)
	{
		QAction* p_SelectedMenuItem;
		Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog;
		PSchGroupName oPSchGroupName;
		char m_chName[SCH_OBJ_NAME_STR_LEN];
		//================= ВЫПОЛНЕНИЕ ПУНКТОВ МЕНЮ. =================//
		p_SelectedMenuItem = SchematicWindow::p_SafeMenu->exec(QCursor::pos());
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->data() == MENU_RENAME)
			{
				CopyStrArray(oPSchGroupBaseInt.m_chName, m_chName, SCH_OBJ_NAME_STR_LEN);
				p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Имя группы", m_chName, SCH_OBJ_NAME_STR_LEN);
				if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
				{
					memset(&oPSchGroupName, 0, sizeof(oPSchGroupName));
					CopyStrArray(m_chName, oPSchGroupName.m_chName, SCH_OBJ_NAME_STR_LEN);
					CopyStrArray(m_chName, oPSchGroupBaseInt.m_chName, SCH_OBJ_NAME_STR_LEN);
					oPSchGroupName.ullIDInt = oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
					MainWindow::p_Client->SendToServerImmediately(PROTO_O_SCH_GROUP_NAME, (char*)&oPSchGroupName,
																  sizeof(oPSchGroupName));
					p_QLabel->setText(oPSchGroupName.m_chName);
					SchematicWindow::p_MainWindow->p_SchematicWindow->update();
				}
				p_Set_Proposed_String_Dialog->deleteLater();
			}
			else if(p_SelectedMenuItem->data() == MENU_DELETE)
			{
				if(!SchematicWindow::vp_SelectedGroups.contains(this))
				{
					SchematicWindow::vp_SelectedGroups.append(this);
				}
				SchematicView::DeleteSelectedAPFS();
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_FREE_SELECTED)
			{
				SchematicView::AddFreeSelectedElementsToGroupAPFS(this);
			}
			else if(p_SelectedMenuItem->data() == MENU_DISBAND)
			{
				bool bForceSelected = false;
				QVector<GraphicsElementItem*> vp_SortedElements;
				GraphicsGroupItem* p_GraphicsGroupItem;
				//
				if(!SchematicWindow::vp_SelectedGroups.contains(this))
				{
					SchematicWindow::vp_SelectedGroups.append(this);
					bForceSelected = true;
				}
				for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
				{
					p_GraphicsGroupItem = SchematicWindow::vp_SelectedGroups.at(iF);
					SchematicView::SortElementsByZPos(p_GraphicsGroupItem->vp_ConnectedElements, nullptr, &vp_SortedElements);
					for(int iF = 0; iF != vp_SortedElements.count(); iF++)
					{
						GraphicsElementItem* p_GraphicsElementItem = vp_SortedElements.at(iF);
						PSchElementVars oPSchElementVars;
						//
						p_GraphicsElementItem->setZValue(SchematicWindow::dbObjectZPos);
						p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
						SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
						p_GraphicsElementItem->update();
						oPSchElementVars.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
						oPSchElementVars.ullIDGroup = 0;
						oPSchElementVars.oSchElementGraph.dbObjectZPos =
								p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos;
						oPSchElementVars.oSchElementGraph.uchChangesBits = SCH_ELEMENT_BIT_GROUP | SCH_ELEMENT_BIT_ZPOS;
						p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
						p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = 0;
						MainWindow::p_Client->AddPocketToOutputBufferC(
									PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars, sizeof(PSchElementVars));
						SchematicView::UpdateLinksZPos();
					}
					SchematicWindow::vp_Groups.removeOne(p_GraphicsGroupItem);
					MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsGroupItem);
					vp_SortedElements.clear();
				}
				if(bForceSelected)
				{
					SchematicWindow::vp_SelectedGroups.removeOne(this);
				}
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_ELEMENT)
			{
				GraphicsElementItem* p_GraphicsElementItem;
				//
				p_GraphicsElementItem = SchematicView::CreateNewElementAPFS((char*)m_chNewElement,
																  mapToScene(p_Event->pos()), oPSchGroupBaseInt.oPSchGroupVars.ullIDInt);
				vp_ConnectedElements.push_front(p_GraphicsElementItem);
				p_GraphicsElementItem->p_GraphicsGroupItemRel = this;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
				SchematicView::UpdateGroupFrameByElements(this);
				SchematicView::GroupToTopAPFS(this, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, ADD_SEND_ZPOS, ADD_SEND_FRAME,
														  nullptr, ELEMENTS_BLOCKING_PATTERN_OFF, SEND_ELEMENTS);
				SchematicView::UpdateLinksZPos();
			}
			else if(p_SelectedMenuItem->data() == MENU_CHANGE_BACKGROUND)
			{

			}
		}
		TrySendBufferToServer;
		SchematicWindow::ResetMenu();
	}
}


