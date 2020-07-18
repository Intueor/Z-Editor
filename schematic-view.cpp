//== ВКЛЮЧЕНИЯ.
#include <math.h>
#include <QGraphicsSceneEvent>
#include <QBoxLayout>
#include <QGraphicsProxyWidget>
#include "schematic-view.h"
#include "schematic-window.h"
#include "z-editor-defs.h"
#include "../Z-Hub/Dialogs/set_proposed_string_dialog.h"
#include "../Z-Hub/z-hub-defs.h"

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
QVector<GraphicsElementItem*>* SchematicView::vp_NewElementsForGroup = nullptr;
GraphicsLinkItem* SchematicView::p_GraphicsLinkItemNew = nullptr;
bool SchematicView::bPortAltPressed;
bool SchematicView::bPortLMBPressed;
DbPoint SchematicView::oDbPointPortRB;
DbPoint SchematicView::oDbPointPortCurrent;
DbPoint SchematicView::oDbPointPortOld;
DbPoint SchematicView::oDbPointPortInitialClick;
bool SchematicView::bPortFromElement;
bool SchematicView::bPortMenuExecuted = false;

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
	QPointF pntA = SchematicWindow::p_SchematicView->mapToScene(0, 0);
	QPointF pntB = SchematicWindow::p_SchematicView->mapToScene(
				SchematicWindow::p_SchematicView->viewport()->width(), SchematicWindow::p_SchematicView->viewport()->height());
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
	unsigned char uchA = 255;
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
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX = a_pntMapped.x();
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY = a_pntMapped.y();
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW = 275;
	oPSchElementBase.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH = 75;
        oPSchElementBase.oPSchElementVars.oSchElementGraph.uiObjectBkgColor = QColor(uchR, uchG, uchB, uchA).rgba();
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
		SchematicWindow::p_SafeMenu = new SafeMenu;
		SchematicWindow::p_SafeMenu->addAction(m_chMenuCreateElement)->setData(MENU_CREATE_ELEMENT);
	}
gEx:if(!lp_QGraphicsItemsHided.isEmpty())
	{
		for(int iF = 0;  iF != lp_QGraphicsItemsHided.count(); iF++) // Показываем все ранее спрятянные линки.
		{
			lp_QGraphicsItemsHided.at(iF)->show();
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
	QAction* p_SelectedMenuItem;
	//
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
			QList<QGraphicsItem*> lp_QGraphicsItems;
			//
			MainWindow::p_SchematicWindow->oScene.removeItem(p_QGraphicsRectItemSelectionDash);
			p_QGraphicsRectItemSelectionDash = nullptr;
			lp_QGraphicsItems = MainWindow::p_SchematicWindow->oScene.items();
			for(int iF = 0; iF != lp_QGraphicsItems.count(); iF++)
			{
				QGraphicsItem* p_GraphicsItem = lp_QGraphicsItems.at(iF);
				QPointF oQPointTL;
				QPointF oQPointBR;
				GraphicsElementItem* p_GraphicsElementItem = nullptr;
				GraphicsGroupItem* p_GraphicsGroupItem = nullptr;
				//
				if(p_GraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI)
				{
					if(p_GraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_ELEMENT)
					{
						p_GraphicsElementItem = (GraphicsElementItem*)p_GraphicsItem;
					}
					else if(p_GraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_GROUP)
					{
						p_GraphicsGroupItem = (GraphicsGroupItem*)p_GraphicsItem;
					}
				}
				if((p_GraphicsElementItem != nullptr) | (p_GraphicsGroupItem != nullptr))
				{
					oQPointTL = p_QGraphicsRectItemSelectionDot->rect().topLeft();
					oQPointBR = p_QGraphicsRectItemSelectionDot->rect().bottomRight();
					if(p_GraphicsElementItem)
					{
						if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX > oQPointTL.x()) &
						   (p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY > oQPointTL.y()) &
						   ((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX +
							 p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW) < oQPointBR.x()) &
						   ((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY +
							 p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH) < oQPointBR.y()))
						{
							if(!p_GraphicsElementItem->bSelected)
							{
								SchematicWindow::vp_SelectedElements.push_front(p_GraphicsElementItem);
								p_GraphicsElementItem->p_GraphicsFrameItem->show(); // Зажигаем рамку.
								p_GraphicsElementItem->bSelected = true;
							}
						}
					}
					else
					{
						if((p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX > oQPointTL.x()) &
						   (p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY > oQPointTL.y()) &
						   ((p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX +
							 p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW) < oQPointBR.x()) &
						   ((p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY +
							 p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH) < oQPointBR.y()))
						{
							if(!p_GraphicsGroupItem->bSelected)
							{
								SchematicWindow::vp_SelectedGroups.push_front(p_GraphicsGroupItem);
								p_GraphicsGroupItem->p_GraphicsFrameItem->show(); // Зажигаем рамку.
								p_GraphicsGroupItem->bSelected = true;
							}
						}
					}
				}
			}
			MainWindow::p_SchematicWindow->oScene.removeItem(p_QGraphicsRectItemSelectionDot);
			p_QGraphicsRectItemSelectionDot = nullptr;
			setDragMode(DragMode::ScrollHandDrag);
#ifndef WIN32
			QMouseEvent oEvent(QEvent::Type::MouseMove,
							   MainWindow::p_SchematicWindow->p_SchematicView->mapFromGlobal(QCursor::pos()), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
			MainWindow::p_SchematicWindow->p_SchematicView->mouseMoveEvent(&oEvent);
#endif
		}
	}
	if(SchematicWindow::p_SafeMenu)
	{
		p_SelectedMenuItem = SchematicWindow::p_SafeMenu->exec(p_Event->globalPos());
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->data() == MENU_CREATE_ELEMENT)
			{
				CreateNewElementAPFS((char*)m_chNewElement, pntMouseClickMapped);
				MainWindow::p_Client->SendBufferToServer();
			}
		}
		SchematicWindow::ResetMenu();
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
		UpdateLinkZPositionByElements(p_GraphicsLinkItem);
	}
}

// Подготовка удаления графического элемента из сцены и группы, возврат флага на удаление группы элемента.
void SchematicView::PrepareForRemoveElementFromScene(GraphicsElementItem* p_GraphicsElementItem)
{
	GraphicsGroupItem* p_GraphicsGroupItemRel = p_GraphicsElementItem->p_GraphicsGroupItemRel;
	PSchGroupVars oPSchGroupVars;
	// Удалене связанных портов.
	RemovePortsByID(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt);
	// Если элемент в группе...
	if(p_GraphicsGroupItemRel != nullptr)
	{
		p_GraphicsGroupItemRel->vp_ConnectedElements.removeOne(p_GraphicsElementItem); // Удаление из группы.
		if(!GroupCheckEmptyAndRemoveRecursively(p_GraphicsGroupItemRel))
		{
			UpdateGroupFrameByContentRecursively(p_GraphicsGroupItemRel); // При непустой группе - коррекция её фрейма.
			oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
					p_GraphicsGroupItemRel->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
			oPSchGroupVars.ullIDInt = p_GraphicsGroupItemRel->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
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
	//
	PrepareForRemoveElementFromScene(p_GraphicsElementItem);
		oPSchElementEraser.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
		MainWindow::p_Client->AddPocketToOutputBufferC(
					PROTO_O_SCH_ELEMENT_ERASE, (char*)&oPSchElementEraser, sizeof(PSchElementEraser));
	SchematicWindow::vp_Elements.removeOne(p_GraphicsElementItem);
	MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem);
}

// Остоединение выбранного от группы и подготовка отправки всех изменеий на сервер.
bool SchematicView::DetachSelectedAPFS()
{
	bool bAction = false;
	GraphicsElementItem* p_GraphicsElementItem;
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
	for(int iF = 0; iF != vp_SelectedGroupedElements.count(); iF++)
	{
		p_GraphicsElementItem = vp_SelectedGroupedElements.at(iF);
		if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
		{
			GraphicsGroupItem* p_GraphicsGroupItem = p_GraphicsElementItem->p_GraphicsGroupItemRel;
			//
			p_GraphicsGroupItem->vp_ConnectedElements.removeOne(p_GraphicsElementItem);
			if(!GroupCheckEmptyAndRemoveRecursively(p_GraphicsGroupItem))
			{
				if(!vp_AffectedGroups.contains(p_GraphicsGroupItem)) // Может быть игнор на случай остоединения более одного элемента от группы.
				{
					vp_AffectedGroups.append(p_GraphicsGroupItem); // Добавление в затронутые.
				}
				UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
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
		for(int iF = 0; iF != vp_SelectedGroupedElements.count(); iF++)
		{
			p_GraphicsElementItem = vp_SelectedGroupedElements.at(iF);
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
			UpdateLinksZPos();
		}
	}
	return bAction;
}

// Удаление выбранного и подготовка отправки по запросу.
void SchematicView::DeleteSelectedAPFS()
{
	for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
	{
		DeleteGroupAPFS(SchematicWindow::vp_SelectedGroups.at(iF));
	}
	for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
	{
		DeleteElementAPFS(SchematicWindow::vp_SelectedElements.at(iF));
	}
}

// Переопределение функции обработки нажатия на клавиши.
void SchematicView::keyPressEvent(QKeyEvent* p_Event)
{
	if(p_GraphicsPortItemActive) // Если пришло во время активного порта...
	{
		switch (p_Event->key())
		{
			case Qt::Key_Delete:
			{
				GraphicsPortItem* p_GraphicsPortItemActiveLast = p_GraphicsPortItemActive;
				QGraphicsSceneMouseEvent oQGraphicsSceneMouseEvent(QEvent::MouseButtonRelease);
				//
				oQGraphicsSceneMouseEvent.setButton(Qt::LeftButton);
				p_GraphicsPortItemActive->mouseReleaseEvent(&oQGraphicsSceneMouseEvent);
				while(p_GraphicsPortItemActive)
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
	double dbXMin = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
	double dbYMin = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
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
	UpdateLinkZPositionByElements(p_GraphicsLinkItemNew);
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

// Поднятие элемента на первый план и подготовка отсылки по запросу.
void SchematicView::ElementToTopAPFS(GraphicsElementItem* p_Element, bool bAddElementGroupChange,
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
	SetElementBlockingPattern(p_Element, bBlokingPattern);
	UpdateLinksZPos();
}

// Обновление фрейма группы по геометрии контента рекурсивно.
void SchematicView::UpdateGroupFrameByContentRecursively(GraphicsGroupItem* p_GraphicsGroupItem)
{
	DbPoint oDbPointLeftTop;
	DbPoint oDbPointRightBottom;
	DbFrame oDbFrameResult;
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItemInt;
	// ЭЛЕМЕНТЫ.
	p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(0);
	// Получаем крайние точки первого элемента в группе.
	oDbPointLeftTop.dbX = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
	oDbPointLeftTop.dbY = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
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
		oDbPointLeftTopTemp.dbX = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
		oDbPointLeftTopTemp.dbY = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
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
	// ГРУППЫ.
	// Цикл по группам для наращивания.
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedGroups.count(); iF++)
	{
		DbPoint oDbPointLeftTopTemp;
		DbPoint oDbPointRightBottomTemp;
		//
		p_GraphicsGroupItemInt = p_GraphicsGroupItem->vp_ConnectedGroups.at(iF);
		// Получаем крайние точки следующей группы в группе.
		oDbPointLeftTopTemp.dbX = p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX;
		oDbPointLeftTopTemp.dbY = p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY;
		oDbPointRightBottomTemp.dbX = oDbPointLeftTopTemp.dbX +
				p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW;
		oDbPointRightBottomTemp.dbY = oDbPointLeftTopTemp.dbY +
				p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH;
		// Расширяем до показателей текущей группы при зашкаливании по любой точке периметра.
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
	UpdateSelectedInGroup(p_GraphicsGroupItem, SCH_UPDATE_GROUP_FRAME | SCH_UPDATE_MAIN);
	if(p_GraphicsGroupItem->p_GraphicsGroupItemRel)
	{
		UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem->p_GraphicsGroupItemRel);
	}
}

// Обновление выбранных параметров в элементе.
void SchematicView::UpdateSelectedInElement(GraphicsElementItem* p_GraphicsElementItem, unsigned short ushBits,
											GraphicsPortItem* p_GraphicsPortItem,
											GraphicsLinkItem* p_GraphicsLinkItem, bool bIsIncoming)
{
	GraphicsPortItem* p_GraphicsPortItemInt;
	PSchLinkVars* p_SchLinkVars;
	QGraphicsItem* p_GraphicsItem;
	//
	if(ushBits & SCH_UPDATE_ELEMENT_ZPOS)
	{
		p_GraphicsElementItem->setZValue(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos);
		UpdateLinksZPos();
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
		p_GraphicsElementItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX,
									  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY);
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
						if(p_DbPointSrcPos->dbX < 0) p_DbPointSrcPos->dbX = 0;
						if(p_DbPointSrcPos->dbY < 0) p_DbPointSrcPos->dbY = 0;
						p_GraphicsPortItemInt->setPos(p_DbPointSrcPos->dbX, p_DbPointSrcPos->dbY);
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
						if(p_DbPointDstPos->dbX < 0) p_DbPointDstPos->dbX = 0;
						if(p_DbPointDstPos->dbY < 0) p_DbPointDstPos->dbY = 0;
						p_GraphicsPortItemInt->setPos(p_DbPointDstPos->dbX, p_DbPointDstPos->dbY);
					}
				}
			}
		}
		WidgetsThrAccess::p_ConnGraphicsElementItem = p_GraphicsElementItem;
		ThrUiAccessET(MainWindow::p_WidgetsThrAccess, ElementGroupBoxSizeSet);
		p_GraphicsElementItem->p_GraphicsScalerItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
															oSchElementGraph.oDbObjectFrame.dbW,
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
				UpdateLinkPositionByElements(p_GraphicsLinkItem);
				p_GraphicsLinkItem->update();
			}
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
			   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				UpdateLinkPositionByElements(p_GraphicsLinkItem);
				p_GraphicsLinkItem->update();
			}
		}
	}
	if(ushBits & SCH_UPDATE_LINK_POS)
	{
		UpdateLinkPositionByElements(p_GraphicsLinkItem);
		p_GraphicsLinkItem->update();
	}
	if(ushBits & SCH_UPDATE_GROUP)
	{
		if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
		{
			UpdateGroupFrameByContentRecursively(p_GraphicsElementItem->p_GraphicsGroupItemRel);
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

// Обновление выбранных параметров в группе.
void SchematicView::UpdateSelectedInGroup(GraphicsGroupItem* p_GraphicsGroupItem, unsigned short ushBits)
{
	if(ushBits & SCH_UPDATE_GROUP_ZPOS)
	{
		p_GraphicsGroupItem->setZValue(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos);
		UpdateLinksZPos();
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

// Установка блокировки на элемент.
void SchematicView::SetElementBlockingPattern(GraphicsElementItem* p_GraphicsElementItem, bool bValue)
{
	if(bValue)
	{
		p_GraphicsElementItem->oQBrush.setStyle(Qt::Dense4Pattern);
	}
	else
	{
		p_GraphicsElementItem->oQBrush.setStyle(Qt::SolidPattern);
	}
	UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_MAIN);
}

// Установка блокировки на группу.
void SchematicView::SetGroupBlockingPattern(GraphicsGroupItem* p_GraphicsGroupItem, bool bValue)
{
	if(bValue)
	{
		p_GraphicsGroupItem->oQBrush.setStyle(Qt::Dense6Pattern);
	}
	else
	{
		p_GraphicsGroupItem->oQBrush.setStyle(Qt::SolidPattern);
	}
	UpdateSelectedInGroup(p_GraphicsGroupItem, SCH_UPDATE_MAIN);
}

// Отпускание элемента и подготовка отправки по запросу.
void SchematicView::ReleaseElementAPFS(GraphicsElementItem* p_GraphicsElementItem, bool bWithGroup, bool bWithFrame)
{
	PSchElementVars oPSchElementVars;
	//
	if(p_GraphicsElementItem->oQBrush.style() == Qt::SolidPattern)
		return;
	if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
	{
		if(bWithGroup)
		{
			ReleaseGroupAPFSRecursively(p_GraphicsElementItem->p_GraphicsGroupItemRel);
			return;
		}
	}
	SetElementBlockingPattern(p_GraphicsElementItem, false);
	oPSchElementVars.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
	oPSchElementVars.oSchElementGraph.dbObjectZPos = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos;
	oPSchElementVars.oSchElementGraph.bBusy = false;
	oPSchElementVars.oSchElementGraph.uchChangesBits = SCH_ELEMENT_BIT_ZPOS | SCH_ELEMENT_BIT_BUSY;
	if(bWithFrame)
	{
		oPSchElementVars.oSchElementGraph.uchChangesBits |= SCH_ELEMENT_BIT_FRAME;
		oPSchElementVars.oSchElementGraph.oDbObjectFrame = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame;
	}
	MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars,
												   sizeof(PSchElementVars));
}

// Удаление всех графических элементов портов с элемента по ID.
void SchematicView::RemovePortsByID(unsigned long long ullID)
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

// Добавление свободных элементов в группу и подготовка к отправке по запросу.
bool SchematicView::AddFreeSelectedElementsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
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
		UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
		VerticalToTopAPFS(p_GraphicsGroupItem, SEND_GROUP, SEND_NEW_ELEMENTS_TO_GROUP, ADD_SEND_ZPOS, ADD_SEND_FRAME,
						  p_GraphicsElementItemInitial, nullptr, ELEMENTS_BLOCKING_PATTERN_OFF, SEND_ELEMENTS);
		if(p_GraphicsElementItemInitial != nullptr)
		{
			ElementToTopAPFS(p_GraphicsElementItemInitial, SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_ZPOS,
							 ELEMENTS_BLOCKING_PATTERN_OFF, SEND_ELEMENT);
		}
		UpdateLinksZPos();
	}
	if(bForceSelected)
	{
		SchematicWindow::vp_SelectedElements.removeOne(p_GraphicsElementItemInitial);
	}
	delete vp_NewElementsForGroup;
	vp_NewElementsForGroup = nullptr;
	return bAction;
}

// Выбор элемента.
void SchematicView::SelectElement(GraphicsElementItem* p_GraphicsElementItem, bool bLastState)
{
	if(bLastState != p_GraphicsElementItem->bSelected)
	{ // И раньше было не выбрано - добавление в вектор выбранных элементов.
		SchematicWindow::vp_SelectedElements.push_front(p_GraphicsElementItem);
		p_GraphicsElementItem->p_GraphicsFrameItem->show(); // Зажигаем рамку.
	}
	//
	if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr) // Если присутствует - поднятие, исключая текущий элемент.
	{
		VerticalToTopAPFS(p_GraphicsElementItem->p_GraphicsGroupItemRel, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
						  DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
						  DONT_ADD_SEND_FRAME, p_GraphicsElementItem, ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENTS);
	}
	ElementToTopAPFS(p_GraphicsElementItem, DONT_SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_BUSY,
					 ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENT); // Если в группе - не отсылать.
	p_GraphicsElementItem->bSelected = true;
}

// Отмена выбора элемента.
void SchematicView::DeselectElement(GraphicsElementItem* p_GraphicsElementItem, bool bLastState)
{
	if(bLastState != p_GraphicsElementItem->bSelected)
	{
		int iN;
		//
		iN = SchematicWindow::vp_SelectedElements.indexOf(p_GraphicsElementItem);
		if(iN != -1)
		{
			SchematicWindow::vp_SelectedElements.removeAt(iN);
			p_GraphicsElementItem->p_GraphicsFrameItem->hide(); // Гасим рамку.
		}
	}
	if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr) // Если присутствует - поднятие, исключая текущий элемент.
	{
		VerticalToTopAPFS(p_GraphicsElementItem->p_GraphicsGroupItemRel, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
						  DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
						  DONT_ADD_SEND_FRAME, p_GraphicsElementItem, ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENTS);
	}
	ElementToTopAPFS(p_GraphicsElementItem, DONT_SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_BUSY,
					 ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENT); // Если в группе - не отсылать.
	p_GraphicsElementItem->bSelected = false;
}

// Подъём содержания группы на первый план с сортировкой и подготовкой отсылки.
void SchematicView::SortGroupContentToTopAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
											  bool bAddNewElementsToGroupSending, bool bAddNewGroupsToGroupSending, bool bAddBusyOrZPosToSending,
											  GraphicsElementItem* p_GraphicsElementItemExclude, GraphicsGroupItem* p_GraphicsGroupItemExclude,
											  bool bWithSelectedDiff, bool bBlokingPatterns, bool bSend)
{
	QVector<SortedPointersVariant> v_SortedPointersVariants;
	QVector<SortedPointersVariant> v_SelectedSortedPointersVariants;
	QVector<SortedPointersVariant>* pv_SelectedSortedPointersVariants;
	const SortedPointersVariant* p_SortedPointersVariant;
	//
	if(bWithSelectedDiff)
	{
		pv_SelectedSortedPointersVariants = &v_SelectedSortedPointersVariants;
	}
	else
	{
		pv_SelectedSortedPointersVariants = nullptr;
	}
	SortObjectsByZPos(p_GraphicsGroupItem->vp_ConnectedElements, p_GraphicsElementItemExclude,
					  p_GraphicsGroupItem->vp_ConnectedGroups, p_GraphicsGroupItemExclude,
					  &v_SortedPointersVariants, pv_SelectedSortedPointersVariants);
	for(int iF = 0; iF != v_SortedPointersVariants.count(); iF++)
	{
		p_SortedPointersVariant = &v_SortedPointersVariants.at(iF);
		if(p_SortedPointersVariant->p_GraphicsElementItem != nullptr)
		{
			ElementToTopAPFS(p_SortedPointersVariant->p_GraphicsElementItem, bAddNewElementsToGroupSending,
							 bAddBusyOrZPosToSending, bBlokingPatterns, bSend);
		}
		else
		{
				GroupToTopAPFSRecursively(p_SortedPointersVariant->p_GraphicsGroupItem, bSend, bAddNewElementsToGroupSending,
									  bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
									  false, nullptr, p_GraphicsGroupItemExclude, bBlokingPatterns);
		}
	}
	if(bWithSelectedDiff)
	{
		for(int iF = v_SelectedSortedPointersVariants.count() - 1; iF != -1; iF--)
		{
			p_SortedPointersVariant = &v_SelectedSortedPointersVariants.at(iF);
			if(p_SortedPointersVariant->p_GraphicsElementItem != nullptr)
			{
				ElementToTopAPFS(p_SortedPointersVariant->p_GraphicsElementItem, bAddNewElementsToGroupSending,
								 bAddBusyOrZPosToSending, bBlokingPatterns, bSend);
			}
			else
			{
				GroupToTopAPFSRecursively(p_SortedPointersVariant->p_GraphicsGroupItem, bSend, bAddNewElementsToGroupSending,
										  bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
										  false, nullptr, p_GraphicsGroupItemExclude, bBlokingPatterns);
			}
		}
	}
	UpdateLinksZPos();
}

// Сортировка векторов элементов и групп по Z-позиции с приоритетом по выборке.
void SchematicView::SortObjectsByZPos(QVector<GraphicsElementItem*>& avp_Elements, GraphicsElementItem* p_GraphicsElementItemExclude,
									  QVector<GraphicsGroupItem*>& avp_Groups, GraphicsGroupItem* p_GraphicsGroupItemExclude,
									  QVector<SortedPointersVariant>* pv_SortedPointersVariants,
									  QVector<SortedPointersVariant>* pv_SelectionSortedPointersVariants)
{
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItem;
	QVector<SortedPointersVariant> v_PointersVariants;
	SortedPointersVariant oSortedPointersVariant;
	const SortedPointersVariant* p_SortedPointersVariant;
	//
	// Копия для работы.
	for(int iF = 0; iF < avp_Elements.count(); iF++)
	{
		p_GraphicsElementItem = avp_Elements.at(iF);
		if(p_GraphicsElementItem != p_GraphicsElementItemExclude)
		{
			oSortedPointersVariant.p_GraphicsElementItem = p_GraphicsElementItem;
			oSortedPointersVariant.p_GraphicsGroupItem = nullptr;
			v_PointersVariants.append(oSortedPointersVariant);
		}
	}
	for(int iF = 0; iF < avp_Groups.count(); iF++)
	{
		p_GraphicsGroupItem = avp_Groups.at(iF);
		if(p_GraphicsGroupItem != p_GraphicsGroupItemExclude)
		{
			oSortedPointersVariant.p_GraphicsElementItem = nullptr;
			oSortedPointersVariant.p_GraphicsGroupItem = p_GraphicsGroupItem;
			v_PointersVariants.append(oSortedPointersVariant);
		}
	}
	// Сортировка.
	while(!v_PointersVariants.isEmpty())
	{
		int iVariant;
		qreal dbZ = OVERMAX_NUMBER;
		for(int iF = 0; iF < v_PointersVariants.count(); iF++)
		{
			p_SortedPointersVariant = &v_PointersVariants.at(iF);
			if(p_SortedPointersVariant->p_GraphicsElementItem != nullptr)
			{
				if(dbZ > p_SortedPointersVariant->p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos)
				{
					dbZ = p_SortedPointersVariant->p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos;
					iVariant = iF;
				}
			}
			else
			{
				if(dbZ > p_SortedPointersVariant->p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos)
				{
					dbZ = p_SortedPointersVariant->p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos;
					iVariant = iF;
				}
			}
		}
		p_SortedPointersVariant = &v_PointersVariants.at(iVariant);
		if(pv_SelectionSortedPointersVariants != nullptr)
		{
			if(p_SortedPointersVariant->p_GraphicsElementItem != nullptr)
			{
				if(SchematicWindow::vp_SelectedElements.contains(p_SortedPointersVariant->p_GraphicsElementItem))
				{
					pv_SelectionSortedPointersVariants->append(*p_SortedPointersVariant);
				}
				else
				{
					goto gSE;
				}
			}
			else
			{
				if(SchematicWindow::vp_SelectedGroups.contains(p_SortedPointersVariant->p_GraphicsGroupItem))
				{
					pv_SelectionSortedPointersVariants->append(*p_SortedPointersVariant);
				}
				else
				{
					goto gSE;
				}
			}

		}
		else
		{
gSE:		pv_SortedPointersVariants->append(*p_SortedPointersVariant);
		}
		v_PointersVariants.removeAt(iVariant);
	}
}

// Поднятие вертикали на первый план и подготовка к отсылке по запросу.
void SchematicView::VerticalToTopAPFS(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend,
									  bool bAddNewElementsToGroupSending, bool bAddNewGroupsToGroupSending,
									  bool bAddBusyOrZPosToSending, bool bAddFrame,
									  GraphicsElementItem* p_GraphicsElementItemExclude,
									  bool bBlokingPatterns, bool bSendElements)
{
	GraphicsGroupItem* p_GraphicsGroupItemRoot = p_GraphicsGroupItem;
	// Вниз, к корню.
	while(p_GraphicsGroupItemRoot->p_GraphicsGroupItemRel != nullptr)
	{
		p_GraphicsGroupItemRoot = p_GraphicsGroupItemRoot->p_GraphicsGroupItemRel;
	}
	GroupToTopAPFSRecursively(p_GraphicsGroupItemRoot, bSend, bAddNewElementsToGroupSending, bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
							  bAddFrame, p_GraphicsElementItemExclude, p_GraphicsGroupItem, bBlokingPatterns, bSendElements);
	GroupToTopAPFSRecursively(p_GraphicsGroupItem, bSend, bAddNewElementsToGroupSending, bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
							  bAddFrame, p_GraphicsElementItemExclude, nullptr, bBlokingPatterns, bSendElements);
}

// Поднятие группы на первый план и подготовка к отсылке по запросу рекурсивно.
void SchematicView::GroupToTopAPFSRecursively(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend,
											bool bAddNewElementsToGroupSending, bool bAddNewGroupsToGroupSending,
											bool bAddBusyOrZPosToSending, bool bAddFrame,
											GraphicsElementItem* p_GraphicsElementItemExclude, GraphicsGroupItem* p_GraphicsGroupItemExclude,
											bool bBlokingPatterns, bool bSendElements)
{
	PSchGroupVars oPSchGroupVars;
	//
	if(p_GraphicsGroupItem == p_GraphicsGroupItemExclude) return;
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
	p_GraphicsGroupItem->setZValue(SchematicWindow::dbObjectZPos);
	SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
	p_GraphicsGroupItem->update();
	if(bSend)
	{
		SetGroupBlockingPattern(p_GraphicsGroupItem, bBlokingPatterns);
		oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
		if(bAddBusyOrZPosToSending)
		{
			oPSchGroupVars.oSchGroupGraph.bBusy = true;
			oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_BUSY;
		}
		else
		{
			oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_ZPOS;
			oPSchGroupVars.oSchGroupGraph.dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos;
		}
		if(bAddFrame)
		{
			oPSchGroupVars.oSchGroupGraph.uchChangesBits |= SCH_GROUP_BIT_FRAME;
			oPSchGroupVars.oSchGroupGraph.oDbObjectFrame = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
		}
		MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars,
													   sizeof(PSchGroupVars));
	}
	if(bSend == false) bSendElements = false;
	SortGroupContentToTopAPFS(p_GraphicsGroupItem, bAddNewElementsToGroupSending, bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
							  p_GraphicsElementItemExclude, p_GraphicsGroupItemExclude, GET_SELECTED_OBJECTS_UP,
							  bBlokingPatterns, bSendElements);
}

// Отпускание группы и подготовка отправки по запросу рекурсивно.
void SchematicView::ReleaseGroupAPFSRecursively(GraphicsGroupItem* p_GraphicsGroupItem, GraphicsElementItem* p_GraphicsElementItemExclude,
											  GraphicsGroupItem* p_GraphicsGroupItemExclude, bool bWithFrame,
											  bool bWithElementFrames)
{
	GraphicsElementItem* p_GraphicsElementItem;
	PSchGroupVars oPSchGroupVars;
	//
	if(p_GraphicsGroupItem->oQBrush.style() == Qt::SolidPattern)
		return;
	if(p_GraphicsGroupItem->p_GraphicsGroupItemRel != nullptr)
	{
		ReleaseGroupAPFSRecursively(p_GraphicsGroupItem->p_GraphicsGroupItemRel, nullptr, p_GraphicsGroupItem,
								  bWithFrame, bWithElementFrames);
	}
	oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
	SetGroupBlockingPattern(p_GraphicsGroupItem, false);
	oPSchGroupVars.oSchGroupGraph.dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos;
	oPSchGroupVars.oSchGroupGraph.bBusy = false;
	oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_BUSY;
	if(bWithFrame)
	{
		oPSchGroupVars.oSchGroupGraph.uchChangesBits |= SCH_GROUP_BIT_FRAME;
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX = p_GraphicsGroupItem->x();
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY = p_GraphicsGroupItem->y();
		oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
				p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
	}
	oPSchGroupVars.oSchGroupGraph.uchChangesBits |= SCH_GROUP_BIT_ZPOS;
	MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars,
												   sizeof(PSchGroupVars));
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++)
	{
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iF);
		if(p_GraphicsElementItem != p_GraphicsElementItemExclude)
		{
			ReleaseElementAPFS(p_GraphicsElementItem, WITHOUT_GROUP, bWithElementFrames);
		}
	}
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedGroups.count(); iF++)
	{
		GraphicsGroupItem* p_GraphicsGroupItemHelper = p_GraphicsGroupItem->vp_ConnectedGroups.at(iF);
		if(p_GraphicsGroupItemHelper != p_GraphicsGroupItemExclude)
		{
			ReleaseGroupAPFSRecursively(p_GraphicsGroupItemHelper, nullptr, nullptr, bWithFrame, bWithElementFrames);
		}
	}
}

// Выбор группы.
void SchematicView::SelectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState)
{
	if(bLastState != p_GraphicsGroupItem->bSelected)
	{ // И раньше было не выбрано - добавление в вектор выбранных групп.
		SchematicWindow::vp_SelectedGroups.push_front(p_GraphicsGroupItem);
		p_GraphicsGroupItem->p_GraphicsFrameItem->show(); // Зажигаем рамку.
	}
	//
	for(int iE = 0; iE != SchematicWindow::vp_SelectedGroups.count(); iE ++)
	{
		GraphicsGroupItem* p_GraphicsGroupItemUtil;
		//
		p_GraphicsGroupItemUtil = SchematicWindow::vp_SelectedGroups.at(iE);
		// Отправка наверх всех выбранных групп кроме текущей (её потом, в последнюю очередь, над всеми).
		if(p_GraphicsGroupItemUtil != p_GraphicsGroupItem)
		{
			VerticalToTopAPFS(p_GraphicsGroupItemUtil, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
									  DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
									  DONT_ADD_SEND_FRAME, nullptr, ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENTS);
		}
	}
	VerticalToTopAPFS(p_GraphicsGroupItem, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
							  DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
							  DONT_ADD_SEND_FRAME, nullptr, ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENTS);
	p_GraphicsGroupItem->bSelected = true;
}

// Отмена выбора группы.
void SchematicView::DeselectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState)
{
	if(bLastState != p_GraphicsGroupItem->bSelected)
	{
		int iN;
		//
		iN = SchematicWindow::vp_SelectedGroups.indexOf(p_GraphicsGroupItem);
		if(iN != -1)
		{
			SchematicWindow::vp_SelectedGroups.removeAt(iN);
			p_GraphicsGroupItem->p_GraphicsFrameItem->hide(); // Гасим рамку.
		}
	}
	VerticalToTopAPFS(p_GraphicsGroupItem, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
					  DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
					  DONT_ADD_SEND_FRAME, nullptr, ELEMENTS_BLOCKING_PATTERN_ON, SEND_ELEMENTS);
	p_GraphicsGroupItem->bSelected = false;
}

// Перемещение группы рекурсивно.
void SchematicView::MoveGroupRecursively(GraphicsGroupItem* p_GraphicsGroupItem, QPointF& a_QPointFRes, bool bMoveBody)
{
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItemHelper;
	//
	if(bMoveBody)
	{
		p_GraphicsGroupItem->setPos(p_GraphicsGroupItem->x() + a_QPointFRes.x(), p_GraphicsGroupItem->y() + a_QPointFRes.y());
	}
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX += a_QPointFRes.x();
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY += a_QPointFRes.y();
	UpdateSelectedInGroup(p_GraphicsGroupItem, SCH_UPDATE_MAIN);
	//
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++)
	{
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iF);
		p_GraphicsElementItem->setPos(p_GraphicsElementItem->x() + a_QPointFRes.x(), p_GraphicsElementItem->y() + a_QPointFRes.y());
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX += a_QPointFRes.x();
		p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY += a_QPointFRes.y();
		UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
	}
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedGroups.count(); iF++)
	{
		p_GraphicsGroupItemHelper = p_GraphicsGroupItem->vp_ConnectedGroups.at(iF);
		if(!p_GraphicsGroupItemHelper->bSelected)
		{
			MoveGroupRecursively(p_GraphicsGroupItemHelper, a_QPointFRes, true);
		}
	}
	if(p_GraphicsGroupItem->p_GraphicsGroupItemRel != nullptr)
	{
		if(!p_GraphicsGroupItem->p_GraphicsGroupItemRel->bSelected)
		{
			UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem->p_GraphicsGroupItemRel);
		}
	}
}

// Обновление Z-позиции линка по данным элементов.
void SchematicView::UpdateLinkZPositionByElements(GraphicsLinkItem* p_GraphicsLinkItem)
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

// Обновление позиции линка по данным элементов.
void SchematicView::UpdateLinkPositionByElements(GraphicsLinkItem* p_GraphicsLinkItem)
{
	DbPoint oDbPoint;
	DbPointPair oP;
	//
	oP = CalcPortsCoords(p_GraphicsLinkItem);
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
	p_GraphicsLinkItem->setPos(OVERMAX_NUMBER, OVERMAX_NUMBER); // Вынужденная мера.
	p_GraphicsLinkItem->setPos(oDbPoint.dbX, oDbPoint.dbY);
}

// Помощник коррекции точки порта по краю элемента.
void SchematicView::BindPortToOuterEdgeHelper()
{
	bool bXInside = false;
	bool bYInside = false;
	if(oDbPointPortCurrent.dbX <= 0) // Если текущий X меньше левого края элемента...
	{
		oDbPointPortCurrent.dbX = 0; // Установка на левый край.
		goto gY;
	}
	if(oDbPointPortCurrent.dbX >= oDbPointPortRB.dbX) // Если текущий X больше правого края элемента...
	{
		oDbPointPortCurrent.dbX = oDbPointPortRB.dbX; // Установка на правый край.
	}
	else
	{
		bXInside = true; // Признак нахождения в диапазоне элемента по X.
	}
gY: if(oDbPointPortCurrent.dbY <= 0)
	{
		oDbPointPortCurrent.dbY = 0;
		goto gI;
	}
	if(oDbPointPortCurrent.dbY >= oDbPointPortRB.dbY)
	{
		oDbPointPortCurrent.dbY = oDbPointPortRB.dbY;
	}
	else
	{
		bYInside = true; // Признак нахождения в диапазоне элемента по Y.
	}
gI: if(bXInside && bYInside)
	{
		// Если прошлый X был на краю...
		if((oDbPointPortOld.dbX == 0) || (oDbPointPortOld.dbX == oDbPointPortRB.dbX))
		{
			oDbPointPortCurrent.dbX = oDbPointPortOld.dbX;
		}
		if((oDbPointPortOld.dbY == 0) || (oDbPointPortOld.dbY == oDbPointPortRB.dbY))
		{
			oDbPointPortCurrent.dbY = oDbPointPortOld.dbY;
		}
	}
}

// Установка порта в позицию.
void SchematicView::SetPortToPos(GraphicsPortItem* p_GraphicsPortItem)
{
	p_GraphicsPortItem->setPos(oDbPointPortCurrent.dbX, oDbPointPortCurrent.dbY);
	if(p_GraphicsPortItem->bIsSrc)
	{
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX = oDbPointPortCurrent.dbX;
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY = oDbPointPortCurrent.dbY;
	}
	else
	{
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX = oDbPointPortCurrent.dbX;
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY = oDbPointPortCurrent.dbY;
	}
	UpdateSelectedInElement(p_GraphicsPortItem->p_ParentInt, SCH_UPDATE_LINK_POS | SCH_UPDATE_MAIN, nullptr, p_GraphicsPortItem->p_GraphicsLinkItemInt);
}

// Обработчик события нажатия мыши на элемент.
void SchematicView::ElementMousePressEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event)
{
	bool bLastSt;
	//
	if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
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
			oPSchLinkBase.oPSchLinkVars.ullIDSrc = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
			oPSchLinkBase.oPSchLinkVars.ullIDDst = oPSchLinkBase.oPSchLinkVars.ullIDSrc; // Временно, пока не перетянут на новый элемент.
			oPSchLinkBase.oPSchLinkVars.ushiSrcPort = DEFAULT_NEW_PORT;
			oPSchLinkBase.oPSchLinkVars.ushiDstPort = DEFAULT_NEW_PORT;
			// В координаты элемента.
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos = oDbPointInitialClick;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbX -=
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbY -=
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos = oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos;
			oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos = BindToInnerEdge(p_GraphicsElementItem, oDbPointInitialClick);
			// Создание замкнутого линка (пока что).
			p_GraphicsLinkItemNew = new GraphicsLinkItem(&oPSchLinkBase);
			if(oPSchLinkBase.oPSchLinkVars.oSchLinkGraph.uchChangesBits != SCH_LINK_BIT_INIT_ERROR)
			{
				MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsLinkItemNew);
			}
			else
			{
				delete p_GraphicsLinkItemNew;
				p_GraphicsLinkItemNew = nullptr;
				goto gNL;
			}
			SchematicWindow::vp_Links.push_front(p_GraphicsLinkItemNew);
			UpdateLinkZPositionByElements(p_GraphicsLinkItemNew);
			MainWindow::p_This->RemoteUpdateSchView();
			p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mousePressEvent(p_Event);
			p_GraphicsElementItem->OBMousePressEvent(p_Event);
			p_GraphicsLinkItemNew->p_GraphicsPortItemDst->p_GraphicsFrameItem->show(); // Зажигаем рамку.
			SchematicWindow::p_GraphicsFrameItemForPortFlash = p_GraphicsLinkItemNew->p_GraphicsPortItemDst->p_GraphicsFrameItem;
			return;
		}
		//==== РАБОТА С ВЫБОРКОЙ. ====
gNL:	bLastSt = p_GraphicsElementItem->bSelected; // Запоминаем предыдущее значение выбраности.
		if(p_Event->modifiers() == Qt::ControlModifier)
		{ // При удержании CTRL - инверсия флага выбраности.
			p_GraphicsElementItem->bSelected = !p_GraphicsElementItem->bSelected;
		}
		if(p_GraphicsElementItem->bSelected) // ВЫБРАЛИ...
			SelectElement(p_GraphicsElementItem, bLastSt);
		else // ОТМЕНИЛИ ВЫБОР...
			DeselectElement(p_GraphicsElementItem, bLastSt);
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
						" [" + QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName) + "]";
			}
			else
			{
				strCaption = "Выборка элементов";
			}
			SchematicWindow::p_SafeMenu->setMinimumWidth(GetStringWidthInPixels(SchematicWindow::p_SafeMenu->font(), strCaption) + 34);
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
				if(p_GraphicsElementItem == SchematicWindow::vp_Ports.at(iF)->p_ParentInt)
				{
					SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuPorts))->setData(MENU_PORTS);
					break;
				}
			}
			// Создать группу.
			if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
			{
				SchematicWindow::p_SafeMenu->addAction(m_chMenuCreateGroup)->setData(MENU_CREATE_GROUP);
			}
			// В группу.
			if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
			{
				if(SchematicWindow::vp_SelectedGroups.count() == 1)
				{
					SchematicWindow::p_SafeMenu->addAction(QString(QString(m_chMenuAddFreeSelected) +
																   " [" + QString(SchematicWindow::vp_SelectedGroups.at(0)->oPSchGroupBaseInt.m_chName)
																   + "]"))->setData(MENU_ADD_SELECTED);
				}
			}
			// Из группы.
			if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
			{
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuRemoveFromGroup))->setData(MENU_REMOVE_FROM_GROUPS);
			}
			// Цвет фона.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuBackground))->setData(MENU_CHANGE_BACKGROUND);
		}
	}
	TrySendBufferToServer;
	p_GraphicsElementItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с элементом.
void SchematicView::ElementMouseMoveEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	QPointF oQPointFInit;
	QPointF oQPointFRes;
	//
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_GraphicsLinkItemNew != nullptr)
	{
		p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mouseMoveEvent(p_Event);
		return;
	}
	else if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy) return;
	oQPointFInit = p_GraphicsElementItem->pos();
	p_GraphicsElementItem->OBMouseMoveEvent(p_Event);
	oQPointFRes = p_GraphicsElementItem->pos();
	oQPointFRes.setX(oQPointFRes.x() - oQPointFInit.x());
	oQPointFRes.setY(oQPointFRes.y() - oQPointFInit.y());
	if(p_GraphicsElementItem->bSelected)
	{
		iC = SchematicWindow::vp_SelectedElements.count();
		for(int iE = 0; iE != iC; iE ++)
		{
			GraphicsElementItem* p_GraphicsElementItemUtil;
			//
			p_GraphicsElementItemUtil = SchematicWindow::vp_SelectedElements.at(iE);
			if(p_GraphicsElementItemUtil != p_GraphicsElementItem)
			{
				p_GraphicsElementItemUtil->setPos(p_GraphicsElementItemUtil->x() + oQPointFRes.x(), p_GraphicsElementItemUtil->y() + oQPointFRes.y());
				p_GraphicsElementItemUtil->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX += oQPointFRes.x();
				p_GraphicsElementItemUtil->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY += oQPointFRes.y();
				UpdateSelectedInElement(p_GraphicsElementItemUtil, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
			}
		}
	}
	p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX = p_GraphicsElementItem->x();
	p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY = p_GraphicsElementItem->y();
	UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
}

// Обработчик события отпусканеия мыши на элементе.
void SchematicView::ElementMouseReleaseEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	//
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_GraphicsLinkItemNew != nullptr)
	{
		p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mouseReleaseEvent(p_Event);
		p_GraphicsLinkItemNew = nullptr;
		p_GraphicsElementItem->OBMouseReleaseEvent(p_Event);
		return;
	}
	else if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy) return;
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(p_GraphicsElementItem->bSelected)
		{
			iC = SchematicWindow::vp_SelectedElements.count();
			for(int iE = 0; iE != iC; iE ++)
			{
				GraphicsElementItem* p_GraphicsElementItemUtil;
				//
				p_GraphicsElementItemUtil = SchematicWindow::vp_SelectedElements.at(iE);
				if(p_GraphicsElementItemUtil != p_GraphicsElementItem)
				{
					ReleaseElementAPFS(p_GraphicsElementItemUtil);
				}
			}
		}
		ReleaseElementAPFS(p_GraphicsElementItem);
		TrySendBufferToServer;
	}
	p_GraphicsElementItem->OBMouseReleaseEvent(p_Event);
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
				CopyStrArray(p_GraphicsElementItem->oPSchElementBaseInt.m_chName, m_chName, SCH_OBJ_NAME_STR_LEN);
				p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Имя элемента", m_chName, SCH_OBJ_NAME_STR_LEN);
				if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
				{
					memset(&oPSchElementName, 0, sizeof(oPSchElementName));
					CopyStrArray(m_chName, oPSchElementName.m_chName, SCH_OBJ_NAME_STR_LEN);
					CopyStrArray(m_chName, p_GraphicsElementItem->oPSchElementBaseInt.m_chName, SCH_OBJ_NAME_STR_LEN);
					oPSchElementName.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
					MainWindow::p_Client->SendToServerImmediately(PROTO_O_SCH_ELEMENT_NAME, (char*)&oPSchElementName,
																  sizeof(oPSchElementName));
					p_GraphicsElementItem->p_QGroupBox->setTitle(oPSchElementName.m_chName);
					SchematicWindow::p_MainWindow->p_SchematicWindow->update();
				}
				p_Set_Proposed_String_Dialog->deleteLater();
			}
			else if(p_SelectedMenuItem->data() == MENU_DELETE)
			{
				if(!SchematicWindow::vp_SelectedElements.contains(p_GraphicsElementItem))
				{
					SchematicWindow::vp_SelectedElements.append(p_GraphicsElementItem);
				}
				DeleteSelectedAPFS();
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
				GraphicsElementItem* p_GraphicsElementItemUtil;
				unsigned char uchR = rand() % 255;
				unsigned char uchG = rand() % 255;
				unsigned char uchB = rand() % 255;
				unsigned char uchA = 200;
				vp_NewElementsForGroup = new QVector<GraphicsElementItem*>;
				//
				if(!SchematicWindow::vp_SelectedElements.contains(p_GraphicsElementItem))
				{
					SchematicWindow::vp_SelectedElements.append(p_GraphicsElementItem);
					bForceSelected = true;
				}
				oPSchGroupBase.oPSchGroupVars.ullIDInt = GenerateID();
				strName += ": " + QString::number(oPSchGroupBase.oPSchGroupVars.ullIDInt);
				CopyStrArray((char*)strName.toStdString().c_str(), oPSchGroupBase.m_chName, SCH_OBJ_NAME_STR_LEN);
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
				SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor = QColor(uchR, uchG, uchB, uchA).rgba();
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.bBusy = false;
				oPSchGroupBase.oPSchGroupVars.ullIDGroup = 0;
				p_GraphicsGroupItem = new GraphicsGroupItem(&oPSchGroupBase);
				MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsGroupItem);
				SchematicWindow::vp_Groups.push_front(p_GraphicsGroupItem);
				for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
				{
					p_GraphicsElementItemUtil = SchematicWindow::vp_SelectedElements.at(iF);
					if(p_GraphicsElementItemUtil->oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
					{
						p_GraphicsElementItemUtil->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = oPSchGroupBase.oPSchGroupVars.ullIDInt;
						p_GraphicsElementItemUtil->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
						p_GraphicsGroupItem->vp_ConnectedElements.append(p_GraphicsElementItemUtil);
						vp_NewElementsForGroup->append(p_GraphicsElementItemUtil);
					}
				}
				UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
				oPSchGroupBase.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
						p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_GROUP_BASE, (char*)&oPSchGroupBase, sizeof(PSchGroupBase));
				SortGroupContentToTopAPFS(p_GraphicsGroupItem, SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_ZPOS,
										  nullptr, nullptr, DONT_GET_SELECTED_OBJECTS_UP, DONT_APPLY_BLOCKINGPATTERN, SEND_ELEMENTS);
				UpdateLinksZPos();
				if(bForceSelected)
				{
					SchematicWindow::vp_SelectedElements.removeOne(p_GraphicsElementItem);
				}
				delete vp_NewElementsForGroup;
				vp_NewElementsForGroup = nullptr;
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_SELECTED)
			{
				AddFreeSelectedElementsToGroupAPFS(SchematicWindow::vp_SelectedGroups.at(0), p_GraphicsElementItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_REMOVE_FROM_GROUPS)
			{
				bool bForceSelected = false;
				//
				if(!SchematicWindow::vp_SelectedElements.contains(p_GraphicsElementItem))
				{
					SchematicWindow::vp_SelectedElements.append(p_GraphicsElementItem);
					bForceSelected = true;
				}
				DetachSelectedAPFS();
				if(bForceSelected)
				{
					SchematicWindow::vp_SelectedElements.removeOne(p_GraphicsElementItem);
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

// Обработчик функции рисования элемента.
void SchematicView::ElementPaintHandler(GraphicsElementItem* p_GraphicsElementItem, QPainter* p_Painter)
{
	p_Painter->setRenderHints(QPainter::SmoothPixmapTransform);
	p_Painter->setBrush(p_GraphicsElementItem->oQBrush);
	if(p_GraphicsElementItem->bIsPositivePalette)
	{
		p_Painter->setPen(SchematicWindow::oQPenWhite);
	}
	else
	{
		p_Painter->setPen(SchematicWindow::oQPenBlack);
	}
	p_Painter->drawRect(0, 0,
						p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW,
						p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH);
}

// Обработчик конструктора элемента.
void SchematicView::ElementConstructorHandler(GraphicsElementItem* p_GraphicsElementItem, PSchElementBase* p_PSchElementBase)
{
	int iR, iG, iB;
	QColor oQColorBkg;
	//
	p_GraphicsElementItem->setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	p_GraphicsElementItem->setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_ELEMENT);
	p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
	memcpy(&p_GraphicsElementItem->oPSchElementBaseInt, p_PSchElementBase, sizeof(PSchElementBase));
	p_GraphicsElementItem->setFlag(p_GraphicsElementItem->ItemIsMovable);
	p_GraphicsElementItem->setAcceptHoverEvents(true);
	p_GraphicsElementItem->setCursor(Qt::CursorShape::PointingHandCursor);
	p_GraphicsElementItem->bSelected = false;
	//
	p_GraphicsElementItem->p_QGroupBox = new QGroupBox();
	QVBoxLayout* p_QVBoxLayout = new QVBoxLayout;
	p_GraphicsElementItem->p_QGroupBox->setLayout(p_QVBoxLayout);
	p_GraphicsElementItem->p_QGroupBox->setTitle(p_PSchElementBase->m_chName);
	p_GraphicsElementItem->p_QGroupBox->setAttribute(Qt::WA_TranslucentBackground);
	p_GraphicsElementItem->p_QGroupBox->setCursor(Qt::CursorShape::PointingHandCursor);
	oQColorBkg = QColor::fromRgba(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uiObjectBkgColor);
	oQColorBkg.getRgb(&iR, &iG, &iB);
	if(((iR + iG + iB) / 3) > 128)
	{
		p_GraphicsElementItem->p_QGroupBox->setStyleSheet("QGroupBox { border:1px solid rgba(0, 0, 0, 255); border-radius: 3px; margin-top: 6px; } "
														  "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; "
														  "left: 4px; padding-top: 0px; }");
		p_GraphicsElementItem->oQPalette.setColor(QPalette::Foreground, QColor(Qt::black));
		p_GraphicsElementItem->bIsPositivePalette = false;
	}
	else
	{
		p_GraphicsElementItem->p_QGroupBox->setStyleSheet("QGroupBox { border:1px solid rgba(255, 255, 255, 255); border-radius: 3px; margin-top: 6px; } "
														  "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; "
														  "left: 4px; padding-top: 0px; }");
		p_GraphicsElementItem->oQPalette.setColor(QPalette::Foreground, QColor(Qt::white));
		p_GraphicsElementItem->bIsPositivePalette = true;
	}
	p_GraphicsElementItem->p_QGraphicsProxyWidget =
			MainWindow::p_SchematicWindow->GetSchematicView()->scene()->addWidget(p_GraphicsElementItem->p_QGroupBox); // Только так (на Linux).
	p_GraphicsElementItem->p_QGroupBox->move(3, 0);
	p_GraphicsElementItem->p_QGroupBox->setFixedSize(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW - 6,
													 p_GraphicsElementItem-> oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH - 3);
	p_GraphicsElementItem->p_QGraphicsProxyWidget->setFiltersChildEvents(true);
	p_GraphicsElementItem->p_QGraphicsProxyWidget->setParentItem(p_GraphicsElementItem);
	p_GraphicsElementItem->oQPalette.setBrush(QPalette::Background, p_GraphicsElementItem->oQBrush);
	p_GraphicsElementItem->p_QGroupBox->setPalette(p_GraphicsElementItem->oQPalette);
	//
	p_GraphicsElementItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX,
								  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY);
	p_GraphicsElementItem->setZValue(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos);
	if(SchematicWindow::dbObjectZPos <= p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos)
	{
		SchematicWindow::dbObjectZPos = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	p_GraphicsElementItem->oQBrush.setColor(
				QColor::fromRgba(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.uiObjectBkgColor));
	//
	p_GraphicsElementItem->p_GraphicsScalerItem = new GraphicsScalerItem(p_GraphicsElementItem);
	p_GraphicsElementItem->p_GraphicsScalerItem->setParentItem(p_GraphicsElementItem);
	p_GraphicsElementItem->p_GraphicsScalerItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW,
														p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH);
	p_GraphicsElementItem->p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_ELEMENT, p_GraphicsElementItem);
	p_GraphicsElementItem->p_GraphicsFrameItem->hide();
	//
	SetElementBlockingPattern(p_GraphicsElementItem, p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.bBusy);
}

// Обработчик события нажатия мыши на группу.
void SchematicView::GroupMousePressEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event)
{
	bool bLastSt;
	//
	if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{ //Если группа блокирована занятостью, смещением выборки или главным окном - отказ.
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		//==== РАБОТА С ВЫБОРКОЙ. ====
		bLastSt = p_GraphicsGroupItem->bSelected; // Запоминаем предыдущее значение выбраности.
		if(p_Event->modifiers() == Qt::ControlModifier)
		{ // При удержании CTRL - инверсия флага выбраности.
			p_GraphicsGroupItem->bSelected = !p_GraphicsGroupItem->bSelected;
		}
		if(p_GraphicsGroupItem->bSelected)// ВЫБРАЛИ...
			SelectGroup(p_GraphicsGroupItem, bLastSt);
		else // ОТМЕНИЛИ ВЫБОР...
			DeselectGroup(p_GraphicsGroupItem, bLastSt);
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
						" [" + QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName) + "]";
			}
			else
			{
				strCaption = "Выборка групп";
			}
			SchematicWindow::p_SafeMenu->setMinimumWidth(GetStringWidthInPixels(SchematicWindow::p_SafeMenu->font(), strCaption) + 50);
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
					SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuAddFreeSelected))->setData(MENU_ADD_SELECTED);
				}
			}
			// Цвет фона.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuBackground))->setData(MENU_CHANGE_BACKGROUND);
		}
	}
	TrySendBufferToServer;
	p_GraphicsGroupItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с группой.
void SchematicView::GroupMouseMoveEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	QPointF oQPointFInit;
	QPointF oQPointFRes;
	//
	if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	oQPointFInit = p_GraphicsGroupItem->pos();
	p_GraphicsGroupItem->OBMouseMoveEvent(p_Event);
	oQPointFRes = p_GraphicsGroupItem->pos();
	oQPointFRes.setX(oQPointFRes.x() - oQPointFInit.x()); // Смещение по X.
	oQPointFRes.setY(oQPointFRes.y() - oQPointFInit.y()); // Смещение по Y.
	if(p_GraphicsGroupItem->bSelected)
	{
		iC = SchematicWindow::vp_SelectedGroups.count();
		for(int iE = 0; iE != iC; iE ++)
		{
			GraphicsGroupItem* p_GraphicsGroupItemUtil;
			//
			p_GraphicsGroupItemUtil = SchematicWindow::vp_SelectedGroups.at(iE);
			if(p_GraphicsGroupItemUtil != p_GraphicsGroupItem)
			{
				MoveGroupRecursively(p_GraphicsGroupItemUtil, oQPointFRes);
			}
		}
	}
	MoveGroupRecursively(p_GraphicsGroupItem, oQPointFRes, false);
}

// Обработчик события отпусканеия мыши на группе.
void SchematicView::GroupMouseReleaseEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(p_GraphicsGroupItem->bSelected)
		{
			for(int iE = 0; iE != SchematicWindow::vp_SelectedGroups.count(); iE ++)
			{
				GraphicsGroupItem* p_GraphicsGroupItemUtil;
				//
				p_GraphicsGroupItemUtil = SchematicWindow::vp_SelectedGroups.at(iE);
				if(p_GraphicsGroupItemUtil != p_GraphicsGroupItem)
				{
					ReleaseGroupAPFSRecursively(p_GraphicsGroupItemUtil);
				}
			}
		}
		ReleaseGroupAPFSRecursively(p_GraphicsGroupItem);
		TrySendBufferToServer;
	}
	p_GraphicsGroupItem->OBMouseReleaseEvent(p_Event);
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
				CopyStrArray(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName, m_chName, SCH_OBJ_NAME_STR_LEN);
				p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Имя группы", m_chName, SCH_OBJ_NAME_STR_LEN);
				if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
				{
					memset(&oPSchGroupName, 0, sizeof(oPSchGroupName));
					CopyStrArray(m_chName, oPSchGroupName.m_chName, SCH_OBJ_NAME_STR_LEN);
					CopyStrArray(m_chName, p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName, SCH_OBJ_NAME_STR_LEN);
					oPSchGroupName.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
					MainWindow::p_Client->SendToServerImmediately(PROTO_O_SCH_GROUP_NAME, (char*)&oPSchGroupName,
																  sizeof(oPSchGroupName));
					p_GraphicsGroupItem->p_QLabel->setText(oPSchGroupName.m_chName);
					SchematicWindow::p_MainWindow->p_SchematicWindow->update();
				}
				p_Set_Proposed_String_Dialog->deleteLater();
			}
			else if(p_SelectedMenuItem->data() == MENU_DELETE)
			{
				if(!SchematicWindow::vp_SelectedGroups.contains(p_GraphicsGroupItem))
				{
					SchematicWindow::vp_SelectedGroups.append(p_GraphicsGroupItem);
				}
				DeleteSelectedAPFS();
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_SELECTED)
			{
				AddFreeSelectedElementsToGroupAPFS(p_GraphicsGroupItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_DISBAND)
			{
				bool bForceSelected = false;
				GraphicsGroupItem* p_GraphicsGroupItemUtil;
				//
				if(!SchematicWindow::vp_SelectedGroups.contains(p_GraphicsGroupItem))
				{
					SchematicWindow::vp_SelectedGroups.append(p_GraphicsGroupItem);
					bForceSelected = true;
				}
				for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
				{
					p_GraphicsGroupItemUtil = SchematicWindow::vp_SelectedGroups.at(iF);
					for(int iF = 0; iF != p_GraphicsGroupItemUtil->vp_ConnectedElements.count(); iF++)
					{
						GraphicsElementItem* p_GraphicsElementItem = p_GraphicsGroupItemUtil->vp_ConnectedElements.at(iF);
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
						UpdateLinksZPos();
					}
					SchematicWindow::vp_Groups.removeOne(p_GraphicsGroupItemUtil);
					MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsGroupItemUtil);
				}
				if(bForceSelected)
				{
					SchematicWindow::vp_SelectedGroups.removeOne(p_GraphicsGroupItem);
				}
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_ELEMENT)
			{
				GraphicsElementItem* p_GraphicsElementItem;
				//
				p_GraphicsElementItem = CreateNewElementAPFS((char*)m_chNewElement, p_GraphicsGroupItem->mapToScene(p_Event->pos()),
															 p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt);
				p_GraphicsGroupItem->vp_ConnectedElements.push_front(p_GraphicsElementItem);
				p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
				UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
				VerticalToTopAPFS(p_GraphicsGroupItem, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
								  ADD_SEND_ZPOS, ADD_SEND_FRAME,
								  nullptr, ELEMENTS_BLOCKING_PATTERN_OFF, SEND_ELEMENTS);
				UpdateLinksZPos();
			}
			else if(p_SelectedMenuItem->data() == MENU_CHANGE_BACKGROUND)
			{

			}
		}
		TrySendBufferToServer;
		SchematicWindow::ResetMenu();
	}
}

// Обработчик функции рисования группы.
void SchematicView::GroupPaintHandler(GraphicsGroupItem* p_GraphicsGroupItem, QPainter* p_Painter)
{
	p_Painter->setRenderHints(QPainter::SmoothPixmapTransform);
	p_Painter->setBrush(p_GraphicsGroupItem->oQBrush);
	if(p_GraphicsGroupItem->bIsPositivePalette)
	{
		p_Painter->setPen(SchematicWindow::oQPenWhite);
	}
	else
	{
		p_Painter->setPen(SchematicWindow::oQPenBlack);
	}
	p_Painter->drawRect(0, 0, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW,
						p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH);
	p_Painter->drawLine(1, 18, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW - 1, 18);
}

// Обработчик конструктора группы.
void SchematicView::GroupConstructorHandler(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupBase* p_PSchGroupBase)
{
	int iR, iG, iB, iA;
	QColor oQColorBkg;
	//
	p_GraphicsGroupItem->setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	p_GraphicsGroupItem->setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_GROUP);
	p_GraphicsGroupItem->p_GraphicsGroupItemRel = nullptr;
	memcpy(&p_GraphicsGroupItem->oPSchGroupBaseInt, p_PSchGroupBase, sizeof(PSchGroupBase));
	p_GraphicsGroupItem->setFlag(p_GraphicsGroupItem->ItemIsMovable);
	p_GraphicsGroupItem->setAcceptHoverEvents(true);
	p_GraphicsGroupItem->setCursor(Qt::CursorShape::PointingHandCursor);
	p_GraphicsGroupItem->bSelected = false;
	//
	oQColorBkg = QColor::fromRgba(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor);
	oQColorBkg.getRgb(&iR, &iG, &iB, &iA);
	if(((iR + iG + iB) / 3) > 128)
	{
		p_GraphicsGroupItem->oQPalette.setColor(QPalette::Foreground, QColor(Qt::black));
		p_GraphicsGroupItem->bIsPositivePalette = false;
	}
	else
	{
		p_GraphicsGroupItem->oQPalette.setColor(QPalette::Foreground, QColor(Qt::white));
		p_GraphicsGroupItem->bIsPositivePalette = true;
	}
	//
	p_GraphicsGroupItem->setPos(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX,
								p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY);
	p_GraphicsGroupItem->setZValue(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos);
	if(SchematicWindow::dbObjectZPos <= p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos)
	{
		SchematicWindow::dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	p_GraphicsGroupItem->oQBrush.setColor(QColor::fromRgba(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.uiObjectBkgColor));
	//
	p_GraphicsGroupItem->p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_GROUP, nullptr, p_GraphicsGroupItem);
	p_GraphicsGroupItem->p_GraphicsFrameItem->hide();
	//
	p_GraphicsGroupItem->p_QLabel = new QLabel(QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName));
	p_GraphicsGroupItem->p_QLabel->setAttribute(Qt::WA_TranslucentBackground);
	p_GraphicsGroupItem->p_QLabel->setCursor(Qt::CursorShape::PointingHandCursor);
	p_GraphicsGroupItem->p_QLabel->move(3, 3);
	p_GraphicsGroupItem->p_QLabel->setFixedSize(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW - 6, 13);
	p_GraphicsGroupItem->p_QGraphicsProxyWidget = MainWindow::p_SchematicWindow->oScene.addWidget(p_GraphicsGroupItem->p_QLabel);
	p_GraphicsGroupItem->p_QGraphicsProxyWidget->setFiltersChildEvents(true);
	p_GraphicsGroupItem->p_QGraphicsProxyWidget->setParentItem(p_GraphicsGroupItem);
	p_GraphicsGroupItem->p_QLabel->setPalette(p_GraphicsGroupItem->oQPalette);
	//
	SetGroupBlockingPattern(p_GraphicsGroupItem, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.bBusy);
}

// Рекурсивное удаление пустых групп.
bool SchematicView::GroupCheckEmptyAndRemoveRecursively(GraphicsGroupItem* p_GraphicsGroupItem)
{
	if(p_GraphicsGroupItem->vp_ConnectedElements.isEmpty() & p_GraphicsGroupItem->vp_ConnectedGroups.isEmpty()) // Если пустая группа...
	{
		if(p_GraphicsGroupItem->p_GraphicsGroupItemRel) // Если была в составе другой...
		{
			p_GraphicsGroupItem->p_GraphicsGroupItemRel->vp_ConnectedGroups.removeOne(p_GraphicsGroupItem); // Удаление из состава.
			GroupCheckEmptyAndRemoveRecursively(p_GraphicsGroupItem->p_GraphicsGroupItemRel); // Теперь та же проверка для хост-группы.
		}
		SchematicWindow::vp_Groups.removeOne(p_GraphicsGroupItem);
		MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsGroupItem);
		return true;
	}
	return false;
}

// Обработчик функции рисования фрейма.
void SchematicView::FramePaintHandler(GraphicsFrameItem* p_GraphicsFrameItem, QPainter* p_Painter)
{
	p_Painter->setBrush(Qt::NoBrush);
	if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_ELEMENT)
	{
		p_Painter->setPen(SchematicWindow::oQPenElementFrameFlash);
		p_Painter->drawRect(-2,
							-2,
							p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW + 4,
							p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH + 4);
	}
	else if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_GROUP)
	{
		p_Painter->setPen(SchematicWindow::oQPenGroupFrameFlash);
		p_Painter->drawRect(-2,
							-2,
							p_GraphicsFrameItem->p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW + 4,
							p_GraphicsFrameItem->p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH + 4);
	}
	else if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_PORT)
	{
		p_Painter->setPen(SchematicWindow::oQPenPortFrameFlash);
		p_Painter->drawEllipse(-4, -4, 8, 8);
	}
}

// Обработчик конструктора фрейма.
void SchematicView::FrameConstructorHandler(GraphicsFrameItem* p_GraphicsFrameItem, unsigned short ushKindOfItem,
											GraphicsElementItem* p_ElementParent, GraphicsGroupItem* p_GroupParent, GraphicsPortItem* p_PortParent)
{
	p_GraphicsFrameItem->p_ElementParentInt = p_ElementParent;
	p_GraphicsFrameItem->p_GroupParentInt = p_GroupParent;
	p_GraphicsFrameItem->ushKindOfItemInt = ushKindOfItem;
	p_GraphicsFrameItem->setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	p_GraphicsFrameItem->setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_FRAME);
	if(ushKindOfItem == SCH_KIND_ITEM_ELEMENT)
	{
		p_GraphicsFrameItem->setParentItem(p_GraphicsFrameItem->p_ElementParentInt);
	}
	else if(ushKindOfItem == SCH_KIND_ITEM_GROUP)
	{
		p_GraphicsFrameItem->setParentItem(p_GraphicsFrameItem->p_GroupParentInt);
	}
	else if(ushKindOfItem == SCH_KIND_ITEM_PORT)
	{
		p_GraphicsFrameItem->setParentItem(p_PortParent);
	}
	p_GraphicsFrameItem->setZValue(OVERMIN_NUMBER);
	p_GraphicsFrameItem->setAcceptedMouseButtons(0);
}

// Обработчик вместилища фрейма.
QRectF SchematicView::FrameBoundingRectHandler(GraphicsFrameItem* p_GraphicsFrameItem)
{
	if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_ELEMENT)
	{
		return QRectF(-3,
					  -3,
					  p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW + 6,
					  p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH + 6);
	}
	else if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_GROUP)
	{
		return QRectF(-3,
					  -3,
					  p_GraphicsFrameItem->p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW + 6,
					  p_GraphicsFrameItem->p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH + 6);
	}
	else if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_PORT)
	{
		return QRectF(-5,
					  -5,
					  10,
					  10);
	}
	return QRectF(0, 0, 0, 0);
}

// Обработчик функции рисования линка.
void SchematicView::LinkPaintHandler(GraphicsLinkItem* p_GraphicsLinkItem, QPainter* p_Painter)
{
	CalcPortHelper oC;
	DbPoint oDbPointMid;
	QPainterPath oQPainterPath;
	QPainterPathStroker oQPainterPathStroker;
	//
	oC = CalcLinkLineWidthHeight(p_GraphicsLinkItem);
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

// Обработчик конструктора линка.
void SchematicView::LinkConstructorHandler(GraphicsLinkItem* p_GraphicsLinkItem, PSchLinkBase* p_PSchLinkBase)
{
	p_GraphicsLinkItem->setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	p_GraphicsLinkItem->setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_LINK);
	memcpy(&p_GraphicsLinkItem->oPSchLinkBaseInt, p_PSchLinkBase, sizeof(PSchLinkBase));
	p_GraphicsLinkItem->p_GraphicsElementItemSrc = nullptr;
	p_GraphicsLinkItem->p_GraphicsElementItemDst = nullptr;
	// Поиск присоединённых элементов.
	for(int iF = 0; iF < SchematicWindow::vp_Elements.count(); iF++)
	{
		GraphicsElementItem* p_GraphicsElementItem;
		//
		p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
		if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt == p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc) &&
		   (p_GraphicsLinkItem->p_GraphicsElementItemSrc == nullptr))
		{
			p_GraphicsLinkItem->p_GraphicsElementItemSrc = p_GraphicsElementItem;
		}
		if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt == p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst) &&
		   (p_GraphicsLinkItem->p_GraphicsElementItemDst == nullptr))
		{
			p_GraphicsLinkItem->p_GraphicsElementItemDst = p_GraphicsElementItem;
		}
		if((p_GraphicsLinkItem->p_GraphicsElementItemSrc != nullptr) && (p_GraphicsLinkItem->p_GraphicsElementItemDst != nullptr))
		{
			SchematicWindow::vp_Ports.append(new GraphicsPortItem(p_GraphicsLinkItem, true, p_GraphicsLinkItem->p_GraphicsElementItemSrc));
			SchematicWindow::vp_Ports.append(new GraphicsPortItem(p_GraphicsLinkItem, false, p_GraphicsLinkItem->p_GraphicsElementItemDst));
			p_GraphicsLinkItem->p_PSchElementVarsSrc = &p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.oPSchElementVars;
			p_GraphicsLinkItem->p_PSchElementVarsDst = &p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.oPSchElementVars;
			UpdateLinkPositionByElements(p_GraphicsLinkItem);
			p_GraphicsLinkItem->setAcceptedMouseButtons(0);
			return;
		}
	}
	p_PSchLinkBase->oPSchLinkVars.oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_INIT_ERROR; // Отметка про сбой в поиске элементов.
}

// Вычисление точек портов.
SchematicView::DbPointPair SchematicView::CalcPortsCoords(GraphicsLinkItem* p_GraphicsLinkItem)
{
	DbPointPair oDbPointPairResult;
	//
	oDbPointPairResult.dbSrc.dbX = p_GraphicsLinkItem->p_PSchElementVarsSrc->oSchElementGraph.oDbObjectFrame.dbX +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbX;
	oDbPointPairResult.dbSrc.dbY = p_GraphicsLinkItem->p_PSchElementVarsSrc->oSchElementGraph.oDbObjectFrame.dbY +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbSrcPortGraphPos.dbY;
	oDbPointPairResult.dbDst.dbX = p_GraphicsLinkItem->p_PSchElementVarsDst->oSchElementGraph.oDbObjectFrame.dbX +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos.dbX;
	oDbPointPairResult.dbDst.dbY = p_GraphicsLinkItem->p_PSchElementVarsDst->oSchElementGraph.oDbObjectFrame.dbY +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.oDbDstPortGraphPos.dbY;
	return oDbPointPairResult;
}

// Вычисление квадрата и вместилища линии линка.
SchematicView::CalcPortHelper SchematicView::CalcLinkLineWidthHeight(GraphicsLinkItem* p_GraphicsLinkItem)
{
	CalcPortHelper oRes;
	//
	oRes.oQRectF.setX(0);
	oRes.oQRectF.setY(0);
	oRes.oDbPointPair = CalcPortsCoords(p_GraphicsLinkItem);
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

// Обработчик события нажатия мыши на порт.
void SchematicView::PortMousePressEventHandler(GraphicsPortItem* p_GraphicsPortItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	bPortFromElement = p_GraphicsLinkItemNew != nullptr;
	if(p_GraphicsPortItem->p_SchElementGraph->bBusy & (!bPortFromElement)) return;
	bPortLMBPressed = false;
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		oDbPointPortInitialClick.dbX = p_GraphicsPortItem->pos().x(); // Исходный X.
		oDbPointPortInitialClick.dbY = p_GraphicsPortItem->pos().y(); // Исходный Y.
		oDbPointPortRB.dbX = p_GraphicsPortItem->p_SchElementGraph->oDbObjectFrame.dbW; // Крайняя правая точка.
		oDbPointPortRB.dbY = p_GraphicsPortItem->p_SchElementGraph->oDbObjectFrame.dbH; // Крайняя нижняя точка.
		if(p_GraphicsPortItem->p_ParentInt->p_GraphicsGroupItemRel != nullptr)
		{
			VerticalToTopAPFS(p_GraphicsPortItem->p_ParentInt->p_GraphicsGroupItemRel,
							  SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
							  ADD_SEND_BUSY, DONT_ADD_SEND_FRAME, p_GraphicsPortItem->p_ParentInt,
							  APPLY_BLOCKINGPATTERN, SEND_ELEMENTS);
		}
		ElementToTopAPFS(p_GraphicsPortItem->p_ParentInt, DONT_SEND_ELEMENT_GROUP_CHANGE, ADD_SEND_BUSY,
						 APPLY_BLOCKINGPATTERN, SEND_ELEMENT);
		TrySendBufferToServer;
		if(p_Event->modifiers() == Qt::AltModifier)
		{
			oDbPointPortOld.dbX = oDbPointPortInitialClick.dbX;
			oDbPointPortOld.dbY = oDbPointPortInitialClick.dbY;
			bPortAltPressed = true;
		}
		else
		{
			bPortAltPressed = false;
			oDbPointPortCurrent.dbX = p_GraphicsPortItem->pos().x(); // Текущий X.
			oDbPointPortCurrent.dbY = p_GraphicsPortItem->pos().y(); // Текущий Y.
		}
		bPortLMBPressed = true;
		p_GraphicsPortItemActive = p_GraphicsPortItem;
	}
	else if(p_Event->button() == Qt::MouseButton::RightButton)
	{
		if(SchematicWindow::p_SafeMenu == nullptr)
		{
			SchematicWindow::p_SafeMenu = new SafeMenu;
			SchematicWindow::p_SafeMenu->setStyleSheet("SafeMenu::separator {color: palette(link);}");
			//================= СОСТАВЛЕНИЕ ПУНКТОВ МЕНЮ. =================//
			QString strCaptionSrc = QString(p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName);
			QString strCaptionDst = QString(p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName);
			QString strPortSrc = QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort);
			QString strPortDst = QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort);
			QString strName = QString(m_chLink + QString(" [") + strCaptionSrc + QString(" <> ") + strCaptionDst + QString("]"));
			SchematicWindow::p_SafeMenu->setMinimumWidth(GetStringWidthInPixels(SchematicWindow::p_SafeMenu->font(), strName) + 50);
			// Линк.
			SchematicWindow::p_SafeMenu->addSection(strName)->setDisabled(true);
			// Порт.
			if(p_GraphicsPortItem->bIsSrc)
			{
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuSelectedPort) + "[" + strPortSrc + "]")->setData(MENU_SELECTED_PORT);
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuDstPort) + "[" + strPortDst + "]")->setData(MENU_DST_PORT);
			}
			else
			{
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuSelectedPort) + "[" + strPortDst + "]")->setData(MENU_SELECTED_PORT);
				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuSrcPort) + "[" + strPortSrc + "]")->setData(MENU_SRC_PORT);
			}
			// Удалить.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuDelete))->setData(MENU_DELETE);
		}
	}
	p_GraphicsPortItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с портом.
void SchematicView::PortMouseMoveEventHandler(GraphicsPortItem* p_GraphicsPortItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_GraphicsPortItem->p_SchElementGraph->bBusy & (!bPortFromElement)) return;
	//
	if(bPortLMBPressed)
	{
		if(!bPortAltPressed)
		{
			oDbPointPortOld.dbX = p_GraphicsPortItem->pos().x(); // Исходный X.
			oDbPointPortOld.dbY = p_GraphicsPortItem->pos().y(); // Исходный Y.
		}
	}
	p_GraphicsPortItem->OBMouseMoveEvent(p_Event); // Даём мышке уйти.
	if(bPortLMBPressed)
	{
		oDbPointPortCurrent.dbX = p_GraphicsPortItem->pos().x(); // Текущий X.
		oDbPointPortCurrent.dbY = p_GraphicsPortItem->pos().y(); // Текущий Y.
		if(!bPortAltPressed)
		{
			BindPortToOuterEdgeHelper();
		}
		SetPortToPos(p_GraphicsPortItem);
	}
}

// Обработчик события отпусканеия мыши на порте.
void SchematicView::PortMouseReleaseEventHandler(GraphicsPortItem* p_GraphicsPortItem, QGraphicsSceneMouseEvent* p_Event)
{
	QGraphicsItem* p_QGraphicsItem;
	QGraphicsItem* p_QGraphicsItemFounded = nullptr;
	GraphicsElementItem* p_GraphicsElementItemFounded = nullptr;
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItem;
	GraphicsLinkItem* p_GraphicsLinkItem;
	double dbZ = OVERMIN_NUMBER;
	DbPoint oDbMapped;
	char m_chPortNumber[PORT_NUMBER_STR_LEN];
	//
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_GraphicsPortItem->p_SchElementGraph->bBusy & (!bPortFromElement)) return;
	if(bPortLMBPressed)
	{
		if(bPortAltPressed) // Если жали Alt...
		{
			QList<QGraphicsItem*> lp_QGraphicsItems = MainWindow::p_SchematicWindow->oScene.items();
			QVector<QGraphicsItem*> vp_QGraphicsItemsOnPosition;
			// Позиция отпускания мышки на сцене.
			oDbMapped.dbX = p_Event->scenePos().x();
			oDbMapped.dbY = p_Event->scenePos().y();
			// Поиск всех групп и элементов в данной точке.
			for(int iF = 0; iF != lp_QGraphicsItems.count(); iF++)
			{
				p_QGraphicsItem = lp_QGraphicsItems.at(iF);
				if(p_QGraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI)
				{
					double dbX, dbXR, dbY, dbYD;
					if(p_QGraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_ELEMENT)
					{
						//
						p_GraphicsElementItem = (GraphicsElementItem*) p_QGraphicsItem;
						dbX = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX;
						dbY = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY;
						dbXR = dbX + p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbW;
						dbYD = dbY + p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbH;
						if(((oDbMapped.dbX > dbX) & (oDbMapped.dbX < dbXR)) & ((oDbMapped.dbY > dbY) & (oDbMapped.dbY < dbYD)))
						{
							vp_QGraphicsItemsOnPosition.append(p_QGraphicsItem);
						}
					}
					else if(p_QGraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_GROUP)
					{
						p_GraphicsGroupItem = (GraphicsGroupItem*) p_QGraphicsItem;
						dbX = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbX;
						dbY = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbY;
						dbXR = dbX + p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbW;
						dbYD = dbY + p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame.dbH;
						if(((oDbMapped.dbX > dbX) & (oDbMapped.dbX < dbXR)) & ((oDbMapped.dbY > dbY) & (oDbMapped.dbY < dbYD)))
						{
							vp_QGraphicsItemsOnPosition.append(p_QGraphicsItem);
						}
					}
				}
			}
			// Поиск самой верхней группы \ элемента.
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
			if(p_QGraphicsItemFounded->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_ELEMENT) // Если найденное самое верхнее - элемент...
			{
				p_GraphicsElementItemFounded = (GraphicsElementItem*) p_QGraphicsItemFounded; // Его в найденный рабочий.
			}
			if(p_GraphicsElementItemFounded) // Если есть найденный верхний рабочий...
			{
				// Поиск ошибки пользователя.
				if(p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
				   p_GraphicsPortItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
				{ // Если на исходный элемент...

					oDbPointPortCurrent = BindToInnerEdge(p_GraphicsElementItemFounded, oDbMapped); // Прикрепление с внутренней стороны.
					if(bPortFromElement)
					{
gEld:					p_GraphicsElementItemFounded = nullptr; // Не найдено корректного элемента.
gEl:					SchematicWindow::vp_Ports.removeOne(p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemSrc);
						SchematicWindow::vp_Ports.removeOne(p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemDst);
						SchematicWindow::vp_Links.removeOne(p_GraphicsPortItem->p_GraphicsLinkItemInt);
						MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsPortItem->p_GraphicsLinkItemInt);
						MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemSrc);
						MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemDst);
						goto gF;
					}
					oDbPointPortCurrent = BindToInnerEdge(p_GraphicsElementItemFounded, oDbMapped); // Прикрепление с внутренней стороны.
					p_GraphicsElementItemFounded = nullptr; // Не найдено корректного элемента.
					goto gEr; // На устанвку позиции граф. порта и отправку данных об этом.
				}
				else if((p_GraphicsPortItem->bIsSrc &
						 (p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
						  p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDDst)) |
						(!p_GraphicsPortItem->bIsSrc &
						 (p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
						  p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDSrc)))
				{ // Если замыкание линка...
gED:				p_GraphicsElementItemFounded = nullptr;
					if(bPortFromElement) goto gEl;
					oDbPointPortCurrent = oDbPointPortInitialClick; // Возврат точки порта на начальную от нажатия на ПКМ.
					SetPortToPos(p_GraphicsPortItem); // Установка позиции граф. порта.
					goto gF; // На отпускание группы (по надобности) и элемента, затем - на выход.
				}
				// Тест на создание дупликата линка (те же элементы, те же порты).
				for(int iF = 0; iF != SchematicWindow::vp_Links.count(); iF++)
				{
					p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iF);
					if(bPortFromElement)
					{
						if((p_GraphicsLinkItem != p_GraphicsLinkItemNew) &
						   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc ==
							p_GraphicsLinkItemNew->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc) &
						   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
							p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.ullIDInt) &
						   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort == DEFAULT_NEW_PORT) &
						   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort == DEFAULT_NEW_PORT))
						{
							goto gEld;
						}
					}
					else
					{
						if(p_GraphicsLinkItem != p_GraphicsPortItem->p_GraphicsLinkItemInt)
						{
							if(!p_GraphicsPortItem->bIsSrc) // Если текущий порт - приёмник...
							{
								if((p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc ==
									p_GraphicsPortItem->p_GraphicsLinkItemInt->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc) &
								   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort ==
									p_GraphicsPortItem->p_GraphicsLinkItemInt->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort) &
								   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
									p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.ullIDInt) &
								   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort ==
									p_GraphicsPortItem->p_GraphicsLinkItemInt->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort))
									goto gED;
							}
							else
							{
								if((p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
									p_GraphicsPortItem->p_GraphicsLinkItemInt->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst) &
								   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort ==
									p_GraphicsPortItem->p_GraphicsLinkItemInt->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort) &
								   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc ==
									p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.ullIDInt) &
								   (p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort ==
									p_GraphicsPortItem->p_GraphicsLinkItemInt->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort))
									goto gED;
							}
						}
					}
				}
			}
			// Отсутствие ошибок - на отпускание группы (по надобности) и элемента, затем - на замещение линка.
			// Не нашли, но пришло с элемента - в отказ.
			else if(bPortFromElement) goto gEl;
		}
		BindPortToOuterEdgeHelper();
gEr:	SetPortToPos(p_GraphicsPortItem);
		if(p_GraphicsPortItem->bIsSrc)
		{
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX = p_GraphicsPortItem->x();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY =p_GraphicsPortItem->y();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_SCR_PORT_POS;
		}
		else
		{
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX = p_GraphicsPortItem->x();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY = p_GraphicsPortItem->y();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_DST_PORT_POS;
		}
		if(!bPortFromElement)
		{
			MainWindow::p_Client->AddPocketToOutputBufferC(
						PROTO_O_SCH_LINK_VARS, (char*)p_GraphicsPortItem->p_PSchLinkVarsInt, sizeof(PSchLinkVars));
		}
gF:		if(p_GraphicsPortItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
		{
			if(p_GraphicsPortItem->p_ParentInt->p_GraphicsGroupItemRel != nullptr)
			{
				ReleaseGroupAPFSRecursively(p_GraphicsPortItem->p_ParentInt->p_GraphicsGroupItemRel, p_GraphicsPortItem->p_ParentInt, nullptr,
								 WITHOUT_FRAME, WITHOUT_ELEMENTS_FRAMES);
			}
		}
		ReleaseElementAPFS(p_GraphicsPortItem->p_ParentInt, WITHOUT_GROUP, WITHOUT_POSITION);
	}
	p_GraphicsPortItem->OBMouseReleaseEvent(p_Event);
	if(bPortLMBPressed)
	{
		if(p_GraphicsElementItemFounded)
		{
			ReplaceLink(p_GraphicsPortItem->p_GraphicsLinkItemInt, p_GraphicsElementItemFounded,
						p_GraphicsPortItem->bIsSrc, oDbMapped, bPortFromElement);
		}
		p_GraphicsPortItemActive = nullptr;
	}
	else if(SchematicWindow::p_SafeMenu != nullptr)
	{
		QAction* p_SelectedMenuItem;
		//================= ВЫПОЛНЕНИЕ ПУНКТОВ МЕНЮ. =================//
		p_SelectedMenuItem = SchematicWindow::p_SafeMenu->exec(QCursor::pos());
		if(p_SelectedMenuItem != 0)
		{
			Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog = nullptr;
			PSchLinkBase oPSchLinkBase;
			//
			if((p_SelectedMenuItem->data() == MENU_SRC_PORT) |
			   (p_SelectedMenuItem->data() == MENU_DST_PORT) |
			   (p_SelectedMenuItem->data() == MENU_SELECTED_PORT))
			{
				oPSchLinkBase.oPSchLinkVars.oSchLinkGraph = p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph;
				oPSchLinkBase.oPSchLinkVars.ullIDSrc = p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDSrc;
				oPSchLinkBase.oPSchLinkVars.ullIDDst = p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDDst;
				oPSchLinkBase.oPSchLinkVars.ushiSrcPort = p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort;
				oPSchLinkBase.oPSchLinkVars.ushiDstPort = p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort;
				DeleteLinkAPFS(p_GraphicsPortItem->p_GraphicsLinkItemInt, NOT_FROM_ELEMENT, DONT_REMOVE_FROM_CLIENT);
				if(p_SelectedMenuItem->data() == MENU_SRC_PORT)
				{
gSrc:				CopyStrArray((char*)QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort).toStdString().c_str(),
								 m_chPortNumber, PORT_NUMBER_STR_LEN);
					p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Номер или псевдоним порта источника",
																				  m_chPortNumber, PORT_NUMBER_STR_LEN);
					if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
					{
						p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort = QString(m_chPortNumber).toUShort();
						p_GraphicsPortItem->p_GraphicsLinkItemInt->
								p_GraphicsPortItemSrc->setToolTip(m_chPortTooltip +
																  QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort));
						oPSchLinkBase.oPSchLinkVars.ushiSrcPort = p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort;
gSd:					MainWindow::p_Client->SendToServerImmediately(PROTO_O_SCH_LINK_BASE, (char*)&oPSchLinkBase, sizeof(PSchLinkBase));
					}
					p_Set_Proposed_String_Dialog->deleteLater();
					goto gEx;
				}
				else if(p_SelectedMenuItem->data() == MENU_DST_PORT)
				{
gDst:				CopyStrArray((char*)QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort).toStdString().c_str(),
								 m_chPortNumber, PORT_NUMBER_STR_LEN);
					p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Номер или псевдоним порта приёмника",
																				  m_chPortNumber, PORT_NUMBER_STR_LEN);
					if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
					{
						p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort = QString(m_chPortNumber).toUShort();
						p_GraphicsPortItem->p_GraphicsLinkItemInt->
								p_GraphicsPortItemDst->setToolTip(m_chPortTooltip +
																  QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort));
						oPSchLinkBase.oPSchLinkVars.ushiDstPort = p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort;
						goto gSd;
					}
				}
				else if(p_SelectedMenuItem->data() == MENU_SELECTED_PORT)
				{
					if(p_GraphicsPortItem->bIsSrc) goto gSrc;
					else goto gDst;
				}
			}
gEx:		if(p_SelectedMenuItem->data() == MENU_DELETE)
			{
				DeleteLinkAPFS(p_GraphicsPortItem->p_GraphicsLinkItemInt);
			}
			if(p_Set_Proposed_String_Dialog) p_Set_Proposed_String_Dialog->deleteLater();
		}
		SchematicWindow::ResetMenu();
		bPortMenuExecuted = true;
	}
	TrySendBufferToServer;
}

// Обработчик функции рисования порта.
void SchematicView::PortPaintHandler(GraphicsPortItem* p_GraphicsPortItem, QPainter* p_Painter)
{
	SchematicWindow::SetTempBrushesStyle(p_GraphicsPortItem->p_ParentInt->oQBrush.style());
	if(p_GraphicsPortItem->bIsSrc)
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

// Обработчик конструктора порта.
void SchematicView::PortConstructorHandler(GraphicsPortItem* p_GraphicsPortItem, GraphicsLinkItem* p_GraphicsLinkItem,
										   bool bSrc, GraphicsElementItem* p_Parent)
{
	p_GraphicsPortItem->p_ParentInt = p_Parent;
	p_GraphicsPortItem->bIsSrc = bSrc;
	bPortAltPressed = false;
	p_GraphicsPortItem->p_PSchLinkVarsInt = &p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars;
	p_GraphicsPortItem->p_GraphicsLinkItemInt = p_GraphicsLinkItem;
	p_GraphicsPortItem->setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	p_GraphicsPortItem->setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_PORT);
	p_GraphicsPortItem->setFlag(p_GraphicsPortItem->ItemIsMovable);
	p_GraphicsPortItem->setAcceptHoverEvents(true);
	p_GraphicsPortItem->setCursor(Qt::CursorShape::PointingHandCursor);
	p_GraphicsPortItem->setParentItem(p_Parent);
	p_GraphicsPortItem->p_SchElementGraph = &p_GraphicsPortItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph;
	if(p_GraphicsPortItem->bIsSrc)
	{
		p_GraphicsPortItem->setPos(p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbX,
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbSrcPortGraphPos.dbY);
		p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemSrc = p_GraphicsPortItem;
		p_GraphicsPortItem->setToolTip(m_chPortTooltip + QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort));
	}
	else
	{
		p_GraphicsPortItem->setPos(p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbX,
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLinkGraph.oDbDstPortGraphPos.dbY);
		p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemDst = p_GraphicsPortItem;
		p_GraphicsPortItem->setToolTip(m_chPortTooltip + QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort));
	}
	p_GraphicsPortItem->p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_PORT, nullptr, nullptr, p_GraphicsPortItem);
	p_GraphicsPortItem->p_GraphicsFrameItem->hide();
}

// Обработчик нахождения курсора над портом.
void SchematicView::PortHoverEnterEventHandler(GraphicsPortItem* p_GraphicsPortItem)
{
	if(!bPortMenuExecuted)
	{
		p_GraphicsPortItem->p_GraphicsFrameItem->show(); // Зажигаем рамку.
		SchematicWindow::p_GraphicsFrameItemForPortFlash = p_GraphicsPortItem->p_GraphicsFrameItem;
	}
	else bPortMenuExecuted = false;
}

// Обработчик ухода курсора с порта.
void SchematicView::PortHoverLeaveEventHandler(GraphicsPortItem* p_GraphicsPortItem)
{
	p_GraphicsPortItem->p_GraphicsFrameItem->hide(); // Гасим рамку.
	SchematicWindow::p_GraphicsFrameItemForPortFlash = nullptr;
}

// Обработчик события нажатия мыши на скалер.
void SchematicView::ScalerMousePressEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(p_GraphicsScalerItem->p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		if(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel != nullptr)
		{
			VerticalToTopAPFS(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel, SEND_GROUP,
							  DONT_SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
							  ADD_SEND_BUSY, DONT_ADD_SEND_FRAME, p_GraphicsScalerItem->p_ParentInt);
		}
		ElementToTopAPFS(p_GraphicsScalerItem->p_ParentInt);
		TrySendBufferToServer;
	}
	p_GraphicsScalerItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с скалером.
void SchematicView::ScalerMouseMoveEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event)
{
	DbPoint oDbPointPos;
	//
	if(p_GraphicsScalerItem->p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	oDbPointPos.dbX = p_GraphicsScalerItem->pos().x();
	oDbPointPos.dbY = p_GraphicsScalerItem->pos().y();
	p_GraphicsScalerItem->OBMouseMoveEvent(p_Event);; // Даём мышке уйти.
	p_GraphicsScalerItem->setPos(QPointF((int)p_GraphicsScalerItem->pos().x(), (int)p_GraphicsScalerItem->pos().y()));
	if(p_Event->scenePos().x() <
	   p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbX + ELEMENT_MIN_X)
	{
		p_GraphicsScalerItem->setX(oDbPointPos.dbX);
	}
	if(p_Event->scenePos().y() <
	   p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph.oDbObjectFrame.dbY + ELEMENT_MIN_Y)
	{
		p_GraphicsScalerItem->setY(oDbPointPos.dbY);
	}
	oDbPointPos.dbX = p_GraphicsScalerItem->pos().x() - oDbPointPos.dbX;
	oDbPointPos.dbY = p_GraphicsScalerItem->pos().y() - oDbPointPos.dbY;
	p_GraphicsScalerItem->p_ParentInt->oDbPointDimIncrements = oDbPointPos;
	UpdateSelectedInElement(p_GraphicsScalerItem->p_ParentInt, SCH_UPDATE_ELEMENT_FRAME | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
	if(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel != nullptr) // По группе элемента без выборки, если она есть.
	{
		UpdateGroupFrameByContentRecursively(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel);
	}
}

// Обработчик события отпусканеия мыши на скалере.
void SchematicView::ScalerMouseReleaseEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(p_GraphicsScalerItem->p_SchElementGraph->bBusy || MainWindow::bBlockingGraphics)
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
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc ==
			   p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_SCR_PORT_POS;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_LINK_VARS, (char*)&p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars, sizeof(PSchLinkVars));
			}
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
			   p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLinkGraph.uchChangesBits = SCH_LINK_BIT_DST_PORT_POS;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_LINK_VARS, (char*)&p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars, sizeof(PSchLinkVars));
			}
		}
		if(p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDGroup != 0)
		{
			if(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel != nullptr)
			{
				ReleaseGroupAPFSRecursively(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel,
								 p_GraphicsScalerItem->p_ParentInt, nullptr, WITH_FRAME, WITHOUT_ELEMENTS_FRAMES);
			}
		}
		ReleaseElementAPFS(p_GraphicsScalerItem->p_ParentInt, WITHOUT_GROUP, WITH_FRAME);
		TrySendBufferToServer;
	}
	p_GraphicsScalerItem->OBMouseReleaseEvent(p_Event);
}

// Обработчик функции рисования скалера.
void SchematicView::ScalerPaintHandler(GraphicsScalerItem* p_GraphicsScalerItem, QPainter* p_Painter)
{
	if(p_GraphicsScalerItem->p_ParentInt->bIsPositivePalette)
	{
		p_Painter->setPen(SchematicWindow::oQPenWhite);
	}
	else
	{
		p_Painter->setPen(SchematicWindow::oQPenBlack);
	}
	p_Painter->setBrush(p_GraphicsScalerItem->p_ParentInt->oQBrush);
	p_Painter->drawPolygon(SchematicWindow::oPolygonForScaler);
}

// Обработчик конструктора скалера.
void SchematicView::ScalerConstructorHandler(GraphicsScalerItem* p_GraphicsScalerItem, GraphicsElementItem* p_Parent)
{
	p_GraphicsScalerItem->p_ParentInt = p_Parent;
	p_GraphicsScalerItem->setData(SCH_TYPE_OF_ITEM, SCH_TYPE_ITEM_UI);
	p_GraphicsScalerItem->setData(SCH_KIND_OF_ITEM, SCH_KIND_ITEM_SCALER);
	p_GraphicsScalerItem->setFlag(p_GraphicsScalerItem->ItemIsMovable);
	p_GraphicsScalerItem->setAcceptHoverEvents(true);
	p_GraphicsScalerItem->setCursor(Qt::CursorShape::SizeFDiagCursor);
	p_GraphicsScalerItem->setParentItem(p_Parent);
	p_GraphicsScalerItem->p_SchElementGraph = &p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchElementGraph;
}
