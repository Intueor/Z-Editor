//== ВКЛЮЧЕНИЯ.
#include <math.h>
#include <QGraphicsSceneEvent>
#include "schematic-view.h"
#include "schematic-window.h"
#include "z-editor-defs.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
bool SchematicView::bLMousePressed = false;
int SchematicView::iXInt = SCH_INTERNAL_POS_UNCHANGED;
int SchematicView::iYInt = SCH_INTERNAL_POS_UNCHANGED;
CBSchematicViewFrameChanged SchematicView::pf_CBSchematicViewFrameChangedInt;
GraphicsPortItem* SchematicView::p_GraphicsPortItemActive = nullptr;
QPointF SchematicView::pntMouseClickMapped;
QPointF SchematicView::pntMouseMoveMapped;
bool SchematicView::bShiftAndLMBPressed = false;
QGraphicsRectItem* SchematicView::p_QGraphicsRectItemSelectionDash = nullptr;
QGraphicsRectItem* SchematicView::p_QGraphicsRectItemSelectionDot = nullptr;

//== ФУНКЦИИ КЛАССОВ.
//== Класс виджета обзора.
// Конструктор.
SchematicView::SchematicView(QWidget* parent) : QGraphicsView(parent)
{
	srand(time(NULL));
	setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
}

// Установка указателя кэлбэка изменения окна обзора.
void SchematicView::SetSchematicViewFrameChangedCB(CBSchematicViewFrameChanged pf_CBSchematicViewFrameChanged)
{
	pf_CBSchematicViewFrameChangedInt = pf_CBSchematicViewFrameChanged;
}

// Определение области видимости.
QRectF SchematicView::GetVisibleRect()
{
	QPointF pntA = mapToScene(0, 0);
	QPointF pntB = mapToScene(this->viewport()->width(), this->viewport()->height());
	return QRectF(pntA, pntB);
}

// Переопределение функции обработки событий колёсика.
void SchematicView::wheelEvent(QWheelEvent* p_Event)
{
	rScaleFactor = pow((double)2, -p_Event->delta() / 240.0);
	rFactor = transform().scale(rScaleFactor, rScaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
	if (rFactor < 0.1 || rFactor > 5)
		return;
	scale(rScaleFactor, rScaleFactor);
	if(pf_CBSchematicViewFrameChangedInt != nullptr)
	{
		pf_CBSchematicViewFrameChangedInt(GetVisibleRect());
	}
}

/// Переопределение функции обработки перемещения мыши.
void SchematicView::mouseMoveEvent(QMouseEvent* p_Event)
{
	if(bShiftAndLMBPressed & bLMousePressed)
	{
		QRectF oQRectF;
		//
		pntMouseMoveMapped = mapToScene(p_Event->x(), p_Event->y());
		if(pntMouseMoveMapped.x() < pntMouseClickMapped.x())
		{
			oQRectF.setX(pntMouseMoveMapped.x());
			oQRectF.setWidth(pntMouseClickMapped.x() - pntMouseMoveMapped.x());
		}
		else
		{
			oQRectF.setX(pntMouseClickMapped.x());
			oQRectF.setWidth(pntMouseMoveMapped.x() - pntMouseClickMapped.x());
		}
		if(pntMouseMoveMapped.y() < pntMouseClickMapped.y())
		{
			oQRectF.setY(pntMouseMoveMapped.y());
			oQRectF.setHeight(pntMouseClickMapped.y() - pntMouseMoveMapped.y());
		}
		else
		{
			oQRectF.setY(pntMouseClickMapped.y());
			oQRectF.setHeight(pntMouseMoveMapped.y() - pntMouseClickMapped.y());
		}
		p_QGraphicsRectItemSelectionDash->setRect(oQRectF);
		p_QGraphicsRectItemSelectionDot->setRect(oQRectF);
	}
	QGraphicsView::mouseMoveEvent(p_Event);
}

// Создание нового элемента и подготовка отсылки параметров.
GraphicsElementItem* SchematicView::CreateNewElementAPFS(char* p_chNameBase, QPointF a_pntMapped, unsigned long long ullIDGroup)
{
	PSchElementBase oPSchElementBase;
	unsigned char uchR = rand() % 255;
	unsigned char uchG = rand() % 255;
	unsigned char uchB = rand() % 255;
	QString strName = QString(p_chNameBase);
	GraphicsElementItem* p_GraphicsElementItem;
	//
	memset(&oPSchElementBase, 0, sizeof(oPSchElementBase));
	oPSchElementBase.oPSchElementVars.ullIDGroup = ullIDGroup;
	oPSchElementBase.bRequestGroupUpdate = (ullIDGroup != 0);
	oPSchElementBase.oPSchElementVars.ullIDInt = GenerateID();
	strName += ": " + QString::number(oPSchElementBase.oPSchElementVars.ullIDInt);
	CopyStrArray((char*)strName.toStdString().c_str(), oPSchElementBase.m_chName,
				 (unsigned int)strName.toStdString().size() + 1);
	SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
	oPSchElementBase.oPSchElementVars.oSchElementGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX = 0;
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY = 0;
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW = 275;
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH = 75;
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX = a_pntMapped.x();
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY = a_pntMapped.y();
	oPSchElementBase.oPSchElementVars.oSchElementGraph.uiObjectBkgColor = QColor(uchR, uchG, uchB).rgb();
	p_GraphicsElementItem = new GraphicsElementItem(&oPSchElementBase);
	MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsElementItem);
	p_GraphicsElementItem->setZValue(oPSchElementBase.oPSchElementVars.oSchElementGraph.dbObjectZPos);
	SchematicWindow::vp_Elements.push_front(p_GraphicsElementItem);
	MainWindow::p_SchematicWindow->oScene.update();
	MainWindow::p_Client->AddPocketToOutputBufferC(
				PROTO_O_SCH_ELEMENT_BASE, (char*)&oPSchElementBase, sizeof(PSchElementBase));
	return p_GraphicsElementItem;
}

// Переопределение функции обработки нажатия на кнопку мыши.
void SchematicView::mousePressEvent(QMouseEvent* p_Event)
{
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItem;
	QGraphicsItem* p_QGraphicsItem;
	QList<QGraphicsItem*> lp_QGraphicsItems = this->items();
	QList<QGraphicsItem*> lp_QGraphicsItemsHided;
	SafeMenu oSafeMenu;
	QAction* p_SelectedMenuItem;
	//
	if(!bShiftAndLMBPressed)
	{
		pntMouseClickMapped = mapToScene(p_Event->x(), p_Event->y());
		pntMouseMoveMapped = pntMouseClickMapped;
	}
	p_QGraphicsItem = MainWindow::p_SchematicWindow->oScene.itemAt(
				pntMouseClickMapped.x(), pntMouseClickMapped.y(), transform()); // Есть ли под курсором что-то...
	if(p_QGraphicsItem) // Если не на пустом месте...
	{
		for(int iF = 0; iF != lp_QGraphicsItems.count(); iF++) // Прячем все линки и добавляем в список.
		{
			p_QGraphicsItem = lp_QGraphicsItems.at(iF);
			if((p_QGraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI) & (p_QGraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_LINK))
			{
				lp_QGraphicsItemsHided.append(p_QGraphicsItem);
				p_QGraphicsItem->hide();
			}
		}
	}
	p_QGraphicsItem = MainWindow::p_SchematicWindow->oScene.itemAt(pntMouseClickMapped.x(), pntMouseClickMapped.y(), transform()); // Новый тест.
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		bLMousePressed = true;
		iXInt = iYInt = SCH_INTERNAL_POS_UNCHANGED;
		if(p_QGraphicsItem) goto gEx;
		if(p_Event->modifiers() == Qt::ControlModifier)
		{
			for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
			{
				p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iF);
				p_GraphicsElementItem->p_GraphicsFrameItem->hide();
				p_GraphicsElementItem->bSelected = false;
			}
			SchematicWindow::vp_SelectedElements.clear();
			for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
			{
				p_GraphicsGroupItem = SchematicWindow::vp_SelectedGroups.at(iF);
				p_GraphicsGroupItem->p_GraphicsFrameItem->hide();
				p_GraphicsGroupItem->bSelected = false;
			}
			SchematicWindow::vp_SelectedGroups.clear();
		}

	}
	else if(p_Event->button() == Qt::MouseButton::RightButton)
	{
		if(p_QGraphicsItem) goto gEx;
		oSafeMenu.addAction(m_chMenuCreateElement)->setData(MENU_CREATE_ELEMENT);
	}
gEx:if(!lp_QGraphicsItemsHided.isEmpty())
	{
		for(int iF = 0;  iF != lp_QGraphicsItemsHided.count(); iF++) // Показываем все ранее спрятянные линки.
		{
			lp_QGraphicsItemsHided.at(iF)->show();
		}
	}
	if(!oSafeMenu.actions().isEmpty())
	{
		p_SelectedMenuItem = oSafeMenu.exec(p_Event->globalPos());
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->data() == MENU_CREATE_ELEMENT)
			{
				CreateNewElementAPFS((char*)m_chNewElement, pntMouseClickMapped);
				MainWindow::p_Client->SendBufferToServer();
			}
		}
	}
	if(!bShiftAndLMBPressed)
	{
		QGraphicsView::mousePressEvent(p_Event);
		if((p_Event->modifiers() == Qt::ShiftModifier) & (p_Event->button() == Qt::MouseButton::LeftButton))
		{
			QRectF oQRectF = QRectF(pntMouseClickMapped.x(), pntMouseClickMapped.y(), 0, 0);
			bShiftAndLMBPressed = true;
			setDragMode(DragMode::NoDrag);
			viewport()->setCursor(Qt::CursorShape::CrossCursor);
			p_QGraphicsRectItemSelectionDash = MainWindow::p_SchematicWindow->oScene.addRect(oQRectF, SchematicWindow::oQPenSelectionDash);
			p_QGraphicsRectItemSelectionDash->setZValue(OVERMAX_NUMBER);
			p_QGraphicsRectItemSelectionDot = MainWindow::p_SchematicWindow->oScene.addRect(oQRectF, SchematicWindow::oQPenSelectionDot);
			p_QGraphicsRectItemSelectionDot->setZValue(OVERMAX_NUMBER - 1);
			MainWindow::p_SchematicWindow->oScene.update();
		}
	}
}

// Переопределение функции обработки отпускания кнопки мыши.
void SchematicView::mouseReleaseEvent(QMouseEvent* p_Event)
{
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		bLMousePressed = false;
		if((iXInt == SCH_INTERNAL_POS_UNCHANGED) & (iYInt == SCH_INTERNAL_POS_UNCHANGED))
		{
			goto gNE;
		}
		// При зафиксированном перетаскивании вида.
		if(pf_CBSchematicViewFrameChangedInt != nullptr)
		{
			pf_CBSchematicViewFrameChangedInt(GetVisibleRect());
		}
	}
gNE:QGraphicsView::mouseReleaseEvent(p_Event);
	if(bShiftAndLMBPressed & (p_Event->button() != Qt::MouseButton::RightButton))
	{
		bShiftAndLMBPressed = false;
		if(p_QGraphicsRectItemSelectionDash)
		{
			MainWindow::p_SchematicWindow->oScene.removeItem(p_QGraphicsRectItemSelectionDash);
			p_QGraphicsRectItemSelectionDash = nullptr;
			MainWindow::p_SchematicWindow->oScene.removeItem(p_QGraphicsRectItemSelectionDot);
			p_QGraphicsRectItemSelectionDot = nullptr;
			setDragMode(DragMode::ScrollHandDrag);
		}
	}
}

// Переопределение функции обработки перетаскивания вида.
void SchematicView::scrollContentsBy(int iX, int iY)
{
	if(bLMousePressed)
	{
		iXInt = iX;
		iYInt = iY;
	}
	QGraphicsView::scrollContentsBy(iX, iY);
}

/// Обновление Z-позиции линков.
void SchematicView::UpdateLinksZPos()
{
	for(int iF = 0; iF < SchematicWindow::vp_Links.count(); iF++)
	{
		GraphicsLinkItem* p_GraphicsLinkItem;
		//
		p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iF);
		p_GraphicsLinkItem->UpdateZPosition(p_GraphicsLinkItem);
	}
}

// Подготовка удаления графического элемента из сцены и группы, возврат флага на удаление группы элемента.
bool SchematicView::PrepareForRemoveElementFromScene(GraphicsElementItem* p_GraphicsElementItem)
{
	bool bGroupMustBeErased = false;
	GraphicsGroupItem* p_GraphicsGroupItem = p_GraphicsElementItem->p_GraphicsGroupItemRel;
	PSchGroupVars oPSchGroupVars;
	// Удалене связанных портов.
	GraphicsElementItem::RemovePortsByID(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt);
	// Если элемент в группе...
	if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
	{
		p_GraphicsGroupItem->vp_ConnectedElements.removeOne(p_GraphicsElementItem); // Удаление из группы.
		if(p_GraphicsGroupItem->vp_ConnectedElements.isEmpty())
		{
			bGroupMustBeErased = true;
		}
		else
		{
			GraphicsElementItem::UpdateGroupFrameByElements(p_GraphicsGroupItem); // При непустой группе - коррекция её фрейма.
			oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
					p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
			oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
			oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_FRAME;
			MainWindow::p_Client->AddPocketToOutputBufferC(
						PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars, sizeof(PSchGroupVars));
		}
	}
	// Удаление связанных линков.
	for(int iE = 0; iE < SchematicWindow::vp_Links.count(); iE++)
	{
		GraphicsLinkItem* p_GraphicsLinkItem;
		//
		p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iE);
		if(p_GraphicsLinkItem->p_GraphicsPortItemSrc->p_ParentInt == p_GraphicsElementItem)
		{
			SchematicWindow::vp_Links.removeAt(iE);
			MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsLinkItem);
			iE--;
		}
		else if(p_GraphicsLinkItem->p_GraphicsPortItemDst->p_ParentInt == p_GraphicsElementItem)
		{
			SchematicWindow::vp_Links.removeAt(iE);
			MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsLinkItem);
			iE--;
		}
	}
	return bGroupMustBeErased;
}

// Подготовка отсылки параметров и удаление группы.
void SchematicView::DeleteGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem)
{
	PSchGroupEraser oPSchGroupEraser;
	GraphicsElementItem* p_GraphicsElementItem;
	//
	oPSchGroupEraser.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
	MainWindow::p_Client->AddPocketToOutputBufferC(
				PROTO_O_SCH_GROUP_ERASE, (char*)&oPSchGroupEraser, sizeof(PSchGroupEraser));
	for(int iE = 0; iE < p_GraphicsGroupItem->vp_ConnectedElements.count(); iE++)
	{
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iE);
		p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
		PrepareForRemoveElementFromScene(p_GraphicsElementItem);
		SchematicWindow::vp_SelectedElements.removeOne(p_GraphicsElementItem);
		MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem);
	}
	SchematicWindow::vp_Groups.removeOne(p_GraphicsGroupItem);
	MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsGroupItem);
}

// Подготовка отсылки параметров и удаление элемента (а так же добавка его группы в лист).
void SchematicView::DeleteElementAPFS(GraphicsElementItem* p_GraphicsElementItem)
{
	PSchElementEraser oPSchElementEraser;
	PSchGroupEraser oPSchGroupEraser;
	//
	if(PrepareForRemoveElementFromScene(p_GraphicsElementItem))
	{
		oPSchGroupEraser.ullIDInt = p_GraphicsElementItem->p_GraphicsGroupItemRel->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
		MainWindow::p_Client->AddPocketToOutputBufferC(
					PROTO_O_SCH_GROUP_ERASE, (char*)&oPSchGroupEraser, sizeof(PSchGroupEraser));
		SchematicWindow::vp_Groups.removeOne(p_GraphicsElementItem->p_GraphicsGroupItemRel);
		MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem->p_GraphicsGroupItemRel);
	}
	else
	{
		oPSchElementEraser.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
		MainWindow::p_Client->AddPocketToOutputBufferC(
					PROTO_O_SCH_ELEMENT_ERASE, (char*)&oPSchElementEraser, sizeof(PSchElementEraser));
	}
	SchematicWindow::vp_Elements.removeOne(p_GraphicsElementItem);
	MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem);
}

// Остоединение выбранных элементов от группы и подготовка отправки всех изменеий на сервер.
bool SchematicView::DetachSelectedAPFS()
{
	bool bAction = false;
	GraphicsElementItem* p_GraphicsElementItem;
	QVector<GraphicsElementItem*> vp_SortedElements;
	PSchGroupVars oPSchGroupVars;
	QVector<GraphicsGroupItem*> vp_AffectedGroups;
	PSchElementVars oPSchElementVars;
	QVector<GraphicsElementItem*> vp_SelectedGroupedElements;
	//
	for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
	{
		GraphicsElementItem* pGraphicsElementItemSelected = SchematicWindow::vp_SelectedElements.at(iF);
		if(pGraphicsElementItemSelected->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
		{
			vp_SelectedGroupedElements.append(pGraphicsElementItemSelected);
		}
	}
	GraphicsGroupItem::SortElementsByZPos(vp_SelectedGroupedElements, nullptr, &vp_SortedElements);
	for(int iF = 0; iF != vp_SortedElements.count(); iF++)
	{
		p_GraphicsElementItem = vp_SortedElements.at(iF);
		if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
		{
			GraphicsGroupItem* p_GraphicsGroupItem = p_GraphicsElementItem->p_GraphicsGroupItemRel;
			//
			p_GraphicsGroupItem->vp_ConnectedElements.removeOne(p_GraphicsElementItem);
			if(p_GraphicsGroupItem->vp_ConnectedElements.isEmpty()) // Если уже пустая...
			{
				SchematicWindow::vp_Groups.removeOne(p_GraphicsElementItem->p_GraphicsGroupItemRel);
				MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem->p_GraphicsGroupItemRel);
				// Пустая группа удалится на сервере сама.
			}
			else
			{ // Иначе...
				if(!vp_AffectedGroups.contains(p_GraphicsGroupItem)) // Может быть игнор на случай остоединения более одного элемента от группы.
				{
					vp_AffectedGroups.append(p_GraphicsGroupItem); // Добавление в затронутые.
				}
				GraphicsElementItem::UpdateGroupFrameByElements(p_GraphicsGroupItem);
			}
			p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = 0;
			bAction = true;
		}
	}
	if(bAction)
	{
		for(int iG = 0; iG != vp_AffectedGroups.count(); iG++)
		{
			GraphicsGroupItem* p_GraphicsGroupItem = vp_AffectedGroups.at(iG);
			//
			oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
					p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
			oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
			oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_FRAME;
			MainWindow::p_Client->AddPocketToOutputBufferC(
						PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars, sizeof(PSchGroupVars));
		}
		for(int iF = 0; iF != vp_SortedElements.count(); iF++)
		{
			p_GraphicsElementItem = vp_SortedElements.at(iF);
			p_GraphicsElementItem->setZValue(SchematicWindow::dbObjectZPos);
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
			SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
			p_GraphicsElementItem->update();
			oPSchElementVars.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
			oPSchElementVars.ullIDGroup = 0;
			oPSchElementVars.oSchElementGraph.dbObjectZPos = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos;
			oPSchElementVars.oSchElementGraph.uchChangesBits = SCH_ELEMENT_BIT_GROUP | SCH_ELEMENT_BIT_ZPOS;
			MainWindow::p_Client->AddPocketToOutputBufferC(
						PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars, sizeof(PSchElementVars));
			SchematicView::UpdateLinksZPos();
		}
	}
	return bAction;
}

// Удаление выбранного и подготовка отправки по запросу.
void SchematicView::DeleteSelectedAPFS()
{
	QVector<GraphicsElementItem*> vp_SortedElements;
	QVector<GraphicsGroupItem*> vp_SortedGroups;
	//
	GraphicsGroupItem::SortGroupsByZPos(SchematicWindow::vp_SelectedGroups, nullptr, &vp_SortedGroups); // Сортировка групп в выборке.
	for(int iF = 0; iF != vp_SortedGroups.count(); iF++)
	{
		DeleteGroupAPFS(vp_SortedGroups.at(iF));
	}
	GraphicsGroupItem::SortElementsByZPos(SchematicWindow::vp_SelectedElements, nullptr, &vp_SortedElements); // Сортировка элементов в выборке.
	for(int iF = 0; iF != vp_SortedElements.count(); iF++)
	{
		DeleteElementAPFS(vp_SortedElements.at(iF));
	}
	SchematicWindow::vp_SelectedGroups.clear();
	SchematicWindow::vp_SelectedElements.clear();
}

// Переопределение функции обработки нажатия на клавиши.
void SchematicView::keyPressEvent(QKeyEvent* p_Event)
{
	if(SchematicView::p_GraphicsPortItemActive) // Если пришло во время активного порта...
	{
		switch (p_Event->key())
		{
			case Qt::Key_Delete:
			{
				GraphicsPortItem* p_GraphicsPortItemActiveLast = SchematicView::p_GraphicsPortItemActive;
				QGraphicsSceneMouseEvent oQGraphicsSceneMouseEvent(QEvent::MouseButtonRelease);
				//
				oQGraphicsSceneMouseEvent.setButton(Qt::LeftButton);
				SchematicView::p_GraphicsPortItemActive->mouseReleaseEvent(&oQGraphicsSceneMouseEvent);
				while(SchematicView::p_GraphicsPortItemActive)
				{
					MSleep(WAITING_FOR_INTERFACE);
				}
				DeleteLinkAPFS(p_GraphicsPortItemActiveLast->p_GraphicsLinkItemInt, NOT_FROM_ELEMENT, REMOVE_FROM_CLIENT);
				goto gS;
			}
		}
	}
	switch (p_Event->key())
	{
		case Qt::Key_Delete:
		{
			DeleteSelectedAPFS();
			break;
		}
	}
gS:	TrySendBufferToServer;
}

// Прикрепление позиции граф. порта к краям элемента.
DbPoint SchematicView::BindToInnerEdge(GraphicsElementItem* p_GraphicsElementItemNew, DbPoint oDbPortPosInitial)
{
	double dbXMin = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbX +
			p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
	double dbYMin = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectPos.dbY +
			p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
	double dbXMax = dbXMin +
			p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW;
	double dbYMax = dbYMin +
			p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH;
	double dbXMinDiff = oDbPortPosInitial.dbX - dbXMin; // Расстояние до левого края.
	double dbYMinDiff = oDbPortPosInitial.dbY - dbYMin; // Расстояние до верхнего края.
	double dbXMaxDiff = dbXMax - oDbPortPosInitial.dbX; // Расстояние до правого края.
	double dbYMaxDiff = dbYMax - oDbPortPosInitial.dbY; // Расстояние до нижнего края.
	bool bXToLeft; // Ближе к левому.
	bool bYToTop; // Ближе к верху.
	if(dbXMinDiff < dbXMaxDiff) bXToLeft = true; else bXToLeft = false;
	if(dbYMinDiff < dbYMaxDiff) bYToTop = true; else bYToTop = false;
	if(bXToLeft & bYToTop) // Если у левого верхнего края...
	{
		if(dbXMinDiff < dbYMinDiff) // Если к левому ближе, чем к верхнему...
		{
			oDbPortPosInitial.dbX = 0; // Прилипли к левому.
			oDbPortPosInitial.dbY -= dbYMin; // Оставляем вертикаль и переводим в координаты элемента.
		}
		else
		{
			oDbPortPosInitial.dbX -= dbXMin; // Оставляем горизонталь и переводим в координаты элемента.
			oDbPortPosInitial.dbY = 0; // Прилипли к верхнему.
		}
	}
	else if (!bXToLeft & !bYToTop) // Если у правого нижнего края...
	{
		if(dbXMaxDiff < dbYMaxDiff) // Если к правому ближе, чем к нижнему...
		{
			oDbPortPosInitial.dbX =
					p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW;// Прилипли к правому.
			oDbPortPosInitial.dbY -= dbYMin; // Оставляем вертикаль и переводим в координаты элемента.
		}
		else
		{
			oDbPortPosInitial.dbX -= dbXMin; // Оставляем горизонталь и переводим в координаты элемента.
			oDbPortPosInitial.dbY =
					p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH;// Прилипли к нижнему.
		}
	}
	else if(bXToLeft & !bYToTop) // Если у левого нижнего края...
	{
		if(dbXMinDiff < dbYMaxDiff) // Если к левому ближе, чем к нижнему...
		{
			oDbPortPosInitial.dbX = 0; // Прилипли к левому.
			oDbPortPosInitial.dbY -= dbYMin; // Оставляем вертикаль и переводим в координаты элемента.
		}
		else
		{
			oDbPortPosInitial.dbX -= dbXMin; // Оставляем горизонталь и переводим в координаты элемента.
			oDbPortPosInitial.dbY =
					p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH;// Прилипли к нижнему.
		}
	}
	else if(!bXToLeft & bYToTop) // Если у правого верхнего края...
	{
		if(dbXMaxDiff < dbYMinDiff) // Если к правому ближе, чем к верхнему...
		{
			oDbPortPosInitial.dbX =
					p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW;// Прилипли к правому.
			oDbPortPosInitial.dbY -= dbYMin; // Оставляем вертикаль и переводим в координаты элемента.
		}
		else
		{
			oDbPortPosInitial.dbX -= dbXMin; // Оставляем горизонталь и переводим в координаты элемента.
			oDbPortPosInitial.dbY = 0;// Прилипли к нижнему.
		}
	}
	return oDbPortPosInitial;
}

// Замена линка.
bool SchematicView::ReplaceLink(GraphicsLinkItem* p_GraphicsLinkItem,
							   GraphicsElementItem* p_GraphicsElementItemNew, bool bIsSrc, DbPoint oDbPortPos, bool bFromElement)
{
	PSchLinkBase oPSchLinkBase;
	GraphicsLinkItem* p_GraphicsLinkItemNew;
	//
	if(bIsSrc)
	{
		oPSchLinkBase.oPSchLinkVars.ullIDSrc = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
		oPSchLinkBase.oPSchLinkVars.ullIDDst = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst;
		//
		oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos = BindToInnerEdge(p_GraphicsElementItemNew, oDbPortPos);
		oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos =
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos;
	}
	else
	{
		oPSchLinkBase.oPSchLinkVars.ullIDSrc = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc;
		oPSchLinkBase.oPSchLinkVars.ullIDDst = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
		//
		oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos =
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos;
		oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos = BindToInnerEdge(p_GraphicsElementItemNew, oDbPortPos);
	}
	oPSchLinkBase.oPSchLinkVars.ushiSrcPort = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort;
	oPSchLinkBase.oPSchLinkVars.ushiDstPort = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort;
	//
	p_GraphicsLinkItemNew = new GraphicsLinkItem(&oPSchLinkBase);
	if(oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.uchChangesBits != SCH_LINK_BIT_INIT_ERROR)
	{
		MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsLinkItemNew);
	}
	else
	{
		delete p_GraphicsLinkItemNew;
		return false;
	}
	DeleteLinkAPFS(p_GraphicsLinkItem, bFromElement, REMOVE_FROM_CLIENT);
	SchematicWindow::vp_Links.push_front(p_GraphicsLinkItemNew);
	GraphicsLinkItem::UpdateZPosition(p_GraphicsLinkItemNew);
	MainWindow::p_Client->AddPocketToOutputBufferC(
				PROTO_O_SCH_LINK_BASE, (char*)&oPSchLinkBase, sizeof(PSchLinkBase));
	TrySendBufferToServer;
	MainWindow::p_This->RemoteUpdateSchView();
	return true;
}

// Удаление линка.
void SchematicView::DeleteLinkAPFS(GraphicsLinkItem* p_GraphicsLinkItem, bool bFromElement, bool bRemoveFromClient)
{
	PSchLinkEraser oPSchLinkEraser;
	//
	if(!bFromElement)
	{
		oPSchLinkEraser.ullIDSrc = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc;
		oPSchLinkEraser.ullIDDst = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst;
		oPSchLinkEraser.ushiSrcPort= p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort;
		oPSchLinkEraser.ushiDstPort = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort;
		MainWindow::p_Client->AddPocketToOutputBufferC(
					PROTO_O_SCH_LINK_ERASE, (char*)&oPSchLinkEraser, sizeof(PSchLinkEraser));
	}
	if(bRemoveFromClient)
	{
		SchematicWindow::vp_Ports.removeOne(p_GraphicsLinkItem->p_GraphicsPortItemSrc);
		SchematicWindow::vp_Ports.removeOne(p_GraphicsLinkItem->p_GraphicsPortItemDst);
		SchematicWindow::vp_Links.removeOne(p_GraphicsLinkItem);
		MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsLinkItem);
		MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsLinkItem->p_GraphicsPortItemSrc);
		MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsLinkItem->p_GraphicsPortItemDst);
	}
}

// Получение длины строки выбранным шрифтом в пикселях.
int SchematicView::GetStringWidthInPixels(const QFont& a_Font, QString& a_strText)
{
	QFontMetrics oQFontMetrics(a_Font);
	//
	return oQFontMetrics.width(a_strText);
}
