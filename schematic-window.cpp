//== ВКЛЮЧЕНИЯ.
#include "schematic-window.h"
#include "ui_schematic-window.h"
#include <math.h>
#include <QTimer>
#include <QGraphicsSceneEvent>

//== МАКРОСЫ.
#define LOG_NAME				"schematic-window"
#define LOG_DIR_PATH			"../Z-Editor/logs/"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS(SchematicWindow)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(SchematicWindow)
Ui::SchematicWindow* SchematicWindow::p_ui = new Ui::SchematicWindow;
const char* SchematicWindow::cp_chUISettingsName = E_SCHEMATICWINDOW_UI_CONF_PATH;
QSettings* SchematicWindow::p_UISettings = nullptr;
MainWindow* SchematicWindow::p_MainWindow = nullptr;
QTimer SchematicWindow::oQTimerSelectionFlashing;
bool SchematicWindow::bRefClose = false;
QBrush SchematicWindow::oQBrushDark;
QBrush SchematicWindow::oQBrushLight;
QBrush SchematicWindow::oQBrushGray;
QPen SchematicWindow::oQPenWhite;
QPen SchematicWindow::oQPenBlack;
QPen SchematicWindow::oQPenWhiteTransparent;
QPen SchematicWindow::oQPenBlackTransparent;
QPen SchematicWindow::oQPenElementFrameFlash;
QPen SchematicWindow::oQPenGroupFrameFlash;
QPen SchematicWindow::oQPenPortFrameFlash;
QPen SchematicWindow::oQPenSelectionDash;
QPen SchematicWindow::oQPenSelectionDot;
QVector<GraphicsElementItem*> SchematicWindow::vp_SelectedElements;
QVector<GraphicsElementItem*> SchematicWindow::vp_SelectedFreeElements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_SelectedFreeGroups;
QVector<GraphicsGroupItem*> SchematicWindow::vp_SelectedGroups;
QVector<GraphicsElementItem*> SchematicWindow::vp_Elements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_Groups;
QVector<GraphicsLinkItem*> SchematicWindow::vp_Links;
QVector<GraphicsPortItem*> SchematicWindow::vp_Ports;
QVector<GraphicsElementItem*> SchematicWindow::vp_LonelyElements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_LonelyGroups;
unsigned char SchematicWindow::uchElementSelectionFlashCounter = 1;
unsigned char SchematicWindow::uchGroupSelectionFlashCounter = 1;
unsigned char SchematicWindow::uchPortSelectionFlashCounter = 1;
QGraphicsScene* SchematicWindow::p_QGraphicsScene = nullptr;
qreal SchematicWindow::dbObjectZPos;
SafeMenu* SchematicWindow::p_SafeMenu = nullptr;
Qt::BrushStyle SchematicWindow::iLStyle, SchematicWindow::iDStyle, SchematicWindow::iGStyle;
GraphicsElementItem* SchematicWindow::p_GraphicsElementItem = nullptr;
bool SchematicWindow::bCleaningSceneNow = true;
SchematicView* SchematicWindow::p_SchematicView = nullptr;
GraphicsFrameItem* SchematicWindow::p_GraphicsFrameItemForPortFlash = nullptr;

//== ФУНКЦИИ КЛАССОВ.
//== Класс окна обзора.
// Конструктор.
SchematicWindow::SchematicWindow(QWidget* p_parent) : QMainWindow(p_parent)
{
	LOG_CTRL_INIT;
	//
	oQBrushLight.setColor(QColor(170, 170, 170, 255)); oQBrushLight.setStyle(Qt::SolidPattern);
	oQBrushDark.setColor(QColor(64, 64, 64, 255)); oQBrushDark.setStyle(Qt::SolidPattern);
	oQBrushGray.setColor(QColor(100, 100, 100, 255)); oQBrushGray.setStyle(Qt::SolidPattern);
	oQPenWhite.setColor(Qt::white); oQPenWhite.setJoinStyle(Qt::MiterJoin);
	oQPenBlack.setColor(Qt::black); oQPenBlack.setJoinStyle(Qt::MiterJoin);
	oQPenWhiteTransparent.setColor(QColor(255, 255, 255, 96)); oQPenWhiteTransparent.setJoinStyle(Qt::MiterJoin);
	oQPenBlackTransparent.setColor(QColor(0, 0, 0, 96)); oQPenBlackTransparent.setJoinStyle(Qt::MiterJoin);
	oQPenElementFrameFlash.setColor(Qt::white); oQPenElementFrameFlash.setWidth(3.1f); oQPenElementFrameFlash.setJoinStyle(Qt::MiterJoin);
	oQPenGroupFrameFlash.setColor(Qt::white); oQPenGroupFrameFlash.setWidth(3.1f); oQPenGroupFrameFlash.setJoinStyle(Qt::MiterJoin);
	oQPenPortFrameFlash.setColor(Qt::white); oQPenPortFrameFlash.setWidth(3.1f); oQPenPortFrameFlash.setJoinStyle(Qt::MiterJoin);
	oQPenSelectionDash.setColor(QColor(255, 255, 255, 255)); oQPenSelectionDash.setStyle(Qt::PenStyle::DashLine);
	oQPenSelectionDash.setJoinStyle(Qt::MiterJoin);
	oQPenSelectionDot.setColor(QColor(0, 5, 10, 255)); oQPenSelectionDot.setStyle(Qt::PenStyle::SolidLine);
	oQPenSelectionDot.setJoinStyle(Qt::MiterJoin);
	//
	p_QGraphicsScene = &this->oScene;
	MainWindow::p_SchematicWindow = this;
	p_UISettings = new QSettings(cp_chUISettingsName, QSettings::IniFormat);
	p_ui->setupUi(this);
	p_SchematicView = p_ui->oSchematicView;
	if(IsFileExists((char*)cp_chUISettingsName))
	{
		LOG_P_2(LOG_CAT_I, "Restore UI states.");
		// MainWidow.
		if(!restoreGeometry(p_UISettings->value("Geometry").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, "Can`t restore Geometry UI state.");
		}
		if(!restoreState(p_UISettings->value("WindowState").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, "Can`t restore WindowState UI state.");
		}
		// Splitters.
	}
	else
	{
		LOG_P_0(LOG_CAT_W, "SchematicWindow_ui.ini is missing and will be created by default at the exit from program.");
	}
	//
	oScene.setSceneRect(0 - (SCENE_DIM_X / 2), 0 - (SCENE_DIM_Y / 2), SCENE_DIM_X, SCENE_DIM_Y);
	oScene.setItemIndexMethod(QGraphicsScene::NoIndex);
	p_ui->oSchematicView->setScene(&oScene);
	p_ui->oSchematicView->SetSchematicViewFrameChangedCB(SchematicViewFrameChangedCallback);
	p_ui->oSchematicView->setBackgroundBrush(QBrush(SchBackgroundActive, Qt::SolidPattern));
	//
	connect(&oQTimerSelectionFlashing, SIGNAL(timeout()), this, SLOT(UpdateSelectionFlash()));
	oQTimerSelectionFlashing.start(6);
}

// Деструктор.
SchematicWindow::~SchematicWindow()
{
	delete p_ui;
	p_QGraphicsScene = nullptr;
}

// Обновление от таймера мерцания выбранных элементов.
void SchematicWindow::UpdateSelectionFlash()
{
	unsigned char uchC;
	int iC;
	//
	if(!MainWindow::bBlockingGraphics)
	{
#ifdef WIN32
		bool bSelectionPresent = false;
#endif
		//
		uchElementSelectionFlashCounter += 2;
		uchGroupSelectionFlashCounter += 1;
		uchPortSelectionFlashCounter += 3;
		uchC = (sinf((float)(uchElementSelectionFlashCounter) / 81.169f)) * 255;
		oQPenElementFrameFlash.setColor(QColor(uchC, uchC, uchC));
		iC = vp_SelectedElements.count();
#ifdef WIN32
		if(iC > 0)
		{
			bSelectionPresent = true;
		}
#endif
		for(int iE = 0; iE != iC; iE ++)
		{
			vp_SelectedElements.at(iE)->p_GraphicsFrameItem->update();
		}
		uchC = (sinf((float)(uchGroupSelectionFlashCounter) / 81.169f)) * 255;
		oQPenGroupFrameFlash.setColor(QColor(uchC, uchC, uchC));
		iC = vp_SelectedGroups.count();
#ifdef WIN32
		if(iC > 0)
		{
			bSelectionPresent = true;
		}
#endif
		for(int iE = 0; iE != iC; iE ++)
		{
			vp_SelectedGroups.at(iE)->p_GraphicsFrameItem->update();
		}
		if(p_GraphicsFrameItemForPortFlash)
		{
			p_GraphicsFrameItemForPortFlash->update();
			uchC = (sinf((float)(uchPortSelectionFlashCounter) / 81.169f)) * 255;
			oQPenPortFrameFlash.setColor(QColor(uchC, uchC, uchC));
		}
#ifdef WIN32
		if(bSelectionPresent | (p_GraphicsFrameItemForPortFlash != nullptr))
		{
			MainWindow::p_SchematicWindow->UpdateScene();
		}
#endif
	}
}

// Очистка сцены.
void SchematicWindow::ClearScene()
{
	p_QGraphicsScene->clear();
	vp_Elements.clear();
	vp_Groups.clear();
	vp_Links.clear();
	vp_Ports.clear();
	vp_LonelyElements.clear();
	vp_SelectedElements.clear();
	vp_SelectedFreeElements.clear();
	vp_SelectedFreeGroups.clear();
	vp_SelectedGroups.clear();
	bCleaningSceneNow = false;
}

// Обновление сцены.
void SchematicWindow::UpdateScene()
{
	p_QGraphicsScene->update();
}

// Закрытие с флагом внешнего управления.
void SchematicWindow::RefClose()
{
	bRefClose = true;
	close();
}

// Процедуры при закрытии окна обзора.
void SchematicWindow::closeEvent(QCloseEvent *p_Event)
{
	p_UISettings->setValue("Geometry", saveGeometry());
	p_UISettings->setValue("WindowState", saveState());
	if((p_MainWindow != nullptr) && bRefClose == false)
	{
		p_MainWindow->UncheckSchemaCheckbox();
	}
	//
	QMainWindow::closeEvent(p_Event);
}

// Получение указателя на окно обзора.
SchematicView* SchematicWindow::GetSchematicView()
{
	return p_ui->oSchematicView;
}

// Установка временного стиля кистей общего пользования.
void SchematicWindow::SetTempBrushesStyle(Qt::BrushStyle iStyle)
{
	iLStyle = SchematicWindow::oQBrushLight.style();
	iGStyle = SchematicWindow::oQBrushGray.style();
	iDStyle = SchematicWindow::oQBrushDark.style();
	SchematicWindow::oQBrushLight.setStyle(iStyle);
	SchematicWindow::oQBrushGray.setStyle(iStyle);
	SchematicWindow::oQBrushDark.setStyle(iStyle);
}

// Отмена временного стиля кистей общего пользования.
void SchematicWindow::RestoreBrushesStyles()
{
	SchematicWindow::oQBrushLight.setStyle(iLStyle);
	SchematicWindow::oQBrushGray.setStyle(iLStyle);
	SchematicWindow::oQBrushDark.setStyle(iDStyle);
}

// Кэлбэк обработки изменения окна обзора от класса вида.
void SchematicWindow::SchematicViewFrameChangedCallback(QRectF oQRectFVisibleFrame)
{
	PSchReadyFrame oPSchReadyFrame;
	//
	if(MainWindow::p_Client != nullptr)
	{
		if(!MainWindow::bBlockingGraphics)
		{
			QRealToDbFrame(oQRectFVisibleFrame, oPSchReadyFrame.oDbFrame);
			LCHECK_BOOL(MainWindow::p_Client->SendToServerImmediately(
							PROTO_O_SCH_READY, (char*)&oPSchReadyFrame, sizeof(PSchReadyFrame)));
		}
	}
}

// Закрытие и сброс меню.
void SchematicWindow::ResetMenu()
{
	if(p_SafeMenu != nullptr)
	{
		p_SafeMenu->close();
		p_SafeMenu->deleteLater();
		p_SafeMenu = nullptr;
	#ifndef WIN32
		QMouseEvent oMEvent(QEvent::Type::MouseMove, p_SchematicView->mapFromGlobal(QCursor::pos()), Qt::NoButton, Qt::NoButton, Qt::NoModifier);
		p_SchematicView->mouseMoveEvent(&oMEvent);
	#endif
	}
}
