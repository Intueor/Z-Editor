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
int SchematicView::iXInt = SCH_INTERNAL_POS_UNCHANGED;
int SchematicView::iYInt = SCH_INTERNAL_POS_UNCHANGED;
CBSchematicViewFrameChanged SchematicView::pf_CBSchematicViewFrameChangedInt;
GraphicsPortItem* SchematicView::p_GraphicsPortItemActive = nullptr;
QPointF SchematicView::pntMouseClickMapped;
QPointF SchematicView::pntMouseMoveMapped;
bool SchematicView::bShiftAndLMBPressed = false;
QGraphicsRectItem* SchematicView::p_QGraphicsRectItemSelectionDash = nullptr;
QGraphicsRectItem* SchematicView::p_QGraphicsRectItemSelectionDot = nullptr;
GraphicsLinkItem* SchematicView::p_GraphicsLinkItemNew = nullptr;
bool SchematicView::bPortAltPressed = false;
bool SchematicView::bLMBPressed = false;
bool SchematicView::bViewLMBPressed = false;
bool SchematicView::bRMBPressed = false;
bool SchematicView::bFieldMenuReady = false;
bool SchematicView::bElementMenuReady = false;
bool SchematicView::bGroupMenuReady = false;
bool SchematicView::bPortMenuReady = false;
DbPoint SchematicView::oDbPointPortRB;
DbPoint SchematicView::oDbPointPortCurrent;
DbPoint SchematicView::oDbPointPortInitialClick;
bool SchematicView::bPortFromElement = false;
bool SchematicView::bPortMenuExecuted = false;
QVector<GraphicsGroupItem*> SchematicView::v_AlreadyMovedGroups;
QVector<SchematicView::EGPointersVariant> SchematicView::v_OccupiedByClient;
constexpr QPointF SchematicView::pntTrR;
constexpr QPointF SchematicView::pntTrT;
constexpr QPointF SchematicView::pntTrL;
QPointF SchematicView::pntMinTrR;
QPointF SchematicView::pntMinTrT;
QPointF SchematicView::pntMinTrL;
QPointF SchematicView::pntMinFrameTrR;
QPointF SchematicView::pntMinFrameTrT;
QPointF SchematicView::pntMinFrameTrL;
QPolygonF SchematicView::oQPolygonFForRectScaler;
QPolygonF SchematicView::oQPolygonFForTriangleScaler;
double SchematicView::dbFrameDimIncSubCorr;
double SchematicView::dbFrameDimIncTwiceSubCorr;
double SchematicView::dbFrameDimIncTwiceSubDoubleCorr;
double SchematicView::dbFrameDimIncNegPlusCorr;
double SchematicView::dbFrameDimIncNegPlusHalfCorr;
double SchematicView::dbPortDimNeg;
double SchematicView::dbPortDimD;
double SchematicView::dbMinTriangleR;
double SchematicView::dbMinElementR;
double SchematicView::dbMinElementD;
double SchematicView::dbMinGroupD;
double SchematicView::dbMinGroupR;
double SchematicView::dbMinCircleR;
double SchematicView::dbMinCircleD;
double SchematicView::dbMinTriangleDerc;
double SchematicView::dbMinTriangleRSubMinTriangleDerc;
double SchematicView::dbMinCircleRPlusFrameDimIncSubCorr;
double SchematicView::dbMinElementDPlusFrameDimIncTwiceSubDoubleCorr;
QVector<GraphicsPortItem*> SchematicView::pv_GraphicsPortItemsCollected;

//== МАКРОСЫ.
#define TempSelectGroup(group)			bool _bForceSelected = false;\
										if(!SchematicWindow::vp_SelectedGroups.contains(group))\
										{\
											SchematicWindow::vp_SelectedGroups.append(group);\
											_bForceSelected = true;\
										}
#define TempDeselectGroup(group)		if(_bForceSelected)\
										{\
											SchematicWindow::vp_SelectedGroups.removeOne(group);\
										}
#define TempSelectElement(element)		bool _bForceSelected = false;\
										if(!SchematicWindow::vp_SelectedElements.contains(element))\
										{\
											SchematicWindow::vp_SelectedElements.append(element);\
											_bForceSelected = true;\
										}
#define TempDeselectElement(element)	if(_bForceSelected)\
										{\
											SchematicWindow::vp_SelectedElements.removeOne(element);\
										}
#define GetDiagPointOnCircle(radius)	(radius + (dbSqrtFromTwoDivByTwo * radius))
#define SetHidingStatus(object,status)	{if(status) object->hide(); else object->show();}

//== ФУНКЦИИ КЛАССОВ.
//== Класс виджета обзора.
// Конструктор.
SchematicView::SchematicView(QWidget* parent) : QGraphicsView(parent)
{
	double dbTSDimSubCorr = SCALER_TR_DIM - SCALER_TR_DIM_CORR;
	double dbFrameDimIncNeg = 0 - FRAME_DIM_INC;
	double dbFrameDimIncCorrHalf = FRAME_DIM_INC_CORR / 2.0f;
	//
	dbFrameDimIncSubCorr = FRAME_DIM_INC - FRAME_DIM_INC_CORR;
	dbFrameDimIncTwiceSubCorr = (FRAME_DIM_INC * 2.0f) - FRAME_DIM_INC_CORR;
	dbFrameDimIncTwiceSubDoubleCorr = dbFrameDimIncTwiceSubCorr - dbFrameDimIncCorrHalf;
	dbFrameDimIncNegPlusCorr = dbFrameDimIncNeg + FRAME_DIM_INC_CORR;
	dbFrameDimIncNegPlusHalfCorr = dbFrameDimIncNeg + dbFrameDimIncCorrHalf;
	//
	dbPortDimNeg = 0 - PORT_DIM;
	dbPortDimD = PORT_DIM * 2.0f;
	dbMinTriangleR = MINIMIZED_DIM / 2.0f;
	dbMinElementR = dbMinTriangleR * MIN_ELEMENT_PROPORTION;
	dbMinElementD = MINIMIZED_DIM * MIN_ELEMENT_PROPORTION;
	dbMinGroupD = MINIMIZED_DIM * MIN_GROUP_PROPORTION;
	dbMinGroupR = dbMinGroupD / 2.0f;
	dbMinCircleR = dbMinTriangleR * MIN_CIRCLE_PROPORTION;
	dbMinCircleD = dbMinCircleR * 2.0f;
	dbMinTriangleDerc = (dbMinTriangleR / TRIANGLE_DECR_PROPORTION) * 2.0f;
	dbMinTriangleRSubMinTriangleDerc = dbMinTriangleR - dbMinTriangleDerc;
	dbMinCircleRPlusFrameDimIncSubCorr = dbMinCircleR + dbFrameDimIncSubCorr;
	dbMinElementDPlusFrameDimIncTwiceSubDoubleCorr = dbMinElementD + dbFrameDimIncTwiceSubDoubleCorr;
	//
	oQPolygonFForRectScaler.append(QPointF(-SCALER_RECT_DIM, -SCALER_RECT_DIM_CORR));
	oQPolygonFForRectScaler.append(QPointF(-SCALER_RECT_DIM_CORR, -SCALER_RECT_DIM));
	oQPolygonFForRectScaler.append(QPointF(-SCALER_RECT_DIM_CORR, -SCALER_RECT_DIM_CORR));
	//
	oQPolygonFForTriangleScaler.append(QPointF(pntTrR.x() * dbTSDimSubCorr, pntTrR.y() * dbTSDimSubCorr));
	oQPolygonFForTriangleScaler.append(QPointF(pntTrT.x() * dbTSDimSubCorr, pntTrT.y() * dbTSDimSubCorr));
	oQPolygonFForTriangleScaler.append(QPointF(pntTrL.x() * dbTSDimSubCorr, pntTrL.y() * dbTSDimSubCorr));
	//
	pntMinTrR = QPointF(dbMinTriangleR + (pntTrR.x() * dbMinTriangleR) - dbMinTriangleDerc,
												 dbMinTriangleR + (pntTrR.y() * dbMinTriangleR));
	pntMinTrT = QPointF(dbMinTriangleR + (pntTrT.x() * dbMinTriangleR) - dbMinTriangleDerc,
												 dbMinTriangleR + (pntTrT.y() * dbMinTriangleR));
	pntMinTrL = QPointF(dbMinTriangleR + (pntTrL.x() * dbMinTriangleR) - dbMinTriangleDerc,
												 dbMinTriangleR + (pntTrL.y() * dbMinTriangleR));
	pntMinFrameTrR = QPointF(dbMinTriangleR + (pntTrR.x() * (dbMinTriangleR + dbFrameDimIncTwiceSubCorr)) - dbMinTriangleDerc,
												 dbMinTriangleR + (pntTrR.y() * (dbMinTriangleR + dbFrameDimIncTwiceSubCorr)));
	pntMinFrameTrT = QPointF(dbMinTriangleR + (pntTrT.x() * (dbMinTriangleR + dbFrameDimIncTwiceSubCorr)) - dbMinTriangleDerc,
												 dbMinTriangleR + (pntTrT.y() * (dbMinTriangleR + dbFrameDimIncTwiceSubCorr)));
	pntMinFrameTrL = QPointF(dbMinTriangleR + (pntTrL.x() * (dbMinTriangleR + dbFrameDimIncTwiceSubCorr)) - dbMinTriangleDerc,
												 dbMinTriangleR + (pntTrL.y() * (dbMinTriangleR + dbFrameDimIncTwiceSubCorr)));
	//
	srand(time(NULL));
	setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
}

// Деструктор.
SchematicView::~SchematicView()
{
	SchematicWindow::ResetMenu();
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
	if((rFactor < 0.1f) || (rFactor > 5.0f))
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
	if(bShiftAndLMBPressed)
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
	oPSchElementBase.oPSchElementVars.oSchEGGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
	oPSchElementBase.oPSchElementVars.oSchEGGraph.oDbFrame.dbX = a_pntMapped.x();
	oPSchElementBase.oPSchElementVars.oSchEGGraph.oDbFrame.dbY = a_pntMapped.y();
	oPSchElementBase.oPSchElementVars.oSchEGGraph.oDbFrame.dbW = 275;
	oPSchElementBase.oPSchElementVars.oSchEGGraph.oDbFrame.dbH = 75;
	oPSchElementBase.uiObjectBkgColor = QColor(uchR, uchG, uchB, uchA).rgba();
	p_GraphicsElementItem = new GraphicsElementItem(&oPSchElementBase);
	MainWindow::p_SchematicWindow->oScene.addItem(p_GraphicsElementItem);
	p_GraphicsElementItem->setZValue(oPSchElementBase.oPSchElementVars.oSchEGGraph.dbObjectZPos);
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
	SchematicWindow::ResetMenu();
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
	p_QGraphicsItem =
			MainWindow::p_SchematicWindow->oScene.itemAt(pntMouseClickMapped.x(), pntMouseClickMapped.y(), transform()); // Новый тест.
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		bViewLMBPressed = true;
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
		bFieldMenuReady = true;
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
		bViewLMBPressed = false;
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
						if(p_GraphicsElementItem->isVisible())
						{
							if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
							   SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
							{
								QPainterPath oQPainterPathShape = p_GraphicsElementItem->shape();
								oQPainterPathShape.translate(QPointF(
																 p_GraphicsElementItem->
																 oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX,
																 p_GraphicsElementItem->
																 oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY));
								QPainterPath oQPainterPathSelection;
								//
								oQPainterPathSelection.addRect(p_QGraphicsRectItemSelectionDot->rect());
								if(oQPainterPathShape.subtracted(oQPainterPathSelection).isEmpty()) goto gTS;
							}
							else
							{
								if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
								   SCH_SETTINGS_EG_BIT_MIN)
								{
									if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX > oQPointTL.x()) &
									   (p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY > oQPointTL.y()) &
									   ((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX +
										 dbMinElementD) < oQPointBR.x()) &
									   ((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY +
										 dbMinElementD) < oQPointBR.y()))
									{
										goto gTS;
									}
								}
								else
								{
									if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX >
										oQPointTL.x()) &
									   (p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY >
										oQPointTL.y()) &
									   ((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX +
										 p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW) <
										oQPointBR.x()) &
									   ((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY +
										 p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH) <
										oQPointBR.y()))
									{
	gTS:								if(!p_GraphicsElementItem->bSelected)
										{
											SchematicWindow::vp_SelectedElements.push_front(p_GraphicsElementItem);
											p_GraphicsElementItem->p_GraphicsFrameItem->show(); // Зажигаем рамку.
											p_GraphicsElementItem->bSelected = true;
										}
									}
								}
							}
						}
					}
					else
					{
						DbPoint oDbPointWH;
						//
						if(p_GraphicsGroupItem->isVisible())
						{
							if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
							{
								oDbPointWH.dbX = dbMinGroupD;
								oDbPointWH.dbY = dbMinGroupD;
							}
							else
							{
								oDbPointWH.dbX = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW;
								oDbPointWH.dbY = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH;
							}
							if((p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX > oQPointTL.x()) &
							   (p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY > oQPointTL.y()) &
							   ((p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX +
								 oDbPointWH.dbX) < oQPointBR.x()) &
							   ((p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY +
								 oDbPointWH.dbY) < oQPointBR.y()))
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
			}
			MainWindow::p_SchematicWindow->oScene.removeItem(p_QGraphicsRectItemSelectionDot);
			p_QGraphicsRectItemSelectionDot = nullptr;
			setDragMode(DragMode::ScrollHandDrag);
#ifndef WIN32
			QMouseEvent oEvent(QEvent::Type::MouseMove,
							   MainWindow::p_SchematicWindow->p_SchematicView->
							   mapFromGlobal(QCursor::pos()), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
			MainWindow::p_SchematicWindow->p_SchematicView->mouseMoveEvent(&oEvent);
#endif
		}
	}
	if((SchematicWindow::p_SafeMenu != nullptr) && bFieldMenuReady)
	{
		bFieldMenuReady = false;
		p_SelectedMenuItem = SchematicWindow::p_SafeMenu->exec(p_Event->globalPos());
		if(p_SelectedMenuItem != 0)
		{
			if(p_SelectedMenuItem->data() == MENU_CREATE_ELEMENT)
			{
				CreateNewElementAPFS((char*)m_chNewElement, pntMouseClickMapped);
				MainWindow::p_Client->SendBufferToServer();
			}
		}
	}
}

// Переопределение функции обработки перетаскивания вида.
void SchematicView::scrollContentsBy(int iX, int iY)
{
	if(bViewLMBPressed)
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
			oPSchGroupVars.oSchEGGraph.oDbFrame =
					p_GraphicsGroupItemRel->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame;
			oPSchGroupVars.ullIDInt = p_GraphicsGroupItemRel->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
			oPSchGroupVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_GROUP_BIT_FRAME;
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
void SchematicView::DeleteGroupRecursiveAPFS(GraphicsGroupItem* p_GraphicsGroupItem, bool bInitial)
{
	PSchGroupEraser oPSchGroupEraser;
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItemHelper;
	GraphicsGroupItem* p_GraphicsGroupItemAbove = nullptr;
	//
	if(bInitial)
	{
		oPSchGroupEraser.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
		MainWindow::p_Client->AddPocketToOutputBufferC(
					PROTO_O_SCH_GROUP_ERASE, (char*)&oPSchGroupEraser, sizeof(PSchGroupEraser));
		p_GraphicsGroupItemAbove = p_GraphicsGroupItem->p_GraphicsGroupItemRel;
	}
	for(int iE = 0; iE < p_GraphicsGroupItem->vp_ConnectedElements.count(); iE++)
	{
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iE);
		p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
		PrepareForRemoveElementFromScene(p_GraphicsElementItem);
		SchematicWindow::vp_SelectedElements.removeOne(p_GraphicsElementItem);
		MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsElementItem);
	}
	if(p_GraphicsGroupItem->p_GraphicsGroupItemRel != nullptr)
	{
		p_GraphicsGroupItemHelper = p_GraphicsGroupItem->p_GraphicsGroupItemRel;
		p_GraphicsGroupItemHelper->vp_ConnectedGroups.removeOne(p_GraphicsGroupItem);
	}
	while(!p_GraphicsGroupItem->vp_ConnectedGroups.isEmpty())
	{
		p_GraphicsGroupItemHelper = p_GraphicsGroupItem->vp_ConnectedGroups.at(0);
		DeleteGroupRecursiveAPFS(p_GraphicsGroupItemHelper, RECURSION_CONTINUE);
	}
	MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsGroupItem);
	SchematicWindow::vp_Groups.removeOne(p_GraphicsGroupItem);
	if(bInitial)
	{
		if(p_GraphicsGroupItemAbove != nullptr)
		{
			UpdateGroupFrameByContentRecursively(p_GraphicsGroupItemAbove);
			BlockingVerticalsAndPopupGroup(p_GraphicsGroupItemAbove,
										   SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
										   ADD_SEND_ZPOS, ADD_SEND_FRAME, SEND_ELEMENTS);
		}
	}
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
				if(!vp_AffectedGroups.contains(p_GraphicsGroupItem)) // Может быть игнор на случай остоединения более одного элемента от гр.
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
			oPSchGroupVars.oSchEGGraph.oDbFrame =
					p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame;
			oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
			oPSchGroupVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_GROUP_BIT_FRAME;
			MainWindow::p_Client->AddPocketToOutputBufferC(
						PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars, sizeof(PSchGroupVars));
		}
		for(int iF = 0; iF != vp_SelectedGroupedElements.count(); iF++)
		{
			p_GraphicsElementItem = vp_SelectedGroupedElements.at(iF);
			p_GraphicsElementItem->setZValue(SchematicWindow::dbObjectZPos);
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
			SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
			p_GraphicsElementItem->update();
			oPSchElementVars.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
			oPSchElementVars.ullIDGroup = 0;
			oPSchElementVars.oSchEGGraph.dbObjectZPos =
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos;
			oPSchElementVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_ELEMENT_BIT_GROUP | SCH_CHANGES_ELEMENT_BIT_ZPOS;
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
		DeleteGroupRecursiveAPFS(SchematicWindow::vp_SelectedGroups.at(iF));
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

// Пересчёт позиции граф. порта к краям элемента.
DbPoint SchematicView::BindToEdge(GraphicsElementItem* p_GraphicsElementItemNew, DbPoint oDbPortPos)
{
	DbPoint oDbFromRB;
	DbPoint oDbCenter;
	//
	oDbCenter.dbX = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2.0f;
	//
	if(p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		double dbR = oDbCenter.dbX;// Радиус.z
		//
		if(p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
		   SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			// ==== Круг ====
			double dbL;
			//
			oDbPortPos.dbX -= dbR; // Положение порта в системе коорд. центра элемента.
			oDbPortPos.dbY -= dbR;
			dbL = sqrt((oDbPortPos.dbX * oDbPortPos.dbX) + (oDbPortPos.dbY * oDbPortPos.dbY)); // Длина ветора порта от центра.
			oDbPortPos.dbX /= dbL; // Нормализуем вектор.
			oDbPortPos.dbY /= dbL;
			oDbPortPos.dbX *= dbR; // Доводим до радиуса.
			oDbPortPos.dbY *= dbR;
			oDbPortPos.dbX += dbR; // Положение порта в системе коорд. круга границ элемента.
			oDbPortPos.dbY += dbR;
		}
		else
		{
			// ==== Треугольник ====
			// Вычисление возможными средствами, нужна консультация математика для корректного решения.
			double dbDecr = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / TRIANGLE_DECR_PROPORTION;
			QPainterPath oQPainterPathLineToCenter;
			QPainterPath oQPainterPathFromCenterToEdge;
			double bdRShift = dbR - dbDecr; // Центр фигуры для треугольника.
			//
			oQPainterPathFromCenterToEdge = p_GraphicsElementItemNew->shape();
			// Выносим точку порта за пределы треугольника без существенной потери точности.
			while(oQPainterPathFromCenterToEdge.contains(QPointF(oDbPortPos.dbX, oDbPortPos.dbY)))
			{
				oDbPortPos.dbX = ((oDbPortPos.dbX - bdRShift) * 2.0f) + bdRShift;
				oDbPortPos.dbY = ((oDbPortPos.dbY - dbR) * 2.0f) + dbR;
			}
			// Строим гадкий треугольник в коорд. элемента.
			oQPainterPathLineToCenter.moveTo(bdRShift, dbR);
			oQPainterPathLineToCenter.lineTo(oDbPortPos.dbX, oDbPortPos.dbY);
			oQPainterPathLineToCenter.lineTo(oDbPortPos.dbX, oDbPortPos.dbY + 0.00001f);
			// Остаток гадкого треугольника от пересечения с основным.
			oQPainterPathFromCenterToEdge = oQPainterPathFromCenterToEdge.intersected(oQPainterPathLineToCenter);
			// Поиск точки.
			if(!oQPainterPathFromCenterToEdge.isEmpty())
			{
				QVector<DbPoint> v_DbPoint;
				DbPoint oDbPoint;
				QPolygonF oQPolygonF;
				// Вычленяем точки на ребре.
				oQPolygonF = oQPainterPathFromCenterToEdge.toFillPolygon();
				for(int iF = 0; iF != oQPolygonF.count(); iF++)
				{
					//
					oDbPoint.dbX = oQPolygonF.at(iF).x() - bdRShift;
					oDbPoint.dbY = oQPolygonF.at(iF).y() - dbR;
					if(!((oDbPoint.dbX == 0) && (oDbPoint.dbY == 0)))
					{
						v_DbPoint.append(oDbPoint);
					}
				}
				// Среднее из точек на ребре (по причине небольшого разброса на пересечении гадкого треугольника).
				if(!v_DbPoint.isEmpty())
				{
					int iC = v_DbPoint.count();
					oDbPoint.dbX = 0;
					oDbPoint.dbY = 0;
					for(int iF = 0; iF != iC; iF++)
					{
						oDbPoint.dbX += v_DbPoint.at(iF).dbX;
						oDbPoint.dbY += v_DbPoint.at(iF).dbY;
					}
					oDbPortPos.dbX = oDbPoint.dbX / iC;
					oDbPortPos.dbY = oDbPoint.dbY / iC;
				}
			}
			oDbPortPos.dbX += bdRShift;
			oDbPortPos.dbY += dbR;
		}
	}
	else
	{
		// ==== Прямоугольник ====
		oDbFromRB.dbX = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW - oDbPortPos.dbX;
		oDbFromRB.dbY = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH - oDbPortPos.dbY;
		oDbCenter.dbY = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH / 2.0f;
		// -= Снаружи к краям =-.
		bool bXInside = false;
		bool bYInside = false;
		//
		if(oDbPortPos.dbX <= 0) // Если текущий X меньше левого края элемента...
		{
			oDbPortPos.dbX = 0; // Установка на левый край.
			goto gY;
		}
		if(oDbPortPos.dbX >= p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW)
		{
			oDbPortPos.dbX = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW;
		}
		else
		{
			bXInside = true; // Признак нахождения в диапазоне элемента по X.
		}
gY:		if(oDbPortPos.dbY <= 0)
		{
			oDbPortPos.dbY = 0;
			goto gI;
		}
		if(oDbPortPos.dbY >= p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH)
		{
			oDbPortPos.dbY = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH;
		}
		else
		{
			bYInside = true; // Признак нахождения в диапазоне элемента по Y.
		}
gI:		if(bXInside && bYInside) // Если внутри...
		{
			// -= Изнутри к краям =-.
			bool bToLeft; // Ближе к левому.
			bool bToTop; // Ближе к верху.
			//
			if(oDbPortPos.dbX < oDbCenter.dbX) bToLeft = true; else bToLeft = false;
			if(oDbPortPos.dbY < oDbCenter.dbY) bToTop = true; else bToTop = false;
			if(bToLeft & bToTop) // Если у левого верхнего края...
			{
				if(oDbPortPos.dbX < oDbPortPos.dbY) // Если к левому ближе, чем к верхнему...
				{
					oDbPortPos.dbX = 0; // Прилипли к левому.
				}
				else
				{
					oDbPortPos.dbY = 0; // Прилипли к верхнему.
				}
			}
			else if (!bToLeft & !bToTop) // Если у правого нижнего края...
			{
				if(oDbFromRB.dbX < oDbFromRB.dbY) // Если к правому ближе, чем к нижнему...
				{
					oDbPortPos.dbX =
							p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW; // Прилипли к правому.
				}
				else
				{
					oDbPortPos.dbY =
							p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH;// Прилипли к нижнему.
				}
			}
			else if(bToLeft & !bToTop) // Если у левого нижнего края...
			{
				if(oDbPortPos.dbX < oDbFromRB.dbY) // Если к левому ближе, чем к нижнему...
				{
					oDbPortPos.dbX = 0; // Прилипли к левому.
				}
				else
				{
					oDbPortPos.dbY =
							p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH;// Прилипли к нижнему.
				}
			}
			else if(!bToLeft & bToTop) // Если у правого верхнего края...
			{
				if(oDbFromRB.dbX < oDbPortPos.dbY) // Если к правому ближе, чем к верхнему...
				{
					oDbPortPos.dbX =
							p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW;// Прилипли к правому.
				}
				else
				{
					oDbPortPos.dbY = 0;// Прилипли к нижнему.
				}
			}
		}
	}
	return oDbPortPos;
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
		oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos = BindToEdge(p_GraphicsElementItemNew, oDbPortPos);
		oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos =
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos;
	}
	else
	{
		oPSchLinkBase.oPSchLinkVars.ullIDSrc = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc;
		oPSchLinkBase.oPSchLinkVars.ullIDDst = p_GraphicsElementItemNew->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
		//
		oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos =
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos;
		oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos = BindToEdge(p_GraphicsElementItemNew, oDbPortPos);
	}
	oPSchLinkBase.oPSchLinkVars.ushiSrcPort = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort;
	oPSchLinkBase.oPSchLinkVars.ushiDstPort = p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort;
	//
	p_GraphicsLinkItemNew = new GraphicsLinkItem(&oPSchLinkBase);
	if(oPSchLinkBase.oPSchLinkVars.oSchLGraph.uchChangesBits != SCH_CHANGES_LINK_BIT_INIT_ERROR)
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

// Поднятие элемента на первый план, блокировка и подготовка отсылки по запросу.
void SchematicView::ElementToTopOrBusyAPFS(GraphicsElementItem* p_Element, bool bAddElementGroupChange,
									 bool bAddBusyOrZPosToSending, bool bSend, bool bToTop)
{
	PSchElementVars oPSchElementVars;
	EGPointersVariant oEGPointersVariant;
	//
	if(bToTop)
	{
		p_Element->setZValue(SchematicWindow::dbObjectZPos);
		p_Element->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
		SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
		p_Element->update();
	}
	if(bSend)
	{
		oPSchElementVars.ullIDInt = p_Element->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
		if(bAddBusyOrZPosToSending == ADD_SEND_BUSY)
		{
			oPSchElementVars.oSchEGGraph.uchSettingsBits |= SCH_SETTINGS_EG_BIT_BUSY;
			oPSchElementVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_ELEMENT_BIT_BUSY;
		}
		else
		{
			oPSchElementVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_ELEMENT_BIT_ZPOS;
			oPSchElementVars.oSchEGGraph.dbObjectZPos = p_Element->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos;
		}
		if(bAddElementGroupChange)
		{
			oPSchElementVars.oSchEGGraph.uchChangesBits |= SCH_CHANGES_ELEMENT_BIT_GROUP;
			oPSchElementVars.ullIDGroup = p_Element->oPSchElementBaseInt.oPSchElementVars.ullIDGroup;
		}
		MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars,
													   sizeof(oPSchElementVars));
	}
	oEGPointersVariant.p_GraphicsElementItem = p_Element;
	oEGPointersVariant.p_GraphicsGroupItem = nullptr;
	if(bAddBusyOrZPosToSending == ADD_SEND_BUSY) v_OccupiedByClient.append(oEGPointersVariant);
	SetElementBlockingPattern(p_Element, bAddBusyOrZPosToSending);
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
	QRectF oQRectFTemp;
	// ЭЛЕМЕНТЫ.
	if(!p_GraphicsGroupItem->vp_ConnectedElements.isEmpty())
	{
		// Если есть элементы - начинаем с них.
		p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(0);
		// Получаем крайние точки первого элемента в группе.
		oDbPointLeftTop.dbX = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX;
		oDbPointLeftTop.dbY = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY;
		oQRectFTemp = p_GraphicsElementItem->boundingRect();
		oDbPointRightBottom.dbX = oDbPointLeftTop.dbX + oQRectFTemp.width();
		oDbPointRightBottom.dbY = oDbPointLeftTop.dbY + oQRectFTemp.height();
		// Цикл по остальным элементам для наращивания.
		for(int iF = 1; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++)
		{
			DbPoint oDbPointLeftTopTemp;
			DbPoint oDbPointRightBottomTemp;
			//
			p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iF);
			// Получаем крайние точки следующего элемента в группе.
			oDbPointLeftTopTemp.dbX = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX;
			oDbPointLeftTopTemp.dbY = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY;
			oQRectFTemp = p_GraphicsElementItem->boundingRect();
			oDbPointRightBottomTemp.dbX = oDbPointLeftTopTemp.dbX + oQRectFTemp.width();
			oDbPointRightBottomTemp.dbY = oDbPointLeftTopTemp.dbY + oQRectFTemp.height();
			// Расширяем до показателей текущего элемента при зашкаливании по любой точке периметра.
			if(oDbPointLeftTop.dbX > oDbPointLeftTopTemp.dbX) oDbPointLeftTop.dbX = oDbPointLeftTopTemp.dbX;
			if(oDbPointLeftTop.dbY > oDbPointLeftTopTemp.dbY) oDbPointLeftTop.dbY = oDbPointLeftTopTemp.dbY;
			if(oDbPointRightBottom.dbX < oDbPointRightBottomTemp.dbX) oDbPointRightBottom.dbX = oDbPointRightBottomTemp.dbX;
			if(oDbPointRightBottom.dbY < oDbPointRightBottomTemp.dbY) oDbPointRightBottom.dbY = oDbPointRightBottomTemp.dbY;
		}
	}
	else
	{
		if(!p_GraphicsGroupItem->vp_ConnectedGroups.isEmpty())
		{
			// Если нет элементов - начинаем с групп.
			p_GraphicsGroupItemInt = p_GraphicsGroupItem->vp_ConnectedGroups.at(0);
			// Получаем крайние точки первой группы в группе.
			oDbPointLeftTop.dbX = p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX;
			oDbPointLeftTop.dbY = p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY;
			oDbPointRightBottom.dbX = oDbPointLeftTop.dbX +
					p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW;
			oDbPointRightBottom.dbY = oDbPointLeftTop.dbY +
					p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH;
		}
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
		oDbPointLeftTopTemp.dbX = p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX;
		oDbPointLeftTopTemp.dbY = p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY;
		if(p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
		{
			oDbPointRightBottomTemp.dbX = oDbPointLeftTopTemp.dbX + dbMinGroupD;
			oDbPointRightBottomTemp.dbY = oDbPointLeftTopTemp.dbY + dbMinGroupD;
		}
		else
		{
			oDbPointRightBottomTemp.dbX = oDbPointLeftTopTemp.dbX +
					p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW;
			oDbPointRightBottomTemp.dbY = oDbPointLeftTopTemp.dbY +
					p_GraphicsGroupItemInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH;
		}
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
	p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame = oDbFrameResult;
	UpdateSelectedInGroup(p_GraphicsGroupItem, SCH_UPDATE_GROUP_FRAME | SCH_UPDATE_MAIN);
	p_GraphicsGroupItem->p_QLabel->setFixedWidth(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW - 6);
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
	double dbDiameterLast;
	//
	if(ushBits & SCH_UPDATE_ELEMENT_ZPOS)
	{
		p_GraphicsElementItem->setZValue(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos);
		UpdateLinksZPos();
	}
	if(ushBits & SCH_UPDATE_ELEMENT_FRAME)
	{
		dbDiameterLast = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW;
		if(!bIsIncoming)
		{
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW +=
					p_GraphicsElementItem->oDbPointDimIncrements.dbX;
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH +=
					p_GraphicsElementItem->oDbPointDimIncrements.dbY;
		}
		p_GraphicsElementItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX,
									  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY);
		QList<QGraphicsItem*> lp_Items = p_GraphicsElementItem->childItems();
		int iCn = lp_Items.count();
		for(int iC = 0; iC < iCn; iC++)
		{
			p_GraphicsItem = lp_Items.at(iC);
			if((p_GraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI) && (p_GraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_PORT))
			{
				DbFrame* p_DbFrameEl;
				//
				p_DbFrameEl = &p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame;
				p_GraphicsPortItemInt = (GraphicsPortItem*)p_GraphicsItem;
				if(p_GraphicsPortItemInt->bIsSrc)
				{
					oDbPointPortCurrent = p_GraphicsPortItemInt->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos;
				}
				else
				{
					oDbPointPortCurrent = p_GraphicsPortItemInt->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos;
				}
				if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
				{
					double dbRadiusLast = dbDiameterLast / 2.0f;
					double dbDiameterNow = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW;
					double dbRadiusNow = dbDiameterNow / 2.0f;
					double dbProp = dbDiameterNow / dbDiameterLast; // Пропорция изменений диаметра.
					// Перевод в коорд. центра объекта.
					oDbPointPortCurrent.dbX -= dbRadiusLast;
					oDbPointPortCurrent.dbY -= dbRadiusLast;
					// Изменение расст. от центра в пропорциях изменений пропорций скалера.
					oDbPointPortCurrent.dbX *= dbProp;
					oDbPointPortCurrent.dbY *= dbProp;
					// Возврат в коорд. верхнего левого угла объекта.
					oDbPointPortCurrent.dbX += dbRadiusNow;
					oDbPointPortCurrent.dbY += dbRadiusNow;
					// Точная коррекция.
					BindToEdge(p_GraphicsElementItem, oDbPointPortCurrent);
					SetPortToPos(p_GraphicsPortItemInt, oDbPointPortCurrent);
				}
				else
				{
					if(oDbPointPortCurrent.dbX > p_DbFrameEl->dbW)
					{
						oDbPointPortCurrent.dbX = p_DbFrameEl->dbW;
					}
					else
					{
						if(!bIsIncoming)
						{
							if((oDbPointPortCurrent.dbX > 0) && (oDbPointPortCurrent.dbY > 0))
							{
								oDbPointPortCurrent.dbX += p_GraphicsElementItem->oDbPointDimIncrements.dbX;
							}
						}
						else
						{
							if(oDbPointPortCurrent.dbX > 0)
							{
								oDbPointPortCurrent.dbY = 0;
							}
							if(oDbPointPortCurrent.dbY > 0)
							{
								oDbPointPortCurrent.dbX = 0;
							}
						}
					}
					if(oDbPointPortCurrent.dbY > p_DbFrameEl->dbH)
					{
						oDbPointPortCurrent.dbY = p_DbFrameEl->dbH;
					}
					else
					{
						if(!bIsIncoming)
						{
							if((oDbPointPortCurrent.dbX > 0) && (oDbPointPortCurrent.dbY > 0))
							{
								oDbPointPortCurrent.dbY += p_GraphicsElementItem->oDbPointDimIncrements.dbY;
							}
						}
						else
						{
							if(oDbPointPortCurrent.dbX > 0)
							{
								oDbPointPortCurrent.dbY = 0;
							}
							if(oDbPointPortCurrent.dbY > 0)
							{
								oDbPointPortCurrent.dbX = 0;
							}
						}
					}
					if(oDbPointPortCurrent.dbX < 0) oDbPointPortCurrent.dbX = 0;
					if(oDbPointPortCurrent.dbY < 0) oDbPointPortCurrent.dbY = 0;
					p_GraphicsPortItemInt->setPos(oDbPointPortCurrent.dbX, oDbPointPortCurrent.dbY);
				}
				if(p_GraphicsPortItemInt->bIsSrc)
				{
					p_GraphicsPortItemInt->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos = oDbPointPortCurrent;
				}
				else
				{
					p_GraphicsPortItemInt->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos = oDbPointPortCurrent;
				}
			}
		}
		if(!(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED))
		{
			WidgetsThrAccess::p_ConnGraphicsElementItem = p_GraphicsElementItem;
			ThrUiAccessET(MainWindow::p_WidgetsThrAccess, ElementGroupBoxSizeSet);
		}
		p_GraphicsElementItem->p_GraphicsScalerItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
															oSchEGGraph.oDbFrame.dbW,
															p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
															oSchEGGraph.oDbFrame.dbH);
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
														  oSchEGGraph.oDbFrame.dbX +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLGraph.oDbSrcPortGraphPos.dbX,
														  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
														  oSchEGGraph.oDbFrame.dbY +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLGraph.oDbSrcPortGraphPos.dbY);
							p_GraphicsPortItemInt->update();
						}
						if(p_SchLinkVars->ullIDDst == p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
						{
							p_GraphicsPortItemInt->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
														  oSchEGGraph.oDbFrame.dbX +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLGraph.oDbDstPortGraphPos.dbX,
														  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.
														  oSchEGGraph.oDbFrame.dbY +
														  p_GraphicsPortItemInt->p_PSchLinkVarsInt->
														  oSchLGraph.oDbDstPortGraphPos.dbY);
							p_GraphicsPortItemInt->update();
						}
					}
				}
			}
		}
	}
	if(ushBits & SCH_UPDATE_PORT_SRC_POS)
	{
		p_GraphicsPortItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbX,
								   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbY);
		p_GraphicsPortItem->update();
	}
	if(ushBits & SCH_UPDATE_PORT_DST_POS)
	{
		p_GraphicsPortItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbX,
								   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY +
								   p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbY);
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
		p_GraphicsGroupItem->setZValue(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos);
		UpdateLinksZPos();
	}
	if(ushBits & SCH_UPDATE_GROUP_FRAME)
	{
		p_GraphicsGroupItem->setPos(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX,
									p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY);
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

// Отпускание занятого клиентом.
void SchematicView::ReleaseOccupiedAPFS()
{
	PSchElementVars oPSchElementVars;
	PSchGroupVars oPSchGroupVars;
	//
	for(int iF = 0; iF != v_OccupiedByClient.count(); iF++)
	{
		EGPointersVariant oEGPointersVariant = v_OccupiedByClient.at(iF);
		//
		if(oEGPointersVariant.p_GraphicsElementItem != nullptr)
		{
			SetElementBlockingPattern(oEGPointersVariant.p_GraphicsElementItem, false);
			oPSchElementVars.ullIDInt = oEGPointersVariant.p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
			oPSchElementVars.oSchEGGraph.dbObjectZPos =
					oEGPointersVariant.p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos;
			ResetBits(oPSchElementVars.oSchEGGraph.uchSettingsBits, SCH_SETTINGS_EG_BIT_BUSY);
			oPSchElementVars.oSchEGGraph.oDbFrame =
					oEGPointersVariant.p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame;
			oPSchElementVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_ELEMENT_BIT_ZPOS |
														  SCH_CHANGES_ELEMENT_BIT_BUSY |
														  SCH_CHANGES_ELEMENT_BIT_FRAME;
			MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars, sizeof(PSchElementVars));
		}
		else
		{
			SetGroupBlockingPattern(oEGPointersVariant.p_GraphicsGroupItem, false);
			oPSchGroupVars.ullIDInt = oEGPointersVariant.p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
			oPSchGroupVars.oSchEGGraph.dbObjectZPos =
					oEGPointersVariant.p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos;
			ResetBits(oPSchGroupVars.oSchEGGraph.uchSettingsBits, SCH_SETTINGS_EG_BIT_BUSY);
			oEGPointersVariant.p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX =
					oEGPointersVariant.p_GraphicsGroupItem->x();
			oEGPointersVariant.p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY =
					oEGPointersVariant.p_GraphicsGroupItem->y();
			oPSchGroupVars.oSchEGGraph.oDbFrame =
					oEGPointersVariant.p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame;
			oPSchGroupVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_GROUP_BIT_BUSY | SCH_CHANGES_GROUP_BIT_FRAME | SCH_CHANGES_GROUP_BIT_ZPOS;
			MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars, sizeof(PSchGroupVars));
		}
	}
	v_OccupiedByClient.clear();
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
void SchematicView::AddFreeSelectedElementsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
													   GraphicsElementItem* p_GraphicsElementItemInitial)
{
	GraphicsElementItem* p_GraphicsElementItem;
	bool bAction;
	//
	if(SchematicWindow::vp_SelectedFreeElements.isEmpty()) return;
	if(p_GraphicsElementItemInitial == nullptr)
	{
		p_GraphicsElementItemInitial = SchematicWindow::vp_SelectedElements.at(0);
	}
	TempSelectElement(p_GraphicsElementItemInitial);
	bAction = false;
	for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
	{
		p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iF);
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup == 0)
		{
			if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup !=
			   p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt)
			{
				p_GraphicsGroupItem->vp_ConnectedElements.push_front(p_GraphicsElementItem);
				p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup =
						p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
				bAction = true;
			}
		}
	}
	if(bAction)
	{
		UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
		BlockingVerticalsAndPopupElement(p_GraphicsElementItemInitial, p_GraphicsGroupItem,
										 SEND_GROUP, SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
										 ADD_SEND_ZPOS, ADD_SEND_FRAME, SEND_ELEMENTS, DONT_AFFECT_SELECTED);
		UpdateLinksZPos();
	}
	TempDeselectElement(p_GraphicsElementItemInitial);
}

void SchematicView::AddFreeSelectedGroupsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem)
{
	GraphicsGroupItem* p_GraphicsGroupItemHelper;
	bool bAction;
	//
	if(SchematicWindow::vp_SelectedFreeGroups.isEmpty()) return;
	bAction = false;
	for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
	{
		p_GraphicsGroupItemHelper = SchematicWindow::vp_SelectedGroups.at(iF);
		if((p_GraphicsGroupItemHelper != p_GraphicsGroupItem) & (p_GraphicsGroupItemHelper->p_GraphicsGroupItemRel == nullptr))
		{
			p_GraphicsGroupItem->vp_ConnectedGroups.push_front(p_GraphicsGroupItemHelper);
			p_GraphicsGroupItemHelper->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
			p_GraphicsGroupItemHelper->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup =
					p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
			bAction = true;
		}
	}
	if(bAction)
	{
		UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
		BlockingVerticalsAndPopupGroup(p_GraphicsGroupItem, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, SEND_NEW_GROUPS_TO_GROUP,
									   ADD_SEND_ZPOS, ADD_SEND_FRAME, SEND_ELEMENTS);
		UpdateLinksZPos();
	}
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
	BlockingVerticalsAndPopupElement(p_GraphicsElementItem, p_GraphicsElementItem->p_GraphicsGroupItemRel,
									 SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
									 DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
									 DONT_ADD_SEND_FRAME, SEND_ELEMENTS);
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
	BlockingVerticalsAndPopupElement(p_GraphicsElementItem, p_GraphicsElementItem->p_GraphicsGroupItemRel,
									 SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
									 DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
									 DONT_ADD_SEND_FRAME, SEND_ELEMENTS);
	p_GraphicsElementItem->bSelected = false;
}

// Сортировка векторов элементов и групп по Z-позиции с приоритетом по выборке.
void SchematicView::SortObjectsByZPos(QVector<GraphicsElementItem*>& avp_Elements, GraphicsElementItem* p_GraphicsElementItemExclude,
									  QVector<GraphicsGroupItem*>& avp_Groups, GraphicsGroupItem* p_GraphicsGroupItemExclude,
									  QVector<EGPointersVariant>* pv_EGPointersVariants)
{
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItem;
	QVector<EGPointersVariant> v_PointersVariants;
	EGPointersVariant oEGPointersVariant;
	const EGPointersVariant* p_EGPointersVariant;
	//
	// Копия для работы.
	for(int iF = 0; iF < avp_Elements.count(); iF++)
	{
		p_GraphicsElementItem = avp_Elements.at(iF);
		if(p_GraphicsElementItem != p_GraphicsElementItemExclude)
		{
			oEGPointersVariant.p_GraphicsElementItem = p_GraphicsElementItem;
			oEGPointersVariant.p_GraphicsGroupItem = nullptr;
			v_PointersVariants.append(oEGPointersVariant);
		}
	}
	for(int iF = 0; iF < avp_Groups.count(); iF++)
	{
		p_GraphicsGroupItem = avp_Groups.at(iF);
		if(p_GraphicsGroupItem != p_GraphicsGroupItemExclude)
		{
			oEGPointersVariant.p_GraphicsElementItem = nullptr;
			oEGPointersVariant.p_GraphicsGroupItem = p_GraphicsGroupItem;
			v_PointersVariants.append(oEGPointersVariant);
		}
	}
	// Сортировка.
	while(!v_PointersVariants.isEmpty())
	{
		int iVariant;
		qreal dbZ = OVERMAX_NUMBER;
		for(int iF = 0; iF < v_PointersVariants.count(); iF++)
		{
			p_EGPointersVariant = &v_PointersVariants.at(iF);
			if(p_EGPointersVariant->p_GraphicsElementItem != nullptr)
			{
				if(dbZ > p_EGPointersVariant->p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos)
				{
					dbZ = p_EGPointersVariant->p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos;
					iVariant = iF;
				}
			}
			else
			{
				if(dbZ > p_EGPointersVariant->p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos)
				{
					dbZ = p_EGPointersVariant->p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos;
					iVariant = iF;
				}
			}
		}
		p_EGPointersVariant = &v_PointersVariants.at(iVariant);
		pv_EGPointersVariants->append(*p_EGPointersVariant);
		v_PointersVariants.removeAt(iVariant);
	}
}

// Поднятие Z-значений ветки группы с фокусом и очистка списка.
void SchematicView::PullUpZOfBranch(QVector<GraphicsGroupItem*>& avp_GraphicsGroupItemsBranch)
{
	while(!avp_GraphicsGroupItemsBranch.isEmpty()) // Заранее подмнимаем ветку группы, что в фокусе.
	{
		GraphicsGroupItem* p_GraphicsGroupItemHelper = avp_GraphicsGroupItemsBranch.constLast();
		p_GraphicsGroupItemHelper->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
		p_GraphicsGroupItemHelper->setZValue(SchematicWindow::dbObjectZPos);
		SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
		avp_GraphicsGroupItemsBranch.removeAt(avp_GraphicsGroupItemsBranch.count() - 1);
	}
}

// Блокировка вертикалей и поднятие выбранного элемента.
void SchematicView::BlockingVerticalsAndPopupElement(GraphicsElementItem* p_GraphicsElementItem, GraphicsGroupItem* p_GraphicsGroupItem,
													 bool bSend, bool bAddNewElementsToGroupSending, bool bAddNewGroupsToGroupSending,
													 bool bAddBusyOrZPosToSending, bool bAddFrame, bool bSendElements, bool bAffectSelected)
{
	GraphicsGroupItem* p_GraphicsGroupItemRoot = p_GraphicsGroupItem;
	QVector<GraphicsGroupItem*> vp_GraphicsGroupItemsFocusedBranch;
	QVector<GraphicsGroupItem*> vp_GroupsWithSelectedElements;
	// Вниз, к корню группы в фокусе.
	if(p_GraphicsGroupItem != nullptr)
	{
		while(p_GraphicsGroupItemRoot->p_GraphicsGroupItemRel != nullptr)
		{
			p_GraphicsGroupItemRoot = p_GraphicsGroupItemRoot->p_GraphicsGroupItemRel;
			vp_GraphicsGroupItemsFocusedBranch.append(p_GraphicsGroupItemRoot);
		}
	}
	// По остальным корням групп... !!! ВОЗМОЖНО, ПОТРЕБУЕТСЯ СОРТИРОВКА КОРНЕЙ !!!
	if(p_GraphicsElementItem->bSelected & bAffectSelected)
	{
		for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
		{
			GraphicsElementItem* p_GraphicsElementItemHelper = SchematicWindow::vp_SelectedElements.at(iF);
			//
			if(p_GraphicsElementItemHelper->p_GraphicsGroupItemRel != nullptr)
			{
				if(!vp_GroupsWithSelectedElements.contains(p_GraphicsElementItemHelper->p_GraphicsGroupItemRel))
				{
					vp_GroupsWithSelectedElements.append(p_GraphicsElementItemHelper->p_GraphicsGroupItemRel);
				}
			}
			else
			{
				if(p_GraphicsElementItemHelper != p_GraphicsElementItem)
				{
					ElementToTopOrBusyAPFS(p_GraphicsElementItemHelper, DONT_SEND_ELEMENT_GROUP_CHANGE, bAddBusyOrZPosToSending,
										   bSendElements, LEAVE_IN_PLACE);
				}
			}
		}
		for(int iF = 0; iF != vp_GroupsWithSelectedElements.count(); iF++)
		{
			GraphicsGroupItem* p_GraphicsGroupItemTempRoot = vp_GroupsWithSelectedElements.at(iF);
			while(p_GraphicsGroupItemTempRoot->p_GraphicsGroupItemRel != nullptr)
			{
				p_GraphicsGroupItemTempRoot = p_GraphicsGroupItemTempRoot->p_GraphicsGroupItemRel;
			}
			if(p_GraphicsGroupItemTempRoot != p_GraphicsGroupItemRoot) // Кроме корня группы в фокусе - его в последнюю очередь.
			{
				GroupsBranchToTopAPFSRecursively(p_GraphicsGroupItemTempRoot, bSend,
												 bAddNewElementsToGroupSending, bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
												 bAddFrame, p_GraphicsElementItem, nullptr, bSendElements, LEAVE_IN_PLACE);
			}
		}
	}
	if(p_GraphicsGroupItem != nullptr)
	{
		PullUpZOfBranch(vp_GraphicsGroupItemsFocusedBranch);
		GroupsBranchToTopAPFSRecursively(p_GraphicsGroupItemRoot, bSend, bAddNewElementsToGroupSending, bAddNewGroupsToGroupSending,
										 bAddBusyOrZPosToSending, bAddFrame, p_GraphicsElementItem, p_GraphicsGroupItem, bSendElements);
		GroupsBranchToTopAPFSRecursively(p_GraphicsGroupItem, bSend, bAddNewElementsToGroupSending,
										 bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
										 bAddFrame, p_GraphicsElementItem, nullptr, bSendElements);
	}
	ElementToTopOrBusyAPFS(p_GraphicsElementItem, bAddNewElementsToGroupSending, bAddBusyOrZPosToSending, bSendElements);
}

// Блокировка вертикалей и поднятие выбранной группы.
void SchematicView::BlockingVerticalsAndPopupGroup(GraphicsGroupItem* p_GraphicsGroupItem,
												   bool bSend, bool bAddNewElementsToGroupSending, bool bAddNewGroupsToGroupSending,
												   bool bAddBusyOrZPosToSending, bool bAddFrame, bool bSendElements)
{
	GraphicsGroupItem* p_GraphicsGroupItemRoot = p_GraphicsGroupItem;
	QVector<GraphicsGroupItem*> vp_GraphicsGroupItemsFocusedBranch;
	bool bGroupSelected = false;
	// Вниз, к корню группы в фокусе.
	bGroupSelected = p_GraphicsGroupItem->bSelected;
	while(p_GraphicsGroupItemRoot->p_GraphicsGroupItemRel != nullptr)
	{
		p_GraphicsGroupItemRoot = p_GraphicsGroupItemRoot->p_GraphicsGroupItemRel;
		vp_GraphicsGroupItemsFocusedBranch.append(p_GraphicsGroupItemRoot);
	}
	// По остальным корням групп... !!! ВОЗМОЖНО, ПОТРЕБУЕТСЯ СОРТИРОВКА КОРНЕЙ !!!
	if(bGroupSelected)
	{
		for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
		{
			GraphicsGroupItem* p_GraphicsGroupItemTempRoot = SchematicWindow::vp_SelectedGroups.at(iF);
			while(p_GraphicsGroupItemTempRoot->p_GraphicsGroupItemRel != nullptr)
			{
				p_GraphicsGroupItemTempRoot = p_GraphicsGroupItemTempRoot->p_GraphicsGroupItemRel;
			}
			if(p_GraphicsGroupItemTempRoot != p_GraphicsGroupItemRoot) // Кроме корня группы в фокусе - его в последнюю очередь.
			{
				GroupsBranchToTopAPFSRecursively(p_GraphicsGroupItemTempRoot, bSend,
												 bAddNewElementsToGroupSending, bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
												 bAddFrame, nullptr, nullptr, bSendElements, LEAVE_IN_PLACE);
			}
		}
	}
	PullUpZOfBranch(vp_GraphicsGroupItemsFocusedBranch);
	GroupsBranchToTopAPFSRecursively(p_GraphicsGroupItemRoot, bSend, bAddNewElementsToGroupSending, bAddNewGroupsToGroupSending,
									 bAddBusyOrZPosToSending, bAddFrame, nullptr, p_GraphicsGroupItem, bSendElements);
	GroupsBranchToTopAPFSRecursively(p_GraphicsGroupItem, bSend, bAddNewElementsToGroupSending,
									 bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
									 bAddFrame, nullptr, nullptr, bSendElements);
}

// Поднятие ветки групп на первый план и подготовка к отсылке по запросу рекурсивно.
void SchematicView::GroupsBranchToTopAPFSRecursively(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend,
													 bool bAddNewElementsToGroupSending, bool bAddNewGroupsToGroupSending,
													 bool bAddBusyOrZPosToSending, bool bAddFrame,
													 GraphicsElementItem* p_GraphicsElementItemExclude,
													 GraphicsGroupItem* p_GraphicsGroupItemExclude,
													 bool bSendElements, bool bToTop)
{
	PSchGroupVars oPSchGroupVars;
	EGPointersVariant oEGPointersVariant;
	QVector<EGPointersVariant> v_EGPointersVariants;
	const EGPointersVariant* p_EGPointersVariant;
	//
	if(p_GraphicsGroupItem == p_GraphicsGroupItemExclude) return;
	if(bToTop)
	{
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
		p_GraphicsGroupItem->setZValue(SchematicWindow::dbObjectZPos);
		SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
		p_GraphicsGroupItem->update();
	}
	if(bSend)
	{
		oPSchGroupVars.ullIDInt = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
		if(bAddBusyOrZPosToSending)
		{
			oPSchGroupVars.oSchEGGraph.uchSettingsBits |= SCH_SETTINGS_EG_BIT_BUSY;
			oPSchGroupVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_GROUP_BIT_BUSY;
		}
		else
		{
			oPSchGroupVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_GROUP_BIT_ZPOS;
			oPSchGroupVars.oSchEGGraph.dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos;
		}
		if(bAddFrame)
		{
			oPSchGroupVars.oSchEGGraph.uchChangesBits |= SCH_CHANGES_GROUP_BIT_FRAME;
			oPSchGroupVars.oSchEGGraph.oDbFrame = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame;
		}
		if(bAddNewGroupsToGroupSending)
		{
			if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup != 0)
			{
				oPSchGroupVars.oSchEGGraph.uchChangesBits |= SCH_CHANGES_GROUP_BIT_GROUP;
				oPSchGroupVars.ullIDGroup = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup;
			}
		}
		MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars,
													   sizeof(PSchGroupVars));
	}
	oEGPointersVariant.p_GraphicsElementItem = nullptr;
	oEGPointersVariant.p_GraphicsGroupItem = p_GraphicsGroupItem;
	if(bAddBusyOrZPosToSending == ADD_SEND_BUSY) v_OccupiedByClient.append(oEGPointersVariant);
	SetGroupBlockingPattern(p_GraphicsGroupItem, bAddBusyOrZPosToSending);
	if(bSend == false) bSendElements = false;
	// Сортировка.
	SortObjectsByZPos(p_GraphicsGroupItem->vp_ConnectedElements, p_GraphicsElementItemExclude,
					  p_GraphicsGroupItem->vp_ConnectedGroups, p_GraphicsGroupItemExclude,
					  &v_EGPointersVariants);
	//
	for(int iF = 0; iF != v_EGPointersVariants.count(); iF++)
	{
		p_EGPointersVariant = &v_EGPointersVariants.at(iF);
		if(p_EGPointersVariant->p_GraphicsElementItem != nullptr)
		{
			ElementToTopOrBusyAPFS(p_EGPointersVariant->p_GraphicsElementItem, bAddNewElementsToGroupSending,
							 bAddBusyOrZPosToSending, bSendElements, bToTop);
		}
		else
		{
			GroupsBranchToTopAPFSRecursively(p_EGPointersVariant->p_GraphicsGroupItem, bSend, bAddNewElementsToGroupSending,
										   bAddNewGroupsToGroupSending, bAddBusyOrZPosToSending,
										   DONT_ADD_SEND_FRAME, nullptr, p_GraphicsGroupItemExclude, SEND_ELEMENTS, bToTop);
		}
	}
	UpdateLinksZPos();
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
	BlockingVerticalsAndPopupGroup(p_GraphicsGroupItem, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
					  DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY,
					  DONT_ADD_SEND_FRAME, SEND_ELEMENTS);
	p_GraphicsGroupItem->bSelected = true;
}

// Отмена выбора группы.
void SchematicView::DeselectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState)
{
	if(bLastState != p_GraphicsGroupItem->bSelected)
	{
		if(SchematicWindow::vp_SelectedGroups.contains(p_GraphicsGroupItem))
		{
			SchematicWindow::vp_SelectedGroups.removeAll(p_GraphicsGroupItem);
			p_GraphicsGroupItem->p_GraphicsFrameItem->hide(); // Гасим рамку.
		}
	}
	BlockingVerticalsAndPopupGroup(p_GraphicsGroupItem, SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP,
					  DONT_SEND_NEW_GROUPS_TO_GROUP, ADD_SEND_BUSY, SEND_ELEMENTS);
	p_GraphicsGroupItem->bSelected = false;
}

// Обновление вертикали фреймов групп рекурсивно.
void SchematicView::UpdateVerticalOfGroupFramesRecursively(GraphicsGroupItem* p_GraphicsGroupItem)
{
	GraphicsGroupItem* p_GraphicsGroupItemHelper;
	//
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedGroups.count(); iF++)
	{
		p_GraphicsGroupItemHelper = p_GraphicsGroupItem->vp_ConnectedGroups.at(iF);
		UpdateVerticalOfGroupFramesRecursively(p_GraphicsGroupItemHelper);
	}
	if(p_GraphicsGroupItem->p_GraphicsGroupItemRel != nullptr)
	{
		UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem->p_GraphicsGroupItemRel);
	}
}

// Перемещение группы рекурсивно.
void SchematicView::MoveGroupRecursively(GraphicsGroupItem* p_GraphicsGroupItem, QPointF& a_QPointFRes, bool bMoveBody)
{
	GraphicsElementItem* p_GraphicsElementItem;
	GraphicsGroupItem* p_GraphicsGroupItemHelper;
	//
	if(!v_AlreadyMovedGroups.contains(p_GraphicsGroupItem))
	{
		if(bMoveBody)
		{
			p_GraphicsGroupItem->setPos(p_GraphicsGroupItem->x() + a_QPointFRes.x(), p_GraphicsGroupItem->y() + a_QPointFRes.y());
		}
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX += a_QPointFRes.x();
		p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY += a_QPointFRes.y();
		UpdateSelectedInGroup(p_GraphicsGroupItem, SCH_UPDATE_MAIN);
		//
		for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedElements.count(); iF++)
		{
			p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iF);
			p_GraphicsElementItem->setPos(p_GraphicsElementItem->x() + a_QPointFRes.x(), p_GraphicsElementItem->y() + a_QPointFRes.y());
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX += a_QPointFRes.x();
			p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY += a_QPointFRes.y();
			UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
		}
		v_AlreadyMovedGroups.append(p_GraphicsGroupItem);
	}
	for(int iF = 0; iF < p_GraphicsGroupItem->vp_ConnectedGroups.count(); iF++)
	{
		p_GraphicsGroupItemHelper = p_GraphicsGroupItem->vp_ConnectedGroups.at(iF);
		MoveGroupRecursively(p_GraphicsGroupItemHelper, a_QPointFRes, true);
	}
	if(p_GraphicsGroupItem->p_GraphicsGroupItemRel != nullptr)
	{
		UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem->p_GraphicsGroupItemRel);
	}
}

// Обновление Z-позиции линка по данным элементов.
void SchematicView::UpdateLinkZPositionByElements(GraphicsLinkItem* p_GraphicsLinkItem)
{
	if(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos >
	   p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos)
	{
		p_GraphicsLinkItem->setZValue(p_GraphicsLinkItem->p_GraphicsElementItemSrc->
									  oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos + SCH_LINK_Z_SHIFT);
	}
	else
	{
		p_GraphicsLinkItem->setZValue(p_GraphicsLinkItem->p_GraphicsElementItemDst->
									  oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos + SCH_LINK_Z_SHIFT);
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

// Установка порта в позицию.
void SchematicView::SetPortToPos(GraphicsPortItem* p_GraphicsPortItem, DbPoint dbPortPos)
{
	p_GraphicsPortItem->setPos(dbPortPos.dbX, dbPortPos.dbY);
	if(p_GraphicsPortItem->bIsSrc)
	{
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbX = dbPortPos.dbX;
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbY = dbPortPos.dbY;
	}
	else
	{
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbX = dbPortPos.dbX;
		p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbY = dbPortPos.dbY;
	}
	UpdateSelectedInElement(p_GraphicsPortItem->p_ParentInt,
							SCH_UPDATE_LINK_POS | SCH_UPDATE_MAIN, nullptr, p_GraphicsPortItem->p_GraphicsLinkItemInt);
}

// Действия при совместном нажатии кнопок мыши.
bool SchematicView::DoubleButtonsPressControl(QGraphicsSceneMouseEvent* p_Event)
{
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		bLMBPressed = true;
		if(bRMBPressed) return true;
	} else if(p_Event->button() == Qt::MouseButton::RightButton)
	{
		bRMBPressed = true;
		if(bLMBPressed) return true;
	}
	return false;
}

// Действия при отпускании кнопок мыши после совместного нажатия.
bool SchematicView::DoubleButtonsReleaseControl()
{
	if(bRMBPressed)
	{
		bRMBPressed = false;
		if(bLMBPressed) return true;
	}
	bLMBPressed = false;
	return false;
}

// Обработчик события нажатия мыши на элемент.
void SchematicView::ElementMousePressEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event)
{
	bool bLastSt;
	//
	SchematicWindow::ResetMenu();
	if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return; //Если элемент блокирован занятостью, смещением выборки или главным окном - отказ.
	}
	if(DoubleButtonsPressControl(p_Event)) // Переключение минимизации.
	{
		unsigned char uchMinStatus = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
									 SCH_SETTINGS_EG_BIT_MIN;
		//
		bLastSt = p_GraphicsElementItem->bSelected; // Запоминаем текущее значение выбраности.
		if(!SchematicWindow::vp_SelectedElements.contains(p_GraphicsElementItem)) // Если не было выбрано - добавляем для массовых действий.
		{
			SchematicWindow::vp_SelectedElements.push_front(p_GraphicsElementItem);
		}
		// Обработка статусов минимизации.
		for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++) // По всем причастным.
		{
			GraphicsElementItem* p_GraphicsElementItemCurrent = SchematicWindow::vp_SelectedElements.at(iF);
			//
			if((p_GraphicsElementItemCurrent->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN) ==
			   uchMinStatus)
			{
				QList<QGraphicsItem*> lp_Items = p_GraphicsElementItemCurrent->childItems();
				int iCn = lp_Items.count();
				bool bPortsPresent = false;
				//
				p_GraphicsElementItemCurrent->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits ^= SCH_SETTINGS_EG_BIT_MIN;
				p_GraphicsElementItemCurrent->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_ELEMENT_BIT_MIN;
				MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_ELEMENT_VARS,
															   (char*)&p_GraphicsElementItemCurrent->oPSchElementBaseInt.oPSchElementVars,
															   sizeof(p_GraphicsElementItemCurrent->oPSchElementBaseInt.oPSchElementVars));
				for(int iC = 0; iC < iCn; iC++)
				{
					GraphicsPortItem* p_GraphicsPortItemInt;
					DbPoint oDbPoint;
					QGraphicsItem* p_GraphicsItem;
					//
					p_GraphicsItem = lp_Items.at(iC);
					if(p_GraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI)
					{
						if(p_GraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_PORT)
						{
							bPortsPresent = true;
							p_GraphicsPortItemInt = (GraphicsPortItem*)p_GraphicsItem;
							oDbPoint = p_GraphicsPortItemInt->oDbPAlterMinPos;
							p_GraphicsPortItemInt->oDbPAlterMinPos.dbX = p_GraphicsPortItemInt->pos().x();
							p_GraphicsPortItemInt->oDbPAlterMinPos.dbY = p_GraphicsPortItemInt->pos().y();
							SetPortToPos(p_GraphicsPortItemInt, oDbPoint);
							SetHidingStatus(p_GraphicsPortItemInt, p_GraphicsElementItemCurrent->oPSchElementBaseInt.
											oPSchElementVars.oSchEGGraph.uchSettingsBits &
											SCH_SETTINGS_EG_BIT_MIN);
						}
					}
				}
				p_GraphicsElementItemCurrent->bPortsForMin = bPortsPresent;
				if(p_GraphicsElementItemCurrent->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
				{
					p_GraphicsElementItemCurrent->p_GraphicsScalerItem->hide();
					if(p_GraphicsElementItemCurrent->p_QGroupBox) p_GraphicsElementItemCurrent->p_QGroupBox->hide();
				}
				else
				{
					p_GraphicsElementItemCurrent->p_GraphicsScalerItem->show();
					if(p_GraphicsElementItemCurrent->p_QGroupBox) p_GraphicsElementItemCurrent->p_QGroupBox->show();
				}
				if(p_GraphicsElementItemCurrent->p_GraphicsGroupItemRel)
				{
					UpdateGroupFrameByContentRecursively(p_GraphicsElementItemCurrent->p_GraphicsGroupItemRel);
				}
			}
		}
		//
		if(!bLastSt) // Если был не выбран и добавлялся для массовых действий - удаление из списка выбранных.
		{
			SchematicWindow::vp_SelectedElements.removeAll(p_GraphicsElementItem);
		}
		goto gM;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		// Создание нового порта.
		if((p_Event->modifiers() == Qt::AltModifier) &&
		   !(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN))
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
			oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos = oDbPointInitialClick;
			oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos.dbX -=
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX;
			oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos.dbY -=
					p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY;
			oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos = oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos;
			oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos =
					BindToEdge(p_GraphicsElementItem, oPSchLinkBase.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos);
			// Создание замкнутого линка (пока что).
			p_GraphicsLinkItemNew = new GraphicsLinkItem(&oPSchLinkBase);
			if(oPSchLinkBase.oPSchLinkVars.oSchLGraph.uchChangesBits != SCH_CHANGES_LINK_BIT_INIT_ERROR)
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
			// Создать группу из выбранного.
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
																   " [" +
																   QString(SchematicWindow::vp_SelectedGroups.at(0)->
																		   oPSchGroupBaseInt.m_chName)
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
			bElementMenuReady = true;
		}
	}
gM:	TrySendBufferToServer;
	p_GraphicsElementItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с элементом.
void SchematicView::ElementMouseMoveEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event)
{
	int iC;
	QPointF oQPointFInit;
	QPointF oQPointFRes;
	//
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier || bRMBPressed)
	{
		return;
	}
	if(p_GraphicsLinkItemNew != nullptr)
	{
		p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mouseMoveEvent(p_Event);
		return;
	}
	else if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY) return;
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
				p_GraphicsElementItemUtil->setPos(p_GraphicsElementItemUtil->x() + oQPointFRes.x(),
												  p_GraphicsElementItemUtil->y() + oQPointFRes.y());
				p_GraphicsElementItemUtil->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX += oQPointFRes.x();
				p_GraphicsElementItemUtil->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY += oQPointFRes.y();
				UpdateSelectedInElement(p_GraphicsElementItemUtil, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
			}
		}
	}
	p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX = p_GraphicsElementItem->x();
	p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY = p_GraphicsElementItem->y();
	UpdateSelectedInElement(p_GraphicsElementItem, SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN | SCH_UPDATE_GROUP);
}

// Создание группы с выбранным.
void SchematicView::CreateGroupFromSelected()
{
	GraphicsGroupItem* p_GraphicsGroupItem;
	PSchGroupBase oPSchGroupBase;
	QString strName = QString(m_chNewGroup);
	GraphicsElementItem* p_GraphicsElementItemUtil;
	GraphicsGroupItem* p_GraphicsGroupItemUtil;
	unsigned char uchR = rand() % 255;
	unsigned char uchG = rand() % 255;
	unsigned char uchB = rand() % 255;
	unsigned char uchA = 200;
	//
	oPSchGroupBase.oPSchGroupVars.ullIDInt = GenerateID();
	strName += ": " + QString::number(oPSchGroupBase.oPSchGroupVars.ullIDInt);
	CopyStrArray((char*)strName.toStdString().c_str(), oPSchGroupBase.m_chName, SCH_OBJ_NAME_STR_LEN);
	oPSchGroupBase.oPSchGroupVars.oSchEGGraph.dbObjectZPos = SchematicWindow::dbObjectZPos;
	SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
	oPSchGroupBase.uiObjectBkgColor = QColor(uchR, uchG, uchB, uchA).rgba();
	ResetBits(oPSchGroupBase.oPSchGroupVars.oSchEGGraph.uchSettingsBits, SCH_SETTINGS_EG_BIT_BUSY);
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
		}
	}
	for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
	{
		p_GraphicsGroupItemUtil = SchematicWindow::vp_SelectedGroups.at(iF);
		if(p_GraphicsGroupItemUtil != p_GraphicsGroupItem)
		{
			if(p_GraphicsGroupItemUtil->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup == 0)
			{
				p_GraphicsGroupItemUtil->oPSchGroupBaseInt.oPSchGroupVars.ullIDGroup = oPSchGroupBase.oPSchGroupVars.ullIDInt;
				p_GraphicsGroupItemUtil->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
				p_GraphicsGroupItem->vp_ConnectedGroups.append(p_GraphicsGroupItemUtil);
			}
		}
	}
	UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
	oPSchGroupBase.oPSchGroupVars.oSchEGGraph.oDbFrame =
			p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame;
	MainWindow::p_Client->AddPocketToOutputBufferC(
				PROTO_O_SCH_GROUP_BASE, (char*)&oPSchGroupBase, sizeof(PSchGroupBase));
	BlockingVerticalsAndPopupGroup(p_GraphicsGroupItem, SEND_GROUP, SEND_NEW_ELEMENTS_TO_GROUP, SEND_NEW_GROUPS_TO_GROUP,
								   ADD_SEND_ZPOS, ADD_SEND_FRAME, SEND_ELEMENTS);
	UpdateLinksZPos();
}

// Обработчик события отпусканеия мыши на элементе.
void SchematicView::ElementMouseReleaseEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	DoubleButtonsReleaseControl();
	if(p_GraphicsLinkItemNew != nullptr)
	{
		p_GraphicsLinkItemNew->p_GraphicsPortItemDst->mouseReleaseEvent(p_Event);
		p_GraphicsLinkItemNew = nullptr;
		p_GraphicsElementItem->OBMouseReleaseEvent(p_Event);
		return;
	}
	else if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY) return;
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		ReleaseOccupiedAPFS();
		TrySendBufferToServer;
	}
	p_GraphicsElementItem->OBMouseReleaseEvent(p_Event);
	if((SchematicWindow::p_SafeMenu != nullptr) && bElementMenuReady)
	{
		QAction* p_SelectedMenuItem;
		Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog;
		PSchElementName oPSchElementName;
		char m_chName[SCH_OBJ_NAME_STR_LEN];
		//
		bElementMenuReady = false;
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
				TempSelectElement(p_GraphicsElementItem);
				CreateGroupFromSelected();
				TempDeselectElement(p_GraphicsElementItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_SELECTED)
			{
				AddFreeSelectedElementsToGroupAPFS(SchematicWindow::vp_SelectedGroups.at(0), p_GraphicsElementItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_REMOVE_FROM_GROUPS)
			{
				TempSelectElement(p_GraphicsElementItem);
				DetachSelectedAPFS();
				TempDeselectElement(p_GraphicsElementItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_CHANGE_BACKGROUND)
			{

			}
			TrySendBufferToServer;
		}
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
	if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		QTextOption oQTextOption;
		QString strU;
		QChar qchF = 0x00AF;
		double dbR = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2.0f;
		// Подготовка имени.
		oQTextOption.setAlignment(Qt::AlignCenter);
		QString strName = QString(p_GraphicsElementItem->oPSchElementBaseInt.m_chName) + "\n";
		strU.fill(qchF, strName.length() + 2);
		strName += strU;
		//
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
			   SCH_SETTINGS_EG_BIT_MIN)
			{
				p_Painter->drawEllipse(QPointF(dbMinCircleR, dbMinCircleR), dbMinCircleR, dbMinCircleR);
				if(p_GraphicsElementItem->bPortsForMin)
				{
					p_Painter->setBrush(SchematicWindow::oQBrushGray);
					p_Painter->setPen(SchematicWindow::oQPenWhite);
					p_Painter->drawEllipse(QPointF(dbMinCircleR, dbMinCircleR), PORT_DIM, PORT_DIM);
				}
			}
			else
			{
				p_Painter->drawEllipse(QPointF(dbR, dbR), dbR, dbR);
				p_Painter->drawText(QRectF(5.0f, 0,
										   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW - 10.0f,
										   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW + 10.0f),
									strName, oQTextOption);
			}
		}
		else
		{
			QPolygonF oQPolygonFForTriangle;
			double dbDecr = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / TRIANGLE_DECR_PROPORTION;
			//
			if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
			   SCH_SETTINGS_EG_BIT_MIN)
			{
				oQPolygonFForTriangle.append(pntMinTrR);
				oQPolygonFForTriangle.append(pntMinTrT);
				oQPolygonFForTriangle.append(pntMinTrL);
				p_Painter->drawConvexPolygon(oQPolygonFForTriangle);
				if(p_GraphicsElementItem->bPortsForMin)
				{
					p_Painter->setBrush(SchematicWindow::oQBrushGray);
					p_Painter->setPen(SchematicWindow::oQPenWhite);
					p_Painter->drawEllipse(QPointF(dbMinTriangleRSubMinTriangleDerc, dbMinTriangleR), PORT_DIM, PORT_DIM);
				}
			}
			else
			{
				oQPolygonFForTriangle.append(QPointF(dbR + (pntTrR.x() * dbR) - dbDecr, dbR + (pntTrR.y() * dbR)));
				oQPolygonFForTriangle.append(QPointF(dbR + (pntTrT.x() * dbR) - dbDecr, dbR + (pntTrT.y() * dbR)));
				oQPolygonFForTriangle.append(QPointF(dbR + (pntTrL.x() * dbR) - dbDecr, dbR + (pntTrL.y() * dbR)));
				p_Painter->drawConvexPolygon(oQPolygonFForTriangle);
				p_Painter->drawText(QRectF((dbR / 3.0f) + 5.0f, 0,
										   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW -
										   (dbR / 1.5f) -
										   (dbDecr * 2.0f) - 10.f,
										   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW + 10.0f),
									strName, oQTextOption);
			}
		}
	}
	else
	{
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
		{
			p_Painter->drawRect(QRectF(0, 0, dbMinElementD, dbMinElementD));
			if(p_GraphicsElementItem->bPortsForMin)
			{
				p_Painter->setBrush(SchematicWindow::oQBrushGray);
				p_Painter->setPen(SchematicWindow::oQPenWhite);
				p_Painter->drawEllipse(QPointF(dbMinElementR, dbMinElementR), PORT_DIM, PORT_DIM);
			}
		}
		else
		{
			p_Painter->drawRect(QRectF(0, 0,
								p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
								p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH));
		}
	}
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
	p_GraphicsElementItem->bPortsForMin = false;
	// Группировщик для стандартного элемента.
	if(!(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED))
	{
		p_GraphicsElementItem->p_QGroupBox = new QGroupBox();
		QVBoxLayout* p_QVBoxLayout = new QVBoxLayout;
		p_GraphicsElementItem->p_QGroupBox->setLayout(p_QVBoxLayout);
		p_GraphicsElementItem->p_QGroupBox->setTitle(p_PSchElementBase->m_chName);
		p_GraphicsElementItem->p_QGroupBox->setAttribute(Qt::WA_TranslucentBackground);
		p_GraphicsElementItem->p_QGroupBox->setCursor(Qt::CursorShape::PointingHandCursor);
		oQColorBkg = QColor::fromRgba(p_GraphicsElementItem->oPSchElementBaseInt.uiObjectBkgColor);
		oQColorBkg.getRgb(&iR, &iG, &iB);
		if(((iR + iG + iB) / 3) > 128)
		{
			p_GraphicsElementItem->p_QGroupBox->
					setStyleSheet("QGroupBox { border:1px solid rgba(0, 0, 0, 255); border-radius: 3px; margin-top: 6px; } "
								  "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; "
								  "left: 4px; padding-top: 0px; }");
			p_GraphicsElementItem->oQPalette.setColor(QPalette::Foreground, QColor(Qt::black));
			p_GraphicsElementItem->bIsPositivePalette = false;
		}
		else
		{
			p_GraphicsElementItem->p_QGroupBox->
					setStyleSheet("QGroupBox { border:1px solid rgba(255, 255, 255, 255); border-radius: 3px; margin-top: 6px; } "
								  "QGroupBox::title { subcontrol-origin: margin; subcontrol-position: top left; "
								  "left: 4px; padding-top: 0px; }");
			p_GraphicsElementItem->oQPalette.setColor(QPalette::Foreground, QColor(Qt::white));
			p_GraphicsElementItem->bIsPositivePalette = true;
		}
		p_GraphicsElementItem->p_QGraphicsProxyWidget =
				MainWindow::p_SchematicWindow->GetSchematicView()->scene()->
				addWidget(p_GraphicsElementItem->p_QGroupBox); // Только так (на Linux).
		p_GraphicsElementItem->p_QGroupBox->move(3, 0);
		p_GraphicsElementItem->p_QGroupBox->
				setFixedSize(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW - 6,
							 p_GraphicsElementItem-> oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH - 3);
		p_GraphicsElementItem->p_QGraphicsProxyWidget->setFiltersChildEvents(true);
		p_GraphicsElementItem->p_QGraphicsProxyWidget->setParentItem(p_GraphicsElementItem);
		p_GraphicsElementItem->oQPalette.setBrush(QPalette::Background, p_GraphicsElementItem->oQBrush);
		p_GraphicsElementItem->p_QGroupBox->setPalette(p_GraphicsElementItem->oQPalette);
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
		{
			p_GraphicsElementItem->p_QGroupBox->hide(); // Если минимизировано - скрываем.
		}
	}
	else p_GraphicsElementItem->p_QGroupBox = nullptr; // Или его отсутствие.
	// Позиционирование.
	p_GraphicsElementItem->setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX,
								  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY);
	p_GraphicsElementItem->setZValue(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos);
	if(SchematicWindow::dbObjectZPos <= p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos)
	{
		SchematicWindow::dbObjectZPos =
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	p_GraphicsElementItem->oQBrush.setColor(
				QColor::fromRgba(p_GraphicsElementItem->oPSchElementBaseInt.uiObjectBkgColor));
	// Добавление скалера.
	p_GraphicsElementItem->p_GraphicsScalerItem = new GraphicsScalerItem(p_GraphicsElementItem);
	p_GraphicsElementItem->p_GraphicsScalerItem->setParentItem(p_GraphicsElementItem);
	if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		double dbR = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2.0f;
		//
		if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			double dbX = GetDiagPointOnCircle(dbR);
			p_GraphicsElementItem->p_GraphicsScalerItem->setPos(dbX, dbX);
		}
		else
		{
			p_GraphicsElementItem->
					p_GraphicsScalerItem->setPos((dbR + (pntTrR.x() * dbR)) - 5.1961522478610438f -
						(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / TRIANGLE_DECR_PROPORTION),
						(dbR + (pntTrR.y() * dbR)) - 3.0f);
		}
	}
	else
	{
		p_GraphicsElementItem->p_GraphicsScalerItem->
				setPos(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
					   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH);
	}
	if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
	{
		p_GraphicsElementItem->p_GraphicsScalerItem->hide();  // Если минимизировано - скрываем.
	}
	// Фрейм для обводки.
	p_GraphicsElementItem->p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_ELEMENT, p_GraphicsElementItem);
	p_GraphicsElementItem->p_GraphicsFrameItem->hide();
	// Статус блокирования.
	SetElementBlockingPattern(p_GraphicsElementItem,
							  p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
							  SCH_SETTINGS_EG_BIT_BUSY);
}

// Установка портов групп после смены статуса минимизации.
void SchematicView::SetPortsPlacementAfterGroupsMinChanges()
{
	for(int iF = 0; iF != pv_GraphicsPortItemsCollected.count(); iF++)
	{
		DbPoint oDbPointLastGroupPos;
		GraphicsPortItem* p_GraphicsPortItemCurrent = pv_GraphicsPortItemsCollected.at(iF);
		GraphicsElementItem* p_GraphicsElementItemCurrent = p_GraphicsPortItemCurrent->p_ParentInt;
		GraphicsGroupItem* p_GraphicsGroupItem = p_GraphicsElementItemCurrent->p_GraphicsGroupItemRel;
		DbPoint oDbPointCorr;
		//
		oDbPointLastGroupPos.dbX = OVERMAX_NUMBER;
		oDbPointLastGroupPos.dbY = OVERMAX_NUMBER;
		while(p_GraphicsGroupItem)
		{
			if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
			{
				oDbPointLastGroupPos.dbX = p_GraphicsGroupItem->pos().x();
				oDbPointLastGroupPos.dbY = p_GraphicsGroupItem->pos().y();
				p_GraphicsGroupItem->bPortsForMin = true;
			}
			p_GraphicsGroupItem = p_GraphicsGroupItem->p_GraphicsGroupItemRel;
		}
		if(oDbPointLastGroupPos.dbX == OVERMAX_NUMBER)
		{
			if(p_GraphicsPortItemCurrent->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
			   SCH_SETTINGS_EG_BIT_MIN)
			{
				oDbPointCorr.dbX = p_GraphicsPortItemCurrent->oDbPMinPos.dbX;
				oDbPointCorr.dbY = p_GraphicsPortItemCurrent->oDbPMinPos.dbY;
			}
			else
			{
				oDbPointCorr.dbX = p_GraphicsPortItemCurrent->oDbPAlterVisPos.dbX;
				oDbPointCorr.dbY = p_GraphicsPortItemCurrent->oDbPAlterVisPos.dbY;
			}
		}
		else
		{
			oDbPointCorr.dbX = oDbPointLastGroupPos.dbX - p_GraphicsElementItemCurrent->pos().x() + dbMinGroupR;
			oDbPointCorr.dbY = oDbPointLastGroupPos.dbY - p_GraphicsElementItemCurrent->pos().y() + dbMinGroupR;
		}
		SetPortToPos(p_GraphicsPortItemCurrent, oDbPointCorr);
	}
}

// Рекурсивные операции по минимизации группы.
void SchematicView::GroupMinOperationsRecursively(GraphicsGroupItem* p_GraphicsGroupItem, bool bNextHiding, bool bHiderFound)
{
	bool bGroupMinStatus = (p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN) != 0;
	//
	SetHidingStatus(p_GraphicsGroupItem, bNextHiding);
	if(bGroupMinStatus)
	{
		if(SchematicWindow::vp_SelectedGroups.contains(p_GraphicsGroupItem))
		{
			if(bHiderFound)
			{
				SchematicWindow::vp_SelectedGroups.removeAll(p_GraphicsGroupItem);
				p_GraphicsGroupItem->p_GraphicsFrameItem->hide();
				p_GraphicsGroupItem->bSelected = false;
			}
			else
			{
				bHiderFound = true;
			}
		}
	}
	bNextHiding |= bGroupMinStatus;
	// Элементы.
	for(int iE = 0; iE != p_GraphicsGroupItem->vp_ConnectedElements.count(); iE++)
	{
		GraphicsElementItem* p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iE);
		QList<QGraphicsItem*> lp_Items = p_GraphicsElementItem->childItems();
		// Меняем видимость элемента.
		SetHidingStatus(p_GraphicsElementItem, bNextHiding);
		if(bNextHiding)
		{
			SchematicWindow::vp_SelectedElements.removeAll(p_GraphicsElementItem);
			p_GraphicsElementItem->p_GraphicsFrameItem->hide();
			p_GraphicsElementItem->bSelected = false;
		}
		// Обработка линков.
		int iCn = lp_Items.count();
		for(int iC = 0; iC < iCn; iC++)
		{
			GraphicsPortItem* p_GraphicsPortItemInt;
			QGraphicsItem* p_GraphicsItem;
			//
			p_GraphicsItem = lp_Items.at(iC);
			if(p_GraphicsItem->data(SCH_TYPE_OF_ITEM) == SCH_TYPE_ITEM_UI)
			{
				if(p_GraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_PORT)
				{
					p_GraphicsPortItemInt = (GraphicsPortItem*)p_GraphicsItem;
					pv_GraphicsPortItemsCollected.append(p_GraphicsPortItemInt);
				}
			}
		}
	}
	for(int iE = 0; iE != p_GraphicsGroupItem->vp_ConnectedGroups.count(); iE++)
	{
		GraphicsGroupItem* p_GraphicsGroupItemInt = p_GraphicsGroupItem->vp_ConnectedGroups.at(iE);
		// Рекурсия.
		GroupMinOperationsRecursively(p_GraphicsGroupItemInt, bNextHiding, bHiderFound);
	}
}

// Обработчик события нажатия мыши на группу.
void SchematicView::GroupMousePressEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event)
{
	bool bLastSt;
	//
	SchematicWindow::ResetMenu();
	if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{ //Если группа блокирована занятостью, смещением выборки или главным окном - отказ.
		return;
	}
	if(DoubleButtonsPressControl(p_Event)) // Переключение минимизации.
	{
		unsigned char uchMinStatus = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits &
									 SCH_SETTINGS_EG_BIT_MIN; // Бит минимизации управляющей группы.
		uchMinStatus ^= SCH_SETTINGS_EG_BIT_MIN; // Действие над битом для управляющей группы (переключение).
		GraphicsGroupItem* p_GraphicsGroupItemCurrent;
		GraphicsGroupItem* p_GraphicsGroupItemRoot;
		QVector<GraphicsGroupItem*> vp_GraphicsGroupItemRoots;
		QVector<GraphicsGroupItem*> vp_GraphicsGroupItemForMin;
		//
		bLastSt = p_GraphicsGroupItem->bSelected; // Запоминаем текущее значение выбраности.
		if(!SchematicWindow::vp_SelectedGroups.contains(p_GraphicsGroupItem)) // Если не было выбрано - добавляем для массовых действий.
		{
			SchematicWindow::vp_SelectedGroups.prepend(p_GraphicsGroupItem);
		}
		// Обработка статусов минимизации.
		// Поиск, маркировка, коллекция и отправка изменяемых.
		for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
		{
			p_GraphicsGroupItemCurrent = SchematicWindow::vp_SelectedGroups.at(iF);
			if((p_GraphicsGroupItemCurrent->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN) !=
			   uchMinStatus) // Если бит минимизации текущей группы не равен переключённому биту для управляющей...
			{
				vp_GraphicsGroupItemForMin.append(p_GraphicsGroupItemCurrent); // В вектор для обработки.
				if(uchMinStatus & SCH_SETTINGS_EG_BIT_MIN) // Если новый статус - вкл. - установка бита.
				{
					SetBits(p_GraphicsGroupItemCurrent->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits,
							SCH_SETTINGS_EG_BIT_MIN);
					p_GraphicsGroupItemCurrent->p_QLabel->hide();
				}
				else // Если - выкл. - сброс.
				{
					ResetBits(p_GraphicsGroupItemCurrent->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits,
							  SCH_SETTINGS_EG_BIT_MIN);
					p_GraphicsGroupItemCurrent->bPortsForMin = false;
					p_GraphicsGroupItemCurrent->p_QLabel->show();
				}
				// Отправка инфо на сервер.
				p_GraphicsGroupItemCurrent->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_GROUP_BIT_MIN;
		//		MainWindow::p_Client->AddPocketToOutputBufferC(PROTO_O_SCH_GROUP_VARS, (char*)&oPSchGroupVars, sizeof(oPSchGroupVars));
				// Сбор корней.
				p_GraphicsGroupItemRoot = p_GraphicsGroupItemCurrent;
				while(p_GraphicsGroupItemCurrent) // Пока не корень.
				{
					p_GraphicsGroupItemRoot = p_GraphicsGroupItemCurrent;
					p_GraphicsGroupItemCurrent = p_GraphicsGroupItemCurrent->p_GraphicsGroupItemRel; // Глубже к корню.
				}
				if(!vp_GraphicsGroupItemRoots.contains(p_GraphicsGroupItemRoot))
				{
					vp_GraphicsGroupItemRoots.append(p_GraphicsGroupItemRoot); // Добавляется только уникальный корень.
				}
			}
		}
		pv_GraphicsPortItemsCollected.clear();
		// Заход в каждый корень.
		for(int iF = 0; iF != vp_GraphicsGroupItemRoots.count(); iF++)
		{
			p_GraphicsGroupItemRoot = vp_GraphicsGroupItemRoots.at(iF);
			GroupMinOperationsRecursively(p_GraphicsGroupItemRoot);
			UpdateVerticalOfGroupFramesRecursively(p_GraphicsGroupItemRoot);
		}
		//
		SetPortsPlacementAfterGroupsMinChanges();
		//
		if(!bLastSt) // Если был не выбран и добавлялся для массовых действий - удаление из списка выбранных.
		{
			SchematicWindow::vp_SelectedGroups.removeAll(p_GraphicsGroupItem);
		}
		goto gG;
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
			bool bGroupSelected = p_GraphicsGroupItem->bSelected;
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
			// Отсоединить.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuDetach))->setData(MENU_DETACH);
			// Создать элемент в группе.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuAddElement))->setData(MENU_ADD_ELEMENT);
			// Добавить выбранные свободные объекты.
			SchematicWindow::vp_SelectedFreeElements.clear();
			SchematicWindow::vp_SelectedFreeGroups.clear();
			TempSelectGroup(p_GraphicsGroupItem);
			if(!SchematicWindow::vp_SelectedElements.isEmpty())
			{
				for(int iF = 0; iF != SchematicWindow::vp_SelectedElements.count(); iF++)
				{
					GraphicsElementItem* p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iF);
					if(p_GraphicsElementItem->p_GraphicsGroupItemRel == nullptr)
					{
						SchematicWindow::vp_SelectedFreeElements.append(p_GraphicsElementItem);
					}
				}
			}
			for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
			{
				GraphicsGroupItem* p_GraphicsGroupItemHelper = SchematicWindow::vp_SelectedGroups.at(iF);
				if(p_GraphicsGroupItemHelper->p_GraphicsGroupItemRel == nullptr)
				{
					SchematicWindow::vp_SelectedFreeGroups.append(p_GraphicsGroupItemHelper);
				}
			}
			if(!SchematicWindow::vp_SelectedFreeElements.isEmpty() | (SchematicWindow::vp_SelectedFreeGroups.count() > 1))
			{ // Если хоть что-то выбрано, кроме текущей группы...
				if(!bGroupSelected) // И если текущая не выбрана...
				{
					SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuAddFreeSelected))->setData(MENU_ADD_SELECTED);
				}
				else goto gC; // Иначе - на создание группы.
			}
			else // Иначе - создание группы.
			{
gC:				SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuCreateGroup))->setData(MENU_CREATE_GROUP);
			}
			TempDeselectGroup(p_GraphicsGroupItem);
			// Цвет фона.
			SchematicWindow::p_SafeMenu->addAction(QString(m_chMenuBackground))->setData(MENU_CHANGE_BACKGROUND);
			bGroupMenuReady = true;
		}
	}
gG:	TrySendBufferToServer;
	p_GraphicsGroupItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с группой.
void SchematicView::GroupMouseMoveEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event)
{
	QPointF oQPointFInit;
	QPointF oQPointFRes;
	//
	if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier || bRMBPressed)
	{
		return;
	}
	oQPointFInit = p_GraphicsGroupItem->pos();
	p_GraphicsGroupItem->OBMouseMoveEvent(p_Event);
	oQPointFRes = p_GraphicsGroupItem->pos();
	oQPointFRes.setX(oQPointFRes.x() - oQPointFInit.x()); // Смещение по X.
	oQPointFRes.setY(oQPointFRes.y() - oQPointFInit.y()); // Смещение по Y.
	v_AlreadyMovedGroups.clear();
	MoveGroupRecursively(p_GraphicsGroupItem, oQPointFRes, false);
	if(p_GraphicsGroupItem->bSelected)
	{
		for(int iE = 0; iE != SchematicWindow::vp_SelectedGroups.count(); iE ++)
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
}

// Обработчик события отпусканеия мыши на группе.
void SchematicView::GroupMouseReleaseEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	DoubleButtonsReleaseControl();
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		ReleaseOccupiedAPFS();
		TrySendBufferToServer;
	}
	p_GraphicsGroupItem->OBMouseReleaseEvent(p_Event);
	if((SchematicWindow::p_SafeMenu != nullptr) && bGroupMenuReady)
	{
		QAction* p_SelectedMenuItem;
		Set_Proposed_String_Dialog* p_Set_Proposed_String_Dialog;
		PSchGroupName oPSchGroupName;
		char m_chName[SCH_OBJ_NAME_STR_LEN];
		//
		bGroupMenuReady = false;
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
				AddFreeSelectedGroupsToGroupAPFS(p_GraphicsGroupItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_CREATE_GROUP)
			{
				TempSelectGroup(p_GraphicsGroupItem);
				CreateGroupFromSelected();
				TempDeselectGroup(p_GraphicsGroupItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_DISBAND)
			{
				GraphicsGroupItem* p_GraphicsGroupItemUtil;
				//
				TempSelectGroup(p_GraphicsGroupItem);
				for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
				{
					p_GraphicsGroupItemUtil = SchematicWindow::vp_SelectedGroups.at(iF);
					for(int iF = 0; iF != p_GraphicsGroupItemUtil->vp_ConnectedElements.count(); iF++)
					{
						GraphicsElementItem* p_GraphicsElementItem = p_GraphicsGroupItemUtil->vp_ConnectedElements.at(iF);
						PSchElementVars oPSchElementVars;
						//
						p_GraphicsElementItem->setZValue(SchematicWindow::dbObjectZPos);
						p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos =
								SchematicWindow::dbObjectZPos;
						SchematicWindow::dbObjectZPos += SCH_NEXT_Z_SHIFT;
						p_GraphicsElementItem->update();
						oPSchElementVars.ullIDInt = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt;
						oPSchElementVars.ullIDGroup = 0;
						oPSchElementVars.oSchEGGraph.dbObjectZPos =
								p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.dbObjectZPos;
						oPSchElementVars.oSchEGGraph.uchChangesBits = SCH_CHANGES_ELEMENT_BIT_GROUP | SCH_CHANGES_ELEMENT_BIT_ZPOS;
						p_GraphicsElementItem->p_GraphicsGroupItemRel = nullptr;
						p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup = 0;
						MainWindow::p_Client->AddPocketToOutputBufferC(
									PROTO_O_SCH_ELEMENT_VARS, (char*)&oPSchElementVars, sizeof(PSchElementVars));
						UpdateLinksZPos();
					}
					SchematicWindow::vp_Groups.removeOne(p_GraphicsGroupItemUtil);
					MainWindow::p_SchematicWindow->oScene.removeItem(p_GraphicsGroupItemUtil);
				}
				TempDeselectGroup(p_GraphicsGroupItem);
			}
			else if(p_SelectedMenuItem->data() == MENU_ADD_ELEMENT)
			{
				GraphicsElementItem* p_GraphicsElementItem;
				//
				p_GraphicsElementItem = CreateNewElementAPFS((char*)m_chNewElement, p_GraphicsGroupItem->mapToScene(p_Event->pos()),
															 p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt);
				p_GraphicsGroupItem->vp_ConnectedElements.push_front(p_GraphicsElementItem);
				p_GraphicsElementItem->p_GraphicsGroupItemRel = p_GraphicsGroupItem;
				p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDGroup =
						p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.ullIDInt;
				UpdateGroupFrameByContentRecursively(p_GraphicsGroupItem);
				BlockingVerticalsAndPopupElement(p_GraphicsElementItem, p_GraphicsGroupItem,
												 SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
												 ADD_SEND_ZPOS, ADD_SEND_FRAME, SEND_ELEMENTS, DONT_AFFECT_SELECTED);
				UpdateLinksZPos();
			}
			else if(p_SelectedMenuItem->data() == MENU_CHANGE_BACKGROUND)
			{

			}
		}
		TrySendBufferToServer;
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
	if(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
	{
		p_Painter->drawRect(QRectF(0, 0, dbMinGroupD, dbMinGroupD));
		p_Painter->drawLine(QPointF(dbMinGroupR, 0), QPointF(dbMinGroupR, dbMinGroupD));
		p_Painter->drawLine(QPointF(0, dbMinGroupR), QPointF(dbMinGroupD, dbMinGroupR));
		if(p_GraphicsGroupItem->bPortsForMin)
		{
			p_Painter->setBrush(SchematicWindow::oQBrushGray);
			p_Painter->setPen(SchematicWindow::oQPenWhite);
			p_Painter->drawEllipse(QPointF(dbMinGroupR, dbMinGroupR), PORT_DIM, PORT_DIM);
		}
	}
	else
	{
		p_Painter->drawRect(QRectF(0, 0, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW,
							p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH));
		p_Painter->drawLine(QPointF(1, 18), QPointF(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW - 1, 18));
	}
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
	p_GraphicsGroupItem->bPortsForMin = false;
	//
	oQColorBkg = QColor::fromRgba(p_GraphicsGroupItem->oPSchGroupBaseInt.uiObjectBkgColor);
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
	p_GraphicsGroupItem->setPos(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX,
								p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY);
	p_GraphicsGroupItem->setZValue(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos);
	if(SchematicWindow::dbObjectZPos <= p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos)
	{
		SchematicWindow::dbObjectZPos = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.dbObjectZPos + SCH_NEXT_Z_SHIFT;
	}
	p_GraphicsGroupItem->
			oQBrush.setColor(QColor::fromRgba(p_GraphicsGroupItem->oPSchGroupBaseInt.uiObjectBkgColor));
	//
	p_GraphicsGroupItem->p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_GROUP, nullptr, p_GraphicsGroupItem);
	p_GraphicsGroupItem->p_GraphicsFrameItem->hide();
	//
	p_GraphicsGroupItem->p_QLabel = new QLabel(QString(p_GraphicsGroupItem->oPSchGroupBaseInt.m_chName));
	p_GraphicsGroupItem->p_QLabel->setAttribute(Qt::WA_TranslucentBackground);
	p_GraphicsGroupItem->p_QLabel->setCursor(Qt::CursorShape::PointingHandCursor);
	p_GraphicsGroupItem->p_QLabel->move(3, 0);
	p_GraphicsGroupItem->p_QLabel->setFixedWidth(p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW - 6);
	p_GraphicsGroupItem->p_QGraphicsProxyWidget = MainWindow::p_SchematicWindow->oScene.addWidget(p_GraphicsGroupItem->p_QLabel);
	p_GraphicsGroupItem->p_QGraphicsProxyWidget->setFiltersChildEvents(true);
	p_GraphicsGroupItem->p_QGraphicsProxyWidget->setParentItem(p_GraphicsGroupItem);
	p_GraphicsGroupItem->p_QLabel->setPalette(p_GraphicsGroupItem->oQPalette);
	//
	SetGroupBlockingPattern(p_GraphicsGroupItem, p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits &
							SCH_SETTINGS_EG_BIT_BUSY);
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

// Обработчик функции возврата вместилища рамки.
QRectF SchematicView::FrameBoundingHandler(const GraphicsFrameItem* pc_GraphicsFrameItem)
{
	QRectF oQRectFResult;
	//
	if(pc_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_ELEMENT)
	{
		oQRectFResult = ElementBoundingHandler(pc_GraphicsFrameItem->p_ElementParentInt);
	}
	else if(pc_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_GROUP)
	{
		oQRectFResult = GroupBoundingHandler(pc_GraphicsFrameItem->p_GroupParentInt);
	}
	else
	{
		oQRectFResult = PortBoundingHandler();
	}
	oQRectFResult.setX(oQRectFResult.x() - 6);
	oQRectFResult.setY(oQRectFResult.y() - 6);
	oQRectFResult.setWidth(oQRectFResult.width() + 12);
	oQRectFResult.setHeight(oQRectFResult.height() + 12);
	return oQRectFResult;
}

// Обработчик функции возврата вместилища элемента и его видов.
QRectF SchematicView::ElementBoundingHandler(const GraphicsElementItem* pc_GraphicsElementItem)
{
	if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
			{
				return QRectF(0, 0, dbMinCircleD, dbMinCircleD);
			}
			else
			{
				return QRectF(0, 0,
							  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
							  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW);
			}
		}
		else
		{
			if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
			{
				return QRectF(pntMinTrL.x(), pntMinTrT.y(), pntMinTrR.x(), pntMinTrR.y());
			}
			else
			{
				return QRectF(0, 0,
							  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW -
							  (pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 7.5925f),
							  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW * 0.75245604f);
			}
		}
	}
	if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
	{
		return QRectF(0, 0, dbMinElementD, dbMinElementD);
	}
	else
	{
		return QRectF(0, 0,
					  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
					  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH);
	}
}

// Обработчик функции возврата формы элемента и его видов.
QPainterPath SchematicView::ElementShapeHandler(const GraphicsElementItem* pc_GraphicsElementItem)
{
	QPainterPath oQPainterPath;
	//
	if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
			{
				oQPainterPath.addEllipse(0.0f, 0.0f, dbMinCircleD, dbMinCircleD);
			}
			else
			{
				oQPainterPath.addEllipse(0.0f, 0.0f,
									  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
									  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW);
			}
		}
		else
		{
			QPolygonF oQPolygon;
			double dbHalfW = pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2.0f;
			double dbDecr = pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / TRIANGLE_DECR_PROPORTION;
			//
			if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
			{
				oQPolygon.append(pntMinTrR);
				oQPolygon.append(pntMinTrT);
				oQPolygon.append(pntMinTrL);
				oQPainterPath.addPolygon(oQPolygon);
			}
			else
			{
				oQPolygon.append(QPointF(dbHalfW + (SchematicView::pntTrR.x() * dbHalfW) - dbDecr,
										 dbHalfW + (SchematicView::pntTrR.y() * dbHalfW)));
				oQPolygon.append(QPointF(dbHalfW + (SchematicView::pntTrT.x() * dbHalfW) - dbDecr,
										 dbHalfW + (SchematicView::pntTrT.y() * dbHalfW)));
				oQPolygon.append(QPointF(dbHalfW + (SchematicView::pntTrL.x() * dbHalfW) - dbDecr,
										 dbHalfW + (SchematicView::pntTrL.y() * dbHalfW)));
				oQPainterPath.addPolygon(oQPolygon);
			}
		}
	}
	else
	{
		if(pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
		{
			oQPainterPath.addRect(0, 0, dbMinElementD, dbMinElementD);
		}
		else
		{
			oQPainterPath.addRect(0, 0,
								  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW,
								  pc_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH);
		}
	}
	return oQPainterPath;
}

// Обработчик функции возврата вместилища группы.
QRectF SchematicView::GroupBoundingHandler(const GraphicsGroupItem* pc_GraphicsGroupItem)
{
	if(pc_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
	{
		return QRectF(0, 0, dbMinGroupD, dbMinGroupD);
	}
	else
	{
		return QRectF(0, 0, pc_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW,
					  pc_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH);
	}
}

// Обработчик функции возврата вместилища порта.
QRectF SchematicView::PortBoundingHandler()
{
	return QRectF(dbPortDimNeg, dbPortDimNeg, dbPortDimD, dbPortDimD);
}

// Обработчик функции рисования фрейма.
void SchematicView::FramePaintHandler(GraphicsFrameItem* p_GraphicsFrameItem, QPainter* p_Painter)
{
	p_Painter->setBrush(Qt::NoBrush);
	if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_ELEMENT)
	{
		p_Painter->setPen(SchematicWindow::oQPenElementFrameFlash);
		if(p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
		   SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
		{
			if(p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
			   SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
			{
				double dbR = p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2;
				double dbROutside = dbR + dbFrameDimIncSubCorr;
				//
				if(p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
				   SCH_SETTINGS_EG_BIT_MIN)
				{
					p_Painter->drawEllipse(QPointF(dbMinCircleR, dbMinCircleR),
										   dbMinCircleRPlusFrameDimIncSubCorr, dbMinCircleRPlusFrameDimIncSubCorr);
				}
				else
				{
					p_Painter->drawEllipse(QPointF(dbR, dbR), dbROutside, dbROutside);
				}
			}
			else
			{
				QPolygonF oQPolygonFForTriangle;
				double dbHalfW = p_GraphicsFrameItem->
								 p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2.0f;
				double dbDecr = p_GraphicsFrameItem->
								p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / TRIANGLE_DECR_PROPORTION;
				double dbHalfWOutside = dbHalfW + dbFrameDimIncTwiceSubCorr;
				//
				if(p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
				   SCH_SETTINGS_EG_BIT_MIN)
				{
					oQPolygonFForTriangle.append(pntMinFrameTrR);
					oQPolygonFForTriangle.append(pntMinFrameTrT);
					oQPolygonFForTriangle.append(pntMinFrameTrL);
					p_Painter->drawConvexPolygon(oQPolygonFForTriangle);
				}
				else
				{
					oQPolygonFForTriangle.append(QPointF(dbHalfW + (pntTrR.x() * dbHalfWOutside) - dbDecr,
													 dbHalfW + (pntTrR.y() * dbHalfWOutside)));
					oQPolygonFForTriangle.append(QPointF(dbHalfW + (pntTrT.x() * dbHalfWOutside) - dbDecr,
													 dbHalfW + (pntTrT.y() * dbHalfWOutside)));
					oQPolygonFForTriangle.append(QPointF(dbHalfW + (pntTrL.x() * dbHalfWOutside) - dbDecr,
													 dbHalfW + (pntTrL.y() * dbHalfWOutside)));
					p_Painter->drawConvexPolygon(oQPolygonFForTriangle);
				}
			}
		}
		else
		{
			if(p_GraphicsFrameItem->p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
			   SCH_SETTINGS_EG_BIT_MIN)
			{
				p_Painter->drawRect(QRectF(dbFrameDimIncNegPlusHalfCorr, dbFrameDimIncNegPlusHalfCorr,
										   dbMinElementDPlusFrameDimIncTwiceSubDoubleCorr, dbMinElementDPlusFrameDimIncTwiceSubDoubleCorr));
			}
			else
			{
				p_Painter->drawRect(QRectF(dbFrameDimIncNegPlusHalfCorr, dbFrameDimIncNegPlusHalfCorr,
									p_GraphicsFrameItem->
									p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW +
										   dbFrameDimIncTwiceSubDoubleCorr,
									p_GraphicsFrameItem->
									p_ElementParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH +
										   dbFrameDimIncTwiceSubDoubleCorr));
			}
		}
	}
	else if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_GROUP)
	{
		p_Painter->setPen(SchematicWindow::oQPenGroupFrameFlash);
		if(p_GraphicsFrameItem->p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
		{
			p_Painter->drawRect(QRectF(dbFrameDimIncNegPlusCorr, dbFrameDimIncNegPlusCorr,
								dbMinGroupD + dbFrameDimIncTwiceSubDoubleCorr,
								dbMinGroupD + dbFrameDimIncTwiceSubDoubleCorr));
		}
		else
		{
			p_Painter->drawRect(QRectF(dbFrameDimIncNegPlusCorr, dbFrameDimIncNegPlusCorr,
								p_GraphicsFrameItem->p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW +
									   dbFrameDimIncTwiceSubDoubleCorr,
								p_GraphicsFrameItem->p_GroupParentInt->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH +
									   dbFrameDimIncTwiceSubDoubleCorr));
		}
	}
	else if(p_GraphicsFrameItem->ushKindOfItemInt == SCH_KIND_ITEM_PORT)
	{
		p_Painter->setPen(SchematicWindow::oQPenPortFrameFlash);
		p_Painter->drawEllipse(QPointF(0, 0), PORT_DIM, PORT_DIM);
	}
}

// Обработчик конструктора фрейма.
void SchematicView::FrameConstructorHandler(GraphicsFrameItem* p_GraphicsFrameItem, unsigned short ushKindOfItem,
											GraphicsElementItem* p_ElementParent, GraphicsGroupItem* p_GroupParent,
											GraphicsPortItem* p_PortParent)
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
		if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDSrc) &&
		   (p_GraphicsLinkItem->p_GraphicsElementItemSrc == nullptr))
		{
			p_GraphicsLinkItem->p_GraphicsElementItemSrc = p_GraphicsElementItem;
		}
		if((p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst) &&
		   (p_GraphicsLinkItem->p_GraphicsElementItemDst == nullptr))
		{
			p_GraphicsLinkItem->p_GraphicsElementItemDst = p_GraphicsElementItem;
		}
		if((p_GraphicsLinkItem->p_GraphicsElementItemSrc != nullptr) && (p_GraphicsLinkItem->p_GraphicsElementItemDst != nullptr))
		{
			GraphicsPortItem* p_GraphicsPortItemSrc;
			GraphicsPortItem* p_GraphicsPortItemDst;
			//
			p_GraphicsPortItemSrc = new GraphicsPortItem(p_GraphicsLinkItem, true, p_GraphicsLinkItem->p_GraphicsElementItemSrc);
			p_GraphicsPortItemDst = new GraphicsPortItem(p_GraphicsLinkItem, false, p_GraphicsLinkItem->p_GraphicsElementItemDst);
			p_GraphicsLinkItem->p_PSchElementVarsSrc = &p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.oPSchElementVars;
			p_GraphicsLinkItem->p_PSchElementVarsDst = &p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.oPSchElementVars;
			SchematicWindow::vp_Ports.append(p_GraphicsPortItemSrc);
			SchematicWindow::vp_Ports.append(p_GraphicsPortItemDst);
			// После создания портов по статусу свёрнутости - коррекция в переменных линка.
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos.dbX = p_GraphicsPortItemSrc->pos().x();
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos.dbY = p_GraphicsPortItemSrc->pos().y();
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos.dbX = p_GraphicsPortItemDst->pos().x();
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos.dbY = p_GraphicsPortItemDst->pos().y();
			//
			UpdateLinkPositionByElements(p_GraphicsLinkItem);
			p_GraphicsLinkItem->setAcceptedMouseButtons(0);
			return;
		}
	}
	p_PSchLinkBase->oPSchLinkVars.oSchLGraph.uchChangesBits = SCH_CHANGES_LINK_BIT_INIT_ERROR; // Отметка про сбой в поиске элементов.
}

// Вычисление точек портов.
SchematicView::DbPointPair SchematicView::CalcPortsCoords(GraphicsLinkItem* p_GraphicsLinkItem)
{
	DbPointPair oDbPointPairResult;
	//
	oDbPointPairResult.dbSrc.dbX = p_GraphicsLinkItem->p_PSchElementVarsSrc->oSchEGGraph.oDbFrame.dbX +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos.dbX;
	oDbPointPairResult.dbSrc.dbY = p_GraphicsLinkItem->p_PSchElementVarsSrc->oSchEGGraph.oDbFrame.dbY +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbSrcPortGraphPos.dbY;
	oDbPointPairResult.dbDst.dbX = p_GraphicsLinkItem->p_PSchElementVarsDst->oSchEGGraph.oDbFrame.dbX +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos.dbX;
	oDbPointPairResult.dbDst.dbY = p_GraphicsLinkItem->p_PSchElementVarsDst->oSchEGGraph.oDbFrame.dbY +
			p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.oDbDstPortGraphPos.dbY;
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
	SchematicWindow::ResetMenu();
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(DoubleButtonsPressControl(p_Event))
	{
		return;
	}
	bPortFromElement = p_GraphicsLinkItemNew != nullptr;
	if((p_GraphicsPortItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY) & (!bPortFromElement)) return;
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		oDbPointPortInitialClick.dbX = p_GraphicsPortItem->pos().x(); // Исходный X.
		oDbPointPortInitialClick.dbY = p_GraphicsPortItem->pos().y(); // Исходный Y.
		oDbPointPortRB.dbX = p_GraphicsPortItem->p_SchEGGraph->oDbFrame.dbW; // Крайняя правая точка.
		oDbPointPortRB.dbY = p_GraphicsPortItem->p_SchEGGraph->oDbFrame.dbH; // Крайняя нижняя точка.
		BlockingVerticalsAndPopupElement(p_GraphicsPortItem->p_ParentInt, p_GraphicsPortItem->p_ParentInt->p_GraphicsGroupItemRel,
						  SEND_GROUP, DONT_SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
						  ADD_SEND_BUSY, DONT_ADD_SEND_FRAME, SEND_ELEMENTS, DONT_AFFECT_SELECTED);
		TrySendBufferToServer;
		if(p_Event->modifiers() == Qt::AltModifier)
		{
			bPortAltPressed = true;
		}
		else
		{
			bPortAltPressed = false;
			oDbPointPortCurrent.dbX = p_GraphicsPortItem->pos().x(); // Текущий X.
			oDbPointPortCurrent.dbY = p_GraphicsPortItem->pos().y(); // Текущий Y.
		}
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
			bPortMenuReady = true;
		}
	}
	p_GraphicsPortItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с портом.
void SchematicView::PortMouseMoveEventHandler(GraphicsPortItem* p_GraphicsPortItem, QGraphicsSceneMouseEvent* p_Event)
{
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier || bRMBPressed)
	{
		return;
	}
	if((p_GraphicsPortItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY) & (!bPortFromElement)) return;
	//
	p_GraphicsPortItem->OBMouseMoveEvent(p_Event); // Даём мышке уйти.
	if(bLMBPressed)
	{
		oDbPointPortCurrent.dbX = p_GraphicsPortItem->pos().x(); // Текущий X.
		oDbPointPortCurrent.dbY = p_GraphicsPortItem->pos().y(); // Текущий Y.
		if(!bPortAltPressed)
		{
			oDbPointPortCurrent = BindToEdge(p_GraphicsPortItem->p_ParentInt, oDbPointPortCurrent);
		}
		SetPortToPos(p_GraphicsPortItem, oDbPointPortCurrent);
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
	DbPoint oDbMappedToElement;
	char m_chPortNumber[PORT_NUMBER_STR_LEN];
	bool bLMBPressedOverride;
	//
	if(MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(bPortFromElement) bLMBPressed = true;
	bLMBPressedOverride = bLMBPressed;
	if(DoubleButtonsReleaseControl()) return;
	if((p_GraphicsPortItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY) & (!bPortFromElement)) return;
	if(bLMBPressedOverride)
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
						if(p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
						   SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
						{
							QPointF oQPointFAtElement;
							//
							oQPointFAtElement.setX(oDbMapped.dbX -
												   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX);
							oQPointFAtElement.setY(oDbMapped.dbY -
												   p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY);
							if(p_GraphicsElementItem->shape().contains(oQPointFAtElement))
							{
								vp_QGraphicsItemsOnPosition.append(p_QGraphicsItem);
							};
						}
						else
						{
							dbX = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX;
							dbY = p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY;
							dbXR = dbX + p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW;
							dbYD = dbY + p_GraphicsElementItem->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbH;
							if(((oDbMapped.dbX > dbX) & (oDbMapped.dbX < dbXR)) & ((oDbMapped.dbY > dbY) & (oDbMapped.dbY < dbYD)))
							{
								vp_QGraphicsItemsOnPosition.append(p_QGraphicsItem);
							}
						}
					}
					else if(p_QGraphicsItem->data(SCH_KIND_OF_ITEM) == SCH_KIND_ITEM_GROUP)
					{
						p_GraphicsGroupItem = (GraphicsGroupItem*) p_QGraphicsItem;
						dbX = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbX;
						dbY = p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbY;
						dbXR = dbX + p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbW;
						dbYD = dbY + p_GraphicsGroupItem->oPSchGroupBaseInt.oPSchGroupVars.oSchEGGraph.oDbFrame.dbH;
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
				if(p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
				{ // Если была попытка добавить к свёрнутому...
					if(bPortFromElement)
					{ // Если идёт с нового...
						goto gEld;
					}
					else
					{
						goto gED;
					}
				}
				if(p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.ullIDInt ==
				   p_GraphicsPortItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
				{ // Если на исходный элемент...
					oDbMappedToElement.dbX = oDbMapped.dbX -
											 p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX;
					oDbMappedToElement.dbY = oDbMapped.dbY -
											 p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY;
					oDbPointPortCurrent = BindToEdge(p_GraphicsElementItemFounded, oDbMappedToElement); // Прикрепление.
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
					oDbPointPortCurrent = BindToEdge(p_GraphicsElementItemFounded, oDbMappedToElement); // Прикрепление.
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
					SetPortToPos(p_GraphicsPortItem, oDbPointPortCurrent); // Установка позиции граф. порта.
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
		oDbPointPortCurrent = BindToEdge(p_GraphicsPortItem->p_ParentInt, oDbPointPortCurrent);
gEr:	SetPortToPos(p_GraphicsPortItem, oDbPointPortCurrent);
		if(p_GraphicsPortItem->bIsSrc)
		{
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbX = p_GraphicsPortItem->x();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbY =p_GraphicsPortItem->y();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.uchChangesBits = SCH_CHANGES_LINK_BIT_SCR_PORT_POS;
		}
		else
		{
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbX = p_GraphicsPortItem->x();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbY = p_GraphicsPortItem->y();
			p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.uchChangesBits = SCH_CHANGES_LINK_BIT_DST_PORT_POS;
		}
		if(!bPortFromElement)
		{
			MainWindow::p_Client->AddPocketToOutputBufferC(
						PROTO_O_SCH_LINK_VARS, (char*)p_GraphicsPortItem->p_PSchLinkVarsInt, sizeof(PSchLinkVars));
		}
gF:		ReleaseOccupiedAPFS();
	}
	p_GraphicsPortItem->OBMouseReleaseEvent(p_Event);
	if(bLMBPressedOverride)
	{
		if(p_GraphicsElementItemFounded)
		{
			if(!(p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN))
			{
				oDbMappedToElement.dbX = oDbMapped.dbX -
										 p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX;
				oDbMappedToElement.dbY = oDbMapped.dbY -
										 p_GraphicsElementItemFounded->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY;
				ReplaceLink(p_GraphicsPortItem->p_GraphicsLinkItemInt, p_GraphicsElementItemFounded,
							p_GraphicsPortItem->bIsSrc, oDbMappedToElement, bPortFromElement);
			}
		}
		p_GraphicsPortItemActive = nullptr;
	}
	else if((SchematicWindow::p_SafeMenu != nullptr) && bPortMenuReady)
	{
		QAction* p_SelectedMenuItem;
		//
		bPortMenuReady = false;
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
				oPSchLinkBase.oPSchLinkVars.oSchLGraph = p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph;
				oPSchLinkBase.oPSchLinkVars.ullIDSrc = p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDSrc;
				oPSchLinkBase.oPSchLinkVars.ullIDDst = p_GraphicsPortItem->p_PSchLinkVarsInt->ullIDDst;
				oPSchLinkBase.oPSchLinkVars.ushiSrcPort = p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort;
				oPSchLinkBase.oPSchLinkVars.ushiDstPort = p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort;
				if(p_SelectedMenuItem->data() == MENU_SRC_PORT)
				{
gSrc:				CopyStrArray((char*)QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort).toStdString().c_str(),
								 m_chPortNumber, PORT_NUMBER_STR_LEN);
					p_Set_Proposed_String_Dialog = new Set_Proposed_String_Dialog((char*)"Номер или псевдоним порта источника",
																				  m_chPortNumber, PORT_NUMBER_STR_LEN);
					if(p_Set_Proposed_String_Dialog->exec() == DIALOGS_ACCEPT)
					{
						DeleteLinkAPFS(p_GraphicsPortItem->p_GraphicsLinkItemInt, NOT_FROM_ELEMENT, DONT_REMOVE_FROM_CLIENT);
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
						DeleteLinkAPFS(p_GraphicsPortItem->p_GraphicsLinkItemInt, NOT_FROM_ELEMENT, DONT_REMOVE_FROM_CLIENT);
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
	p_Painter->drawEllipse(QPointF(0, 0), PORT_DIM, PORT_DIM);
	SchematicWindow::RestoreBrushesStyles();
	// Потом можно перевести в места, где актуально меняется.
	p_GraphicsPortItem->oDbPAlterVisPos.dbX = p_GraphicsPortItem->pos().x();
	p_GraphicsPortItem->oDbPAlterVisPos.dbY = p_GraphicsPortItem->pos().y();
}

// Обработчик конструктора порта.
void SchematicView::PortConstructorHandler(GraphicsPortItem* p_GraphicsPortItem, GraphicsLinkItem* p_GraphicsLinkItem,
										   bool bSrc, GraphicsElementItem* p_Parent)
{
	double dbX, dbY;
	//
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
	p_Parent->bPortsForMin = true;
	p_GraphicsPortItem->p_SchEGGraph = &p_GraphicsPortItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph;
	if(p_GraphicsPortItem->bIsSrc)
	{
		dbX = p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbX;
		dbY = p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbSrcPortGraphPos.dbY;
		p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemSrc = p_GraphicsPortItem;
		p_GraphicsPortItem->setToolTip(m_chPortTooltip + QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiSrcPort));
	}
	else
	{
		dbX = p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbX;
		dbY = p_GraphicsPortItem->p_PSchLinkVarsInt->oSchLGraph.oDbDstPortGraphPos.dbY;
		p_GraphicsPortItem->p_GraphicsLinkItemInt->p_GraphicsPortItemDst = p_GraphicsPortItem;
		p_GraphicsPortItem->setToolTip(m_chPortTooltip + QString::number(p_GraphicsPortItem->p_PSchLinkVarsInt->ushiDstPort));
	}
	p_GraphicsPortItem->p_GraphicsFrameItem = new GraphicsFrameItem(SCH_KIND_ITEM_PORT, nullptr, nullptr, p_GraphicsPortItem);
	p_GraphicsPortItem->p_GraphicsFrameItem->hide();
	// Установка минимизированной позиции порта в зависимости от типа элемента.
	if(p_GraphicsPortItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(p_GraphicsPortItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			p_GraphicsPortItem->oDbPMinPos.dbX = dbMinCircleR;
			p_GraphicsPortItem->oDbPMinPos.dbY = dbMinCircleR;
		}
		else
		{
			p_GraphicsPortItem->oDbPMinPos.dbX = dbMinTriangleRSubMinTriangleDerc;
			p_GraphicsPortItem->oDbPMinPos.dbY = dbMinTriangleR;
		}
	}
	else
	{
		p_GraphicsPortItem->oDbPMinPos.dbX = dbMinElementR;
		p_GraphicsPortItem->oDbPMinPos.dbY = dbMinElementR;
	}
	// Установка текущего и запасного положения порта в зависимости от статуса свёрнутости.
	if(p_GraphicsPortItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_EG_BIT_MIN)
	{
		p_GraphicsPortItem->oDbPAlterMinPos.dbX = dbX;
		p_GraphicsPortItem->oDbPAlterMinPos.dbY = dbY;
		p_GraphicsPortItem->setPos(p_GraphicsPortItem->oDbPMinPos.dbX, p_GraphicsPortItem->oDbPMinPos.dbY);
		p_GraphicsPortItem->hide();
	}
	else
	{
		p_GraphicsPortItem->oDbPAlterMinPos = p_GraphicsPortItem->oDbPMinPos;
		p_GraphicsPortItem->setPos(dbX, dbY);
	}
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
	if((p_GraphicsScalerItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY) ||
	   MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	if(p_Event->button() == Qt::MouseButton::LeftButton)
	{
		BlockingVerticalsAndPopupElement(p_GraphicsScalerItem->p_ParentInt,
										 p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel, SEND_GROUP,
						  DONT_SEND_NEW_ELEMENTS_TO_GROUP, DONT_SEND_NEW_GROUPS_TO_GROUP,
						  ADD_SEND_BUSY, DONT_ADD_SEND_FRAME, SEND_ELEMENTS, DONT_AFFECT_SELECTED);
		TrySendBufferToServer;
	}
	p_GraphicsScalerItem->OBMousePressEvent(p_Event);
}

// Обработчик события перемещения мыши с скалером.
void SchematicView::ScalerMouseMoveEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event)
{
	DbPoint oDbPointPos;
	//
	if((p_GraphicsScalerItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY)
	   || MainWindow::bBlockingGraphics || p_Event->modifiers() == Qt::ShiftModifier)
	{
		return;
	}
	oDbPointPos.dbX = p_GraphicsScalerItem->pos().x();
	oDbPointPos.dbY = p_GraphicsScalerItem->pos().y();
	p_GraphicsScalerItem->OBMouseMoveEvent(p_Event);; // Даём мышке уйти.
	if(p_Event->scenePos().x() <
	   p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbX + ELEMENT_MIN_X)
	{
		p_GraphicsScalerItem->setX(oDbPointPos.dbX);
	}
	if(p_Event->scenePos().y() <
	   p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbY + ELEMENT_MIN_Y)
	{
		p_GraphicsScalerItem->setY(oDbPointPos.dbY);
	}
	oDbPointPos.dbX = p_GraphicsScalerItem->pos().x() - oDbPointPos.dbX;
	oDbPointPos.dbY = p_GraphicsScalerItem->pos().y() - oDbPointPos.dbY;
	if(p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(oDbPointPos.dbX < oDbPointPos.dbY)
			oDbPointPos.dbY = oDbPointPos.dbX;
		else if (oDbPointPos.dbY < oDbPointPos.dbX)
			oDbPointPos.dbX = oDbPointPos.dbY;
	}
	p_GraphicsScalerItem->p_ParentInt->oDbPointDimIncrements = oDbPointPos;
	UpdateSelectedInElement(p_GraphicsScalerItem->p_ParentInt, SCH_UPDATE_ELEMENT_FRAME | SCH_UPDATE_LINKS_POS | SCH_UPDATE_MAIN);
	if(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel != nullptr) // По группе элемента без выборки, если она есть.
	{
		UpdateGroupFrameByContentRecursively(p_GraphicsScalerItem->p_ParentInt->p_GraphicsGroupItemRel);
	}
	if(p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
		   SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			double dbPR =
					GetDiagPointOnCircle(p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2);
			p_GraphicsScalerItem->setX(dbPR);
			p_GraphicsScalerItem->setY(dbPR);
		}
		else
		{
			double dbR = p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2.0f;
			//
			p_GraphicsScalerItem->setPos((dbR + (pntTrR.x() * dbR)) - 5.1961522478610438f -
										 (p_GraphicsScalerItem->p_ParentInt->
										  oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / TRIANGLE_DECR_PROPORTION),
										 (dbR + (pntTrR.y() * dbR)) - 3.0f);
		}
	}
}

// Обработчик события отпусканеия мыши на скалере.
void SchematicView::ScalerMouseReleaseEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event)
{
	if((p_GraphicsScalerItem->p_SchEGGraph->uchSettingsBits & SCH_SETTINGS_EG_BIT_BUSY) || MainWindow::bBlockingGraphics)
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
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.uchChangesBits = SCH_CHANGES_LINK_BIT_SCR_PORT_POS;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_LINK_VARS, (char*)&p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars, sizeof(PSchLinkVars));
			}
			if(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ullIDDst ==
			   p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.ullIDInt)
			{
				p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.oSchLGraph.uchChangesBits = SCH_CHANGES_LINK_BIT_DST_PORT_POS;
				MainWindow::p_Client->AddPocketToOutputBufferC(
							PROTO_O_SCH_LINK_VARS, (char*)&p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars, sizeof(PSchLinkVars));
			}
		}
		ReleaseOccupiedAPFS();
		TrySendBufferToServer;
	}
	p_GraphicsScalerItem->OBMouseReleaseEvent(p_Event);
}

// Обработчик функции возврата вместилища скалера и его видов.
QRectF SchematicView::ScalerBoundingHandler(const GraphicsScalerItem* pc_GraphicsScalerItem)
{
	QRectF oQRectF;
	//
	if(pc_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(pc_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
		   SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			oQRectF.setX(-SCALER_CIR_DIM);
			oQRectF.setY(-SCALER_CIR_DIM);
			oQRectF.setWidth(SCALER_CIR_DIM * 2.0f);
			oQRectF.setHeight(SCALER_CIR_DIM * 2.0f);
		}
		else
		{
			oQRectF.setX(-SCALER_TR_DIM);
			oQRectF.setY(-SCALER_TR_DIM);
			oQRectF.setWidth(SCALER_TR_DIM * 2.0f);
			oQRectF.setHeight(SCALER_TR_DIM * 2.0f);
		}
	}
	else
	{
		oQRectF.setX(-SCALER_RECT_DIM);
		oQRectF.setY(-SCALER_RECT_DIM);
		oQRectF.setWidth(SCALER_RECT_DIM);
		oQRectF.setHeight(SCALER_RECT_DIM);
	}
	return oQRectF;
}

// Вычисление формы скалера окружности.
QPainterPath SchematicView::CalcCicrleScalerPath(const GraphicsScalerItem* pc_GraphicsScalerItem)
{
	QPainterPath oQPainterPathScaller;
	QPainterPath oQPainterPathParent;
	double dbR = pc_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW / 2.0f;
	double dbX = -GetDiagPointOnCircle(dbR);
	double dbDCorr = pc_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.oDbFrame.dbW -
					 SCALER_CIR_DIM_CORR;
	//
	oQPainterPathScaller.addEllipse(QPointF(0, 0), SCALER_CIR_DIM, SCALER_CIR_DIM);
	oQPainterPathParent.addEllipse(dbX, dbX, dbDCorr, dbDCorr);
	oQPainterPathScaller = oQPainterPathScaller.intersected(oQPainterPathParent);
	return oQPainterPathScaller;
}

// Обработчик функции возврата формы элемента и его видов.
QPainterPath SchematicView::ScalerShapeHandler(const GraphicsScalerItem* pc_GraphicsScalerItem)
{
	QPainterPath oQPainterPath;
	//
	if(pc_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
	   SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(pc_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
			SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			return CalcCicrleScalerPath(pc_GraphicsScalerItem);
		}
		else
		{
			oQPainterPath.addPolygon(oQPolygonFForTriangleScaler);
		}
	}
	else
	{
		oQPainterPath.addPolygon(oQPolygonFForRectScaler);
	}
	return oQPainterPath;
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
	if(p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits & SCH_SETTINGS_ELEMENT_BIT_EXTENDED)
	{
		if(p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph.uchSettingsBits &
		   SCH_SETTINGS_ELEMENT_BIT_RECEIVER)
		{
			p_Painter->drawPath(CalcCicrleScalerPath(p_GraphicsScalerItem));
		}
		else
		{
			p_Painter->drawConvexPolygon(oQPolygonFForTriangleScaler);
		}
	}
	else
	{
		p_Painter->drawPolygon(oQPolygonFForRectScaler);
	}
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
	p_GraphicsScalerItem->p_SchEGGraph = &p_GraphicsScalerItem->p_ParentInt->oPSchElementBaseInt.oPSchElementVars.oSchEGGraph;
}
