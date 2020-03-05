//== ВКЛЮЧЕНИЯ.
#include <QGraphicsProxyWidget>
#include <QApplication>
#include <QGraphicsSceneEvent>
#include <QBoxLayout>
#include "main-window.h"
#include "graphics-element-item.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"

//== МАКРОСЫ.
#define LOG_NAME                                "Element"
#define LOG_DIR_PATH							"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS_MULTIOBJECT(GraphicsElementItem)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(GraphicsElementItem)
QString GraphicsElementItem::strAddGroupName;
QVector<GraphicsElementItem*>* GraphicsElementItem::vp_NewElementsForGroup = nullptr;
DbPoint GraphicsElementItem::oDbPointInitial;
GraphicsLinkItem* GraphicsElementItem::p_GraphicsLinkItem;

//== ФУНКЦИИ КЛАССОВ.
//== Класс графического элемента.
// Конструктор.
GraphicsElementItem::GraphicsElementItem(PSchElementBase* p_PSchElementBase)
{
	LOG_CTRL_INIT_MULTIOBJECT;
	//
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
	p_QGroupBox->move(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX + 3,
					  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY);
	p_QGroupBox->setFixedSize(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW - 6,
							  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH - 3);
	p_QGraphicsProxyWidget->setFiltersChildEvents(true);
	p_QGraphicsProxyWidget->setParentItem(this);
	oQPalette.setBrush(QPalette::Background, oQBrush);
	p_QGroupBox->setPalette(oQPalette);
	//
	setPos(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX,
		   oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY);
	setZValue(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos);
	if(SchematicWindow::dbObjectZPos <= oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos)
	{
		SchematicWindow::dbObjectZPos = oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	oQBrush.setColor(QRgb(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uiObjectBkgColor));
	//
	p_GraphicsScalerItem = new GraphicsScalerItem(this);
	p_GraphicsScalerItem->setParentItem(this);
	p_GraphicsScalerItem->setPos(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX +
								 oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW,
								 oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY +
								 oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH);
	p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_ELEMENT, this);
	p_GraphicsFrameItem->hide();
	//
	SetBlockingPattern(this, oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy);
}

// Деструктор.
GraphicsElementItem::~GraphicsElementItem()
{
	delete p_QGroupBox;
	delete p_GraphicsFrameItem;
	delete p_GraphicsScalerItem;
}

// Удаление всех графических элементов портов с элемента по ID.
void GraphicsElementItem::RemovePortsByID(unsigned long long ullID)
{
	int iPC = SchematicWindow::vp_Ports.count();
	GraphicsPortItem* p_GraphicsPortItem;
	//
	for(int iP = 0; iP < iPC; iP++) // По всем граф. портам...
	{
		p_GraphicsPortItem = SchematicWindow::vp_Ports.at(iP);
		if((p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDDst == ullID ) ||
		   (p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDSrc == ullID)) // Если подключен...
		{
			MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsPortItem); // Удаление из сцены.
			SchematicWindow::vp_Ports.removeAt(iP); // Удаление указателя из списка.
			iPC--;
			iP--;
		}
	}
}

// Установка параметров элемента по структуре.
void GraphicsElementItem::SetElementParameters(GraphicsElementItem* p_GraphicsElementItem, PSchElementBase& a_PSchElementBase)
{
	p_GraphicsElementItem->oPSchElementBaseInt = a_PSchElementBase;
}

// Входное обновление параметров элемента по структуре.
void GraphicsElementItem::IncomingUpdateElementParameters(GraphicsElementItem* p_GraphicsElementItem, PSchElementVars& a_SchElementVars)
{
	// Фрейм.
	if(a_SchElementVars.oSchElementGraph.uchChangesBits & SCH_ELEMENT_BIT_FRAME)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] frame.");
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame =
				a_SchElementVars.oSchElementGraph.oDbObjectFrame;
		p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem,
											  SCH_UPDATE_ELEMENT_FRAME | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN, 0, 0, true);
	}
	// Позиция.
	if(a_SchElementVars.oSchElementGraph.uchChangesBits & SCH_ELEMENT_BIT_POS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] pos.");
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos =
				a_SchElementVars.oSchElementGraph.oDbObjectPos;
		p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem, SCH_UPDATE_ELEMENT_POS | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
	}
	// Отношение к группе.
	if(a_SchElementVars.oSchElementGraph.uchChangesBits & SCH_ELEMENT_BIT_GROUP)
	{
		GraphicsGroupItem* p_GraphicsGroupItem;
		//
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] group.");
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0) // Если элемент в группе...
		{
			if(a_SchElementVars.ullIDGroup == 0) // И пришёл запрос на ОТСОЕДИНЕНИЕ от группы...
			{
				p_GraphicsElementItem->p_GraphicsGroupItemRel->vp_ConnectedElements.removeOne(p_GraphicsElementItem);
				if(p_GraphicsElementItem->p_GraphicsGroupItemRel->vp_ConnectedElements.isEmpty())
				{ // Последний был - удаление группы.
					SchematicWindow::vp_Groups.removeOne(p_GraphicsElementItem->p_GraphicsGroupItemRel);
					MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem->p_GraphicsGroupItemRel);
				}
				else
				{ // Ещё остались...
					GraphicsElementItem::UpdateGroupFrameByElements(p_GraphicsElementItem->p_GraphicsGroupItemRel);
				}
				p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = 0;
				// Элемент не поднимается автоматически, т.к. при отсоединением Z-позиция приходит от отсоединившего клиента.
			}
		}
		else // Если не в группе, ДОБАВЛЕНИЕ... (Непосредственный перенос пока не реализуем).
		{
			for(int iF = 0; iF != SchematicWindow::vp_Groups.count(); iF++)
			{
				p_GraphicsGroupItem = SchematicWindow::vp_Groups.at(iF);
				if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt == a_SchElementVars.ullIDGroup)
				{
					p_GraphicsGroupItem->vp_ConnectedElements.append(p_GraphicsElementItem);
					p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = a_SchElementVars.ullIDGroup;
					p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem, SCH_UPDATE_MAIN);
					goto gFn;
				}
			}
		}
	}
	// Z-позиция.
gFn:if(a_SchElementVars.oSchElementGraph.uchChangesBits & SCH_ELEMENT_BIT_ZPOS)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] z-pos.");
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos =
				a_SchElementVars.oSchElementGraph.dbObjectZPos;
		p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem, SCH_UPDATE_ELEMENT_ZPOS | SCH_UPDATE_MAIN);
		if(SchematicWindow::dbObjectZPos <= p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos)
		{
			SchematicWindow::dbObjectZPos = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos +
					SCH_NEXT_Z_SHIFT;
		}
	}
	// Цвет подложки.
	if(a_SchElementVars.oSchElementGraph.uchChangesBits & SCH_ELEMENT_BIT_BKG_COLOR)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] color.");
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uiObjectBkgColor =
				a_SchElementVars.oSchElementGraph.uiObjectBkgColor;
		p_GraphicsElementItem->oQBrush.setColor(QRgb(p_GraphicsElementItem->
													 oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uiObjectBkgColor));
	}
	// Занятость.
	if(a_SchElementVars.oSchElementGraph.uchChangesBits & SCH_ELEMENT_BIT_BUSY)
	{
		LOG_P_2(LOG_CAT_I, "[" << QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName).toStdString() << "] busy.");
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy = a_SchElementVars.oSchElementGraph.bBusy;
		p_GraphicsElementItem->SetBlockingPattern(p_GraphicsElementItem, p_GraphicsElementItem->
												  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy);
	}
}

// Переопределение функции сообщения о вмещающем прямоугольнике.
QRectF GraphicsElementItem::boundingRect() const
{
	return QRectF(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX,
				  oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY,
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
	p_Painter->drawRect(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX,
						oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY,
						oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW,
						oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH);
}

// Переопределение функции шага событий элемента.
void GraphicsElementItem::advance(int iStep)
{
	iStep = iStep; // Заглушка.
}

// Поднятие элемента на первый план и подготовка отсылки по запросу.
void GraphicsElementItem::ElementToTopAPFS(GraphicsElementItem* p_Element, bool bAddElementGroupChange,
														   bool bAddBusyOrZPosToSending,
														   bool bBlokingPattern, bool bSend)
{
	PSchElementVars oPSchElementVars;
	//
	p_Element->setZValue(SchematicWindow::dbObjectZPos);
	p_Element->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
	SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
	p_Element->update();
	if(bSend)
	{
		oPSchElementVars.ullIDInt = p_Element->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
		if(bAddBusyOrZPosToSending)
		{
			oPSchElementVars.oSchElementGraph.bBusy = true;
			oPSchElementVars.oSchElementGraph.uchChangesBits = SCH_ELEMENT_BIT_BUSY;
		}
		else
		{
			oPSchElementVars.oSchElementGraph.uchChangesBits = SCH_ELEMENT_BIT_ZPOS;
			oPSchElementVars.oSchElementGraph.dbObjectZPos = p_Element->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos;
		}
		if(bAddElementGroupChange)
		{
			if(vp_NewElementsForGroup != nullptr)
			{
				if(vp_NewElementsForGroup->contains(p_Element))
				{
					oPSchElementVars.oSchElementGraph.uchChangesBits |= SCH_ELEMENT_BIT_GROUP;
					oPSchElementVars.ullIDGroup = p_Element->oPSchElementBaseInt.oPSchElementVars.ullIDGroup;
					vp_NewElementsForGroup->removeOne(p_Element);
				}
			}
		}
		MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars,
													   sizeof(oPSchElementVars));
	}
	p_Element->SetBlockingPattern(p_Element, bBlokingPattern);
	SchematicView::UpdateLinksZPos();
}

// Переопределение функции обработки нажатия мыши.
void GraphicsElementItem::mousePressEvent(QGraphicsSceneMouseEvent* p_Event)
{
	bool bLastSt;
	bool bInGroup;
	//
	if(oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy || MainWindow::bBlockingGraphics)
	{ //Если элемент блокирован занятостью или главным окном - отказ.
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		// Создание нового порта.
		if(p_Event->modifiers() == Qt::AltModifier)
		{
			PSchLinkBase oPSchLinkBase;
			//
			oDbPointInitial.dbX = p_Event->scenePos().x();
			oDbPointInitial.dbY = p_Event->scenePos().y();
			//
			oPSchLinkBase.oPSchLinkVars.ullIDSrc = this->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
			oPSchLinkBase.oPSchLinkVars.ullIDDst = oPSchLinkBase.oPSchLinkVars.ullIDSrc; // Временно, пока не перетянут на новый элемент.
			oPSchLinkBase.oPSchLinkVars.ushiSrcPort = DEFAULT_NEW_PORT;
			oPSchLinkBase.oPSchLinkVars.ushiDstPort = DEFAULT_NEW_PORT;
			// В координаты элемента.
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos = oDbPointInitial;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbX -=
					oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbY -=
					oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos = oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos =
					SchematicView::BindToInnerEdge(this, oDbPointInitial);
			// Создание замкнутого линка (пока что).
			p_GraphicsLinkItem = new GraphicsLinkItem(&oPSchLinkBase);
			if(oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.uchChangesBits != SCH_LINK_BIT_INIT_ERROR)
			{
				MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsLinkItem);
			}
			else
			{
				delete p_GraphicsLinkItem;
				p_GraphicsLinkItem = nullptr;
				goto gNL;
			}
			SchematicWindow::vp_Links.push_front(p_GraphicsLinkItem);
			GraphicsLinkItem::UpdateZPosition(p_GraphicsLinkItem);
			MainWindow::p_This->RemoteUpdateSchView();
			p_GraphicsLinkItem->p_GraphicsPortItemDst->mousePressEvent(p_Event);
			QGraphicsItem::mousePressEvent(p_Event);
			p_GraphicsLinkItem->p_GraphicsPortItemDst->p_GraphicsFrameItem->show(); // Зажигаем рамку.
			SchematicWindow::p_GraphicsFrameItemForPortFlash = p_GraphicsLinkItem->p_GraphicsPortItemDst->p_GraphicsFrameItem;
			return;
		}
		//==== РАБОТА С ВЫБОРКОЙ. ====
gNL:	bLastSt = bSelected; // Запоминаем предыдущее значение выбраности.
		if(p_Event->modifiers() == Qt::ControlModifier)
		{ // При удержании CTRL - инверсия флага выбраности.
			bSelected = !bSelected;
		}
		if(bSelected)
		{ // Если ВЫБРАЛИ...
			if(bLastSt != bSelected)
			{ // И раньше было не выбрано - добавление в вектор выбранных элементов.
				SchematicWindow::vp_SelectedElements.push_front(this);
				p_GraphicsFrameItem->show(); // Зажигаем рамку.
			}
			// СОРТИРОВКА.
			QVector<GraphicsElementItem*> vp_SortedElements;
			//
			GraphicsGroupItem::SortElementsByZPos(SchematicWindow::vp_SelectedElements, this, &vp_SortedElements); // Сортировка элементов в выборке.
			for(int iE = 0; iE != vp_SortedElements.count(); iE++) // Цикл по сортированным выбранным элементам.
			{
				GraphicsElementItem* p_GraphicsElementItem;
				//
				p_GraphicsElementItem = vp_SortedElements.at(iE);
				// Отправка наверх всех выбранных элементов кроме текущего (его потом, в последнюю очередь, над всеми).
				if(p_GraphicsElementItem != this)
				{
					bInGroup = (p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr); // Присутствие группы у элемента.
					if(bInGroup)
					{
						GraphicsGroupItem::GroupToTopAPFS(p_GraphicsElementItem->p_GraphicsGroupItemRel,
																		  SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, ADD_SEND_BUSY,
																		  DONT_ADD_SEND_FRAME, p_GraphicsElementItem,
																		  ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENTS); // Не поднимать элемент.
					}
					 // Текущий выбранный наверх, над группой.
					ElementToTopAPFS(p_GraphicsElementItem, DONT_SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_BUSY,
													 ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENT);
				}
			}
		}
		else
		{ // ОТМЕНИЛИ ВЫБОР...
			if(bLastSt != bSelected)
			{
				int iN;
				//
				iN = SchematicWindow::vp_SelectedElements.indexOf(this);
				if(iN != -1)
				{
					SchematicWindow::vp_SelectedElements.removeAt(iN);
					p_GraphicsFrameItem->hide(); // Гасим рамку.
				}
			}
		}
		bInGroup = (p_GraphicsGroupItemRel != nullptr); // Присутствие группы у элемента.
		if(bInGroup) // Если присутствует - поднятие, исключая текущий элемент.
		{
			GraphicsGroupItem::GroupToTopAPFS(p_GraphicsGroupItemRel, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, ADD_SEND_BUSY,
															  DONT_ADD_SEND_FRAME, this, ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENTS);
		}
		ElementToTopAPFS(this, DONT_SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_BUSY,
										 ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENT); // Если в группе - не отсылать.
	}
	else if(p_Event->button() == Qt::MouseButton::RightButton)
	{
		if(SchematicWindow::p_SafeMenu == nullptr)
		{
			SchematicWindow::p_SafeMenu = new SafeMenu;

			//================= СОСТАВЛЕНИЕ ПУНКТОВ МЕНЮ. =================//
			// Объект.
			QString strCaption;
			bool bNoSelection = SchematicWindow::vp_SelectedElements.isEmpty();
			//
			if(bNoSelection)
			{
				strCaption = QString(m_chElement) +
						" [" + QString(this->oPSchElementBaseInt.m_chName) + "]";
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
					strAddGroupName = QString(m_chMenuAddFreeSelected) + " [" +
							QString(SchematicWindow::vp_SelectedGroups.at(0)->oPSchGroupBaseInt.m_chName) + "]";
					SchematicWindow::p_SafeMenu->addAction(strAddGroupName)->setData(MENU_ADD_FREE_SELECTED);
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
	if(MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(p_GraphicsLinkItem != nullptr)
	{
		p_GraphicsLinkItem->p_GraphicsPortItemDst->mouseMoveEvent(p_Event);
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
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX += oQPointFRes.x();
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY += oQPointFRes.y();
				p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
			}
		}
	}
	oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX = x();
	oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY = y();
	UpdateSelected(this, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
}

// Отпускание элемента и подготовка отправки по запросу.
void GraphicsElementItem::ReleaseElementAPFS(GraphicsElementItem* p_GraphicsElementItem, bool bWithGroup, bool bWithPosition, bool bWithFrame)
{
	PSchElementVars oPSchElementVars;
	//
	if(p_GraphicsElementItem->oQBrush.style() == Qt::SolidPattern)
		return;
	if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
	{
		if(bWithGroup)
		{
			p_GraphicsElementItem->p_GraphicsGroupItemRel->ReleaseGroupAPFS(p_GraphicsElementItem->p_GraphicsGroupItemRel);
			return;
		}
	}
	p_GraphicsElementItem->SetBlockingPattern(p_GraphicsElementItem, false);
	oPSchElementVars.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
	oPSchElementVars.oSchElementGraph.dbObjectZPos = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos;
	oPSchElementVars.oSchElementGraph.bBusy = false;
	oPSchElementVars.oSchElementGraph.uchChangesBits = SCH_ELEMENT_BIT_ZPOS | SCH_ELEMENT_BIT_BUSY;
	if(bWithPosition)
	{
		oPSchElementVars.oSchElementGraph.uchChangesBits |= SCH_ELEMENT_BIT_POS;
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX = p_GraphicsElementItem->x();
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY = p_GraphicsElementItem->y();
		oPSchElementVars.oSchElementGraph.oDbObjectPos = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos;
	}
	if(bWithFrame)
	{
		oPSchElementVars.oSchElementGraph.uchChangesBits |= SCH_ELEMENT_BIT_FRAME;
		oPSchElementVars.oSchElementGraph.oDbObjectFrame = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame;
	}
	MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars,
												   sizeof(PSchElementVars));
}

// Добавление свободных элементов в группу и подготовка к отправке по запросу.
bool GraphicsElementItem::AddFreeSelectedElementsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
														 GraphicsElementItem* p_GraphicsElementItemInitial)
{
	GraphicsElementItem* p_GraphicsElementItem;
	bool bForceSelected = false;
	bool bAction;
	//
	vp_NewElementsForGroup = new QVector<GraphicsElementItem*>;
	if(p_GraphicsElementItemInitial != nullptr)
	{
		if(!SchematicWindow::vp_SelectedElements.contains(p_GraphicsElementItemInitial))
		{
			SchematicWindow::vp_SelectedElements.append(p_GraphicsElementItemInitial);
			bForceSelected = true;
		}
	}
	bAction = false;
	for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
	{
		p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iF);
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
		{
			if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt)
			{
				p_GraphicsGroupItem->vp_ConnectedElements.push_front(p_GraphicsElementItem);
				p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup =
						p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
				vp_NewElementsForGroup->append(p_GraphicsElementItem);
				bAction = true;
			}
		}
	}
	if(bAction)
	{
		GraphicsElementItem::UpdateGroupFrameByElements(p_GraphicsGroupItem);
		GraphicsGroupItem::GroupToTopAPFS(p_GraphicsGroupItem, SEND_GROUP, SEND_NEW_ELEMENTS_TO_GROUP, ADD_SEND_ZPOS, ADD_SEND_FRAME,
										  p_GraphicsElementItemInitial, ELEMENTS_BLOCKING_PATTERN_OFF, SEND_ELEMENTS);
		if(p_GraphicsElementItemInitial != nullptr)
		{
			ElementToTopAPFS(p_GraphicsElementItemInitial, SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_ZPOS,
							 ELEMENTS_BLOCKING_PATTERN_OFF, SEND_ELEMENT);
		}
		SchematicView::UpdateLinksZPos();
	}
	if(bForceSelected)
	{
		SchematicWindow::vp_SelectedElements.removeOne(p_GraphicsElementItemInitial);
	}
	delete vp_NewElementsForGroup;
	vp_NewElementsForGroup = nullptr;
	return bAction;
}

// Переопределение функции обработки отпускания мыши.
void GraphicsElementItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	//
	if(MainWindow::bBlockingGraphics)
	{
		return;
	}
	if(p_GraphicsLinkItem != nullptr)
	{
		p_GraphicsLinkItem->p_GraphicsPortItemDst->mouseReleaseEvent(p_Event);
		p_GraphicsLinkItem = nullptr;
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
					ReleaseElementAPFS(p_GraphicsElementItem);
				}
			}
		}
		ReleaseElementAPFS(this);
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
				vp_NewElementsForGroup = new QVector<GraphicsElementItem*>;
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
						vp_NewElementsForGroup->append(p_GraphicsElementItem);
					}
				}
				UpdateGroupFrameByElements(p_GraphicsGroupItem);
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
						p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_GROUP_BASE, (char*)&oPSchGroupBase, sizeof(PSchGroupBase));
				GraphicsGroupItem::SortGroupElementsToTopAPFS(p_GraphicsGroupItem, SEND_NEW_ELEMENTS_TO_GROUP, ADD_SEND_ZPOS,
														   nullptr, DONT_GET_SELECTED_ELEMENTS_UP, DONT_APPLY_BLOCKINGPATTERN, SEND_ELEMENTS);
				SchematicView::UpdateLinksZPos();
				if(bForceSelected)
				{
					SchematicWindow::vp_SelectedElements.removeOne(this);
				}
				delete vp_NewElementsForGroup;
				vp_NewElementsForGroup = nullptr;
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_FREE_SELECTED)
			{
				AddFreeSelectedElementsToGroupAPFS(SchematicWindow::vp_SelectedGroups.at(0), this);
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

// Установка блокировки на элемент.
void GraphicsElementItem::SetBlockingPattern(GraphicsElementItem* p_GraphicsElementItem, bool bValue)
{
	if(bValue)
	{
		p_GraphicsElementItem->oQBrush.setStyle(Qt::Dense4Pattern);
	}
	else
	{
		p_GraphicsElementItem->oQBrush.setStyle(Qt::SolidPattern);
	}
	p_GraphicsElementItem->UpdateSelected(p_GraphicsElementItem, SCH_UPDATE_MAIN);
}

// Обновление фрейма группы по геометрии включённых элементов.
void GraphicsElementItem::UpdateGroupFrameByElements(GraphicsGroupItem* p_GraphicsGroupItem)
{
	DbPoint oDbPointLeftTop;
	DbPoint oDbPointRightBottom;
	DbFrame oDbFrameResult;
	GraphicsElementItem* p_GraphicsElementItem;
	//
	p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(0);
	// Получаем крайние точки первого элемента в группе.
	oDbPointLeftTop.dbX =
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX +
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
	oDbPointLeftTop.dbY =
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY +
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
	oDbPointRightBottom.dbX = oDbPointLeftTop.dbX +
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW;
	oDbPointRightBottom.dbY = oDbPointLeftTop.dbY +
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH;
	// Цикл по остальным элементам для наращивания.
	for(int iF = 1; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++)
	{
		DbPoint oDbPointLeftTopTemp;
		DbPoint oDbPointRightBottomTemp;
		//
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iF);
		// Получаем крайние точки следующего элемента в группе.
		oDbPointLeftTopTemp.dbX =
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX +
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
		oDbPointLeftTopTemp.dbY =
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY +
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
		oDbPointRightBottomTemp.dbX = oDbPointLeftTopTemp.dbX +
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW;
		oDbPointRightBottomTemp.dbY = oDbPointLeftTopTemp.dbY +
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH;
		// Расширяем до показателей текущего элемента при зашкаливании по любой точке периметра.
		if(oDbPointLeftTop.dbX > oDbPointLeftTopTemp.dbX) oDbPointLeftTop.dbX = oDbPointLeftTopTemp.dbX;
		if(oDbPointLeftTop.dbY > oDbPointLeftTopTemp.dbY) oDbPointLeftTop.dbY = oDbPointLeftTopTemp.dbY;
		if(oDbPointRightBottom.dbX < oDbPointRightBottomTemp.dbX) oDbPointRightBottom.dbX = oDbPointRightBottomTemp.dbX;
		if(oDbPointRightBottom.dbY < oDbPointRightBottomTemp.dbY) oDbPointRightBottom.dbY = oDbPointRightBottomTemp.dbY;
	}
	// Припуски на рамку.
	oDbFrameResult.dbX = oDbPointLeftTop.dbX - 2;
	oDbFrameResult.dbY = oDbPointLeftTop.dbY - 20;
	oDbFrameResult.dbW = oDbPointRightBottom.dbX - oDbPointLeftTop.dbX + 4;
	oDbFrameResult.dbH = oDbPointRightBottom.dbY - oDbPointLeftTop.dbY + 22;
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame = oDbFrameResult;
	p_GraphicsGroupItem->UpdateSelected(p_GraphicsGroupItem, SCH_UPDATE_GROUP_FRAME | SCH_UPDATE_MAIN);
}

// Обновление выбранных граф. объектов.
void GraphicsElementItem::UpdateSelected(GraphicsElementItem* p_GraphicsElementItem, unsigned short ushBits,
										 GraphicsPortItem* p_GraphicsPortItem,
										 GraphicsLinkItem* p_GraphicsLinkItem, bool bIsIncoming)
{
	GraphicsPortItem* p_GraphicsPortItemInt;
	PSchLinkVars* p_SchLinkVars;
	QGraphicsItem* p_GraphicsItem;
	//
	if(ushBits & SCH_UPDATE_ELEMENT_POS)
	{
		p_GraphicsElementItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX,
			   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY);
	}
	if(ushBits & SCH_UPDATE_ELEMENT_ZPOS)
	{
		p_GraphicsElementItem->setZValue(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos);
		SchematicView::UpdateLinksZPos();
	}
	if(ushBits & SCH_UPDATE_ELEMENT_FRAME)
	{
		if(!bIsIncoming)
		{
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW +=
					p_GraphicsElementItem->oDbPointDimIncrements.dbX;
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH +=
					p_GraphicsElementItem->oDbPointDimIncrements.dbY;
		}
		QList<QGraphicsItem*> lp_Items = p_GraphicsElementItem->childItems();
		int iCn = lp_Items.count();
		for(int iC = 0; iC < iCn; iC++)
		{
			p_GraphicsItem = lp_Items.at(iC);
			if(p_GraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI)
			{
				if(p_GraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_PORT)
				{
					DbFrame* p_DbFrameEl;
					//
					p_DbFrameEl = &p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame;
					p_GraphicsPortItemInt = (GraphicsPortItem*)p_GraphicsItem;
					if(p_GraphicsPortItemInt->bIsSrc)
					{
						DbPoint* p_DbPointSrcPos;
						//
						p_DbPointSrcPos = &p_GraphicsPortItemInt->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos;
						if(p_DbPointSrcPos->dbX > p_DbFrameEl->dbW)
						{
							p_DbPointSrcPos->dbX = p_DbFrameEl->dbW;
						}
						else
						{
							if(!bIsIncoming)
							{
								if((p_DbPointSrcPos->dbX > 0) && (p_DbPointSrcPos->dbY > 0))
								{
									p_DbPointSrcPos->dbX += p_GraphicsElementItem->oDbPointDimIncrements.dbX;
								}
							}
							else
							{
								if(p_DbPointSrcPos->dbX > 0)
								{
									p_DbPointSrcPos->dbY = 0;
								}
								if(p_DbPointSrcPos->dbY > 0)
								{
									p_DbPointSrcPos->dbX = 0;
								}
							}
						}
						if(p_DbPointSrcPos->dbY > p_DbFrameEl->dbH)
						{
							p_DbPointSrcPos->dbY = p_DbFrameEl->dbH;
						}
						else
						{
							if(!bIsIncoming)
							{
								if((p_DbPointSrcPos->dbX > 0) && (p_DbPointSrcPos->dbY > 0))
								{
									p_DbPointSrcPos->dbY += p_GraphicsElementItem->oDbPointDimIncrements.dbY;
								}
							}
							else
							{
								if(p_DbPointSrcPos->dbX > 0)
								{
									p_DbPointSrcPos->dbY = 0;
								}
								if(p_DbPointSrcPos->dbY > 0)
								{
									p_DbPointSrcPos->dbX = 0;
								}
							}
						}
						p_GraphicsPortItemInt->setPos(p_DbFrameEl->dbX + p_DbPointSrcPos->dbX, p_DbFrameEl->dbY + p_DbPointSrcPos->dbY);
					}
					else
					{
						DbPoint* p_DbPointDstPos;
						//
						p_DbPointDstPos = &p_GraphicsPortItemInt->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos;
						if(p_DbPointDstPos->dbX > p_DbFrameEl->dbW)
						{
							p_DbPointDstPos->dbX = p_DbFrameEl->dbW;
						}
						else
						{
							if(!bIsIncoming)
							{
								if((p_DbPointDstPos->dbX > 0) && (p_DbPointDstPos->dbY > 0))
								{
									p_DbPointDstPos->dbX += p_GraphicsElementItem->oDbPointDimIncrements.dbX;
								}
							}
							else
							{
								if(p_DbPointDstPos->dbX > 0)
								{
									p_DbPointDstPos->dbY = 0;
								}
								if(p_DbPointDstPos->dbY > 0)
								{
									p_DbPointDstPos->dbX = 0;
								}
							}
						}
						if(p_DbPointDstPos->dbY > p_DbFrameEl->dbH)
						{
							p_DbPointDstPos->dbY = p_DbFrameEl->dbH;
						}
						else
						{
							if(!bIsIncoming)
							{
								if((p_DbPointDstPos->dbX > 0) && (p_DbPointDstPos->dbY > 0))
								{
									p_DbPointDstPos->dbY += p_GraphicsElementItem->oDbPointDimIncrements.dbY;
								}
							}
							else
							{
								if(p_DbPointDstPos->dbX > 0)
								{
									p_DbPointDstPos->dbY = 0;
								}
								if(p_DbPointDstPos->dbY > 0)
								{
									p_DbPointDstPos->dbX = 0;
								}
							}
						}
						p_GraphicsPortItemInt->setPos(p_DbFrameEl->dbX + p_DbPointDstPos->dbX, p_DbFrameEl->dbY + p_DbPointDstPos->dbY);
					}
				}
			}
		}
		WidgetsThrAccess::p_ConnGraphicsElementItem = p_GraphicsElementItem;
		ThrUiAccessET(MainWindow::p_WidgetsThrAccess, ElementGroupBoxSizeSet);
		p_GraphicsElementItem->p_GraphicsScalerItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
															oSchElementGraph.oDbObjectFrame.dbX +
															p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
															oSchElementGraph.oDbObjectFrame.dbW,
															p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
															oSchElementGraph.oDbObjectFrame.dbY +
															p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
															oSchElementGraph.oDbObjectFrame.dbH);
		p_GraphicsElementItem->p_GraphicsScalerItem->update();
		SchematicWindow::p_QGraphicsScene->update();
	}
	if(ushBits & SCH_UPDATE_PORTS_POS)
	{
		QList<QGraphicsItem*> lp_Items = p_GraphicsElementItem->childItems();
		int iCn = lp_Items.count();
		for(int iC = 0; iC < iCn; iC++)
		{
			p_GraphicsItem = lp_Items.at(iC);
			if(p_GraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI)
			{
				if(p_GraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_PORT)
				{
					p_GraphicsPortItemInt = (GraphicsPortItem*)p_GraphicsItem;
					if(p_GraphicsPortItemInt->p_ParentInt == p_GraphicsElementItem)
					{
						p_SchLinkVars = p_GraphicsPortItemInt->p_PSchLinkVarsInt;
						if(p_SchLinkVars->ullIDSrc == p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
						{
							p_GraphicsPortItemInt->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
														  oSchElementGraph.oDbObjectFrame.dbX +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLinkGraph.oDbSrcPortGraphPos.dbX,
														  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
														  oSchElementGraph.oDbObjectFrame.dbY +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLinkGraph.oDbSrcPortGraphPos.dbY);
							p_GraphicsPortItemInt->update();
						}
						if(p_SchLinkVars->ullIDDst == p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
						{
							p_GraphicsPortItemInt->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
														  oSchElementGraph.oDbObjectFrame.dbX +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLinkGraph.oDbDstPortGraphPos.dbX,
														  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
														  oSchElementGraph.oDbObjectFrame.dbY +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLinkGraph.oDbDstPortGraphPos.dbY);
							p_GraphicsPortItemInt->update();
						}
					}
				}
			}
		}
	}
	if(ushBits & SCH_UPDATE_PORT_SRC_POS)
	{
		p_GraphicsPortItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX,
								   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY);
		p_GraphicsPortItem->update();
	}
	if(ushBits & SCH_UPDATE_PORT_DST_POS)
	{
		p_GraphicsPortItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX,
								   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY);
		p_GraphicsPortItem->update();
	}
	if(ushBits & SCH_UPDATE_LINKS_POS)
	{
		for(int iF = 0; iF < SchematicWindow::vp_Links.count(); iF++)
		{
			GraphicsLinkItem* p_GraphicsLinkItem;
			//
			p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iF);
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc ==
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				p_GraphicsLinkItem->UpdatePosition(p_GraphicsLinkItem);
				p_GraphicsLinkItem->update();
			}
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				p_GraphicsLinkItem->UpdatePosition(p_GraphicsLinkItem);
				p_GraphicsLinkItem->update();
			}
		}
	}
	if(ushBits & SCH_UPDATE_LINK_POS)
	{
		p_GraphicsLinkItem->UpdatePosition(p_GraphicsLinkItem);
		p_GraphicsLinkItem->update();
	}
	if(ushBits & SCH_UPDATE_GROUP)
	{
		if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
		{
			if(!p_GraphicsElementItem->p_GraphicsGroupItemRel->vp_ConnectedElements.isEmpty())
			{
				UpdateGroupFrameByElements(p_GraphicsElementItem->p_GraphicsGroupItemRel);
			}
		}
	}
	if(ushBits & SCH_UPDATE_MAIN)
	{
#ifdef WIN32
		MainWindow::p_SchematicWindow->UpdateScene();
#else
		p_GraphicsElementItem->update();
#endif

	}
}

// Добавление объекта порта в лист.
void GraphicsElementItem::AddPort(GraphicsLinkItem* p_GraphicsElementItem, bool bSrc, GraphicsElementItem* p_Parent)
{
	SchematicWindow::vp_Ports.append(new GraphicsPortItem(p_GraphicsElementItem, bSrc, p_Parent));
}
