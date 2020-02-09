//== ВКЛЮЧЕНИЯ.
#include <math.h>
#include "schematic-view.h"
#include "schematic-window.h"

//== МАКРОСЫ.
#define SCH_INTERNAL_POS_UNCHANGED          -2147483647

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
bool SchematicView::bLMousePressed = false;
int SchematicView::iXInt = SCH_INTERNAL_POS_UNCHANGED;
int SchematicView::iYInt = SCH_INTERNAL_POS_UNCHANGED;
CBSchematicViewFrameChanged SchematicView::pf_CBSchematicViewFrameChangedInt;

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
	QPointF pntA = this->mapToScene(0, 0);
	QPointF pntB = this->mapToScene(this->viewport()->width(), this->viewport()->height());
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

// Создание нового элемента и подготовка отсылки параметров.
GraphicsElementItem* SchematicView::CreateNewElementAndPrepareForSending(QString& a_strNameBase, QPointF a_pntMapped, unsigned long long ullIDGroup)
{
	PSchElementBase oPSchElementBase;
	unsigned char uchR = rand() % 255;
	unsigned char uchG = rand() % 255;
	unsigned char uchB = rand() % 255;
	QString strName = a_strNameBase;
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
	MainWindow::p_Client->AddPocketToOutputBufferC(
				PROTO_O_SCH_ELEMENT_BASE, (char*)&oPSchElementBase, sizeof(PSchElementBase));
	p_GraphicsElementItem = new GraphicsElementItem(&oPSchElementBase);
	MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsElementItem);
	p_GraphicsElementItem->setZValue(oPSchElementBase.oPSchElementVars.oSchElementGraph.dbObjectZPos);
	SchematicWindow::vp_Elements.push_front(p_GraphicsElementItem);
	MainWindow::p_SchematicWindow->oScene.update();
	return p_GraphicsElementItem;
}

// Переопределение функции обработки нажатия на кнопку мыши.
void SchematicView::mousePressEvent(QMouseEvent* p_Event)
{
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		bLMousePressed = true;
		iXInt = SCH_INTERNAL_POS_UNCHANGED;
	}
	else if(p_Event->button() == Qt::MouseButton::RightButton)
	{
		QMenu oMenu;
		QAction* p_SelectedMenuItem;
		QPointF pntMapped;
		QString strHelper = QString(m_chNewElement);
		//
		pntMapped = this->mapToScene(p_Event->x(), p_Event->y());
		if(MainWindow::p_SchematicWindow->oScene.itemAt(pntMapped.x(), pntMapped.y(), transform())) goto gEx;
		oMenu.addAction(strHelper);
		p_SelectedMenuItem = oMenu.exec(p_Event->globalPos());
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->text() == strHelper)
			{
				CreateNewElementAndPrepareForSending(strHelper, pntMapped);
				MainWindow::p_Client->SendBufferToServer();
			}
		}
	}
gEx:QGraphicsView::mousePressEvent(p_Event);
}

// Переопределение функции обработки отпускания кнопки мыши.
void SchematicView::mouseReleaseEvent(QMouseEvent* p_Event)
{
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		bLMousePressed = false;
		if(iXInt == SCH_INTERNAL_POS_UNCHANGED)
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

// Удаление графического элемента из группы и отправка её фрейма только на сервер.
bool SchematicView::PrepareChangingFrameAndRemoveElementFromGroup(GraphicsElementItem* p_GraphicsElementItem)
{
	bool bGroupMustBeErased = false;
	GraphicsGroupItem* p_GraphicsGroupItem = p_GraphicsElementItem->p_GraphicsGroupItemRel;
	PSchGroupVars oPSchGroupVars;
	//
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
	return bGroupMustBeErased;
}

// Подготовка удаления графического элемента из сцены и группы, возврат флага на удаление группы элемента.
bool SchematicView::PrepareForRemoveElementFromScene(GraphicsElementItem* p_GraphicsElementItem)
{
	bool bGroupMustBeErased = false;
	// Удалене связанных портов.
	GraphicsElementItem::RemovePortsByID(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt);
	// Если элемент в группе...
	if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
	{
		bGroupMustBeErased = PrepareChangingFrameAndRemoveElementFromGroup(p_GraphicsElementItem);
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
void SchematicView::DeleteGroupAndPrepareForSending(GraphicsGroupItem* p_GraphicsGroupItem)
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
void SchematicView::DeleteElementAndPrepareForSending(GraphicsElementItem* p_GraphicsElementItem)
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

// Остоединение выбранного.
bool SchematicView::DetachSelected()
{
	bool bAction = false;
	GraphicsElementItem* p_GraphicsElementItem;
	QVector<GraphicsElementItem*> vp_SortedElements;
	PSchGroupVars oPSchGroupVars;
	QVector<GraphicsGroupItem*> vp_AffectedGroups;
	PSchElementVars oPSchElementVars;
	//
	GraphicsGroupItem::SortElementsByZPos(SchematicWindow::vp_SelectedElements, nullptr, &vp_SortedElements); // Сортировка элементов в выборке.
	for(int iF = 0; iF != vp_SortedElements.count(); iF++)
	{
		p_GraphicsElementItem = vp_SortedElements.at(iF);
		if(p_GraphicsElementItem->p_GraphicsGroupItemRel != nullptr)
		{
			GraphicsGroupItem* p_GraphicsGroupItem = p_GraphicsElementItem->p_GraphicsGroupItemRel;
			//
			if(PrepareChangingFrameAndRemoveElementFromGroup(p_GraphicsElementItem))  // Остоединение элемента и подготовка сообщения об этом.
			{
				SchematicWindow::vp_Groups.removeOne(p_GraphicsElementItem->p_GraphicsGroupItemRel);
				MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem->p_GraphicsGroupItemRel);
			}
			else
			{ // Если не была удалена группа как пустая...
				if(!vp_AffectedGroups.contains(p_GraphicsGroupItem)) // Может быть игнор на случай остоединения более одного элемента от группы.
				{
					vp_AffectedGroups.append(p_GraphicsGroupItem); // Добавление в затронутые.
				}
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
			oPSchGroupVars.oSchGroupGraph.dbObjectZPos =
					p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.dbObjectZPos;
			oPSchGroupVars.oSchGroupGraph.oDbObjectFrame =
					p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchGroupGraph.oDbObjectFrame;
			oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
			oPSchGroupVars.oSchGroupGraph.uchChangesBits = SCH_GROUP_BIT_FRAME | SCH_GROUP_BIT_ZPOS;
			MainWindow::p_Client->AddPocketToOutputBufferC(
						PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars, sizeof(PSchGroupVars));
		}
		for(int iF = 0; iF != vp_SortedElements.count(); iF++)
		{
			// Модифицированный вариант ElementToTop - под нужды отправки группы и Z-позиции всем.
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

// Удаление выбранного.
bool SchematicView::DeleteSelected()
{
	bool bAction = false;
	QVector<GraphicsElementItem*> vp_SortedElements;
	QVector<GraphicsGroupItem*> vp_SortedGroups;
	//
	GraphicsGroupItem::SortGroupsByZPos(SchematicWindow::vp_SelectedGroups, nullptr, &vp_SortedGroups); // Сортировка групп в выборке.
	for(int iF = 0; iF != vp_SortedGroups.count(); iF++)
	{
		DeleteGroupAndPrepareForSending(vp_SortedGroups.at(iF));
		bAction = true;
	}
	GraphicsGroupItem::SortElementsByZPos(SchematicWindow::vp_SelectedElements, nullptr, &vp_SortedElements); // Сортировка элементов в выборке.
	for(int iF = 0; iF != vp_SortedElements.count(); iF++)
	{
		DeleteElementAndPrepareForSending(vp_SortedElements.at(iF));
		bAction = true;
	}
	SchematicWindow::vp_SelectedGroups.clear();
	SchematicWindow::vp_SelectedElements.clear();
	return bAction;
}

// Переопределение функции обработки нажатия на клавиши.
void SchematicView::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_Delete:
	{
		DeleteSelected();
		break;
	}
	}
}


