//== ВКЛЮЧЕНИЯ.
#include "schematic-window.h"
#include "ui_schematic-window.h"
#include <math.h>
#include <QTimer>

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
QPolygon SchematicWindow::oPolygonForScaler;
QTimer SchematicWindow::oQTimerSelectionFlashing;
bool SchematicWindow::bRefClose = false;
QBrush SchematicWindow::oQBrushDark;
QBrush SchematicWindow::oQBrushLight;
QPen SchematicWindow::oQPenWhite;
QPen SchematicWindow::oQPenBlack;
QPen SchematicWindow::oQPenWhiteTransparent;
QPen SchematicWindow::oQPenBlackTransparent;
QPen SchematicWindow::oQPenElementFrameFlash;
QPen SchematicWindow::oQPenGroupFrameFlash;
QVector<GraphicsElementItem*> SchematicWindow::vp_SelectedElements;
QVector<GraphicsElementItem*> SchematicWindow::vp_SelectedFreeElements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_SelectedGroups;
QVector<GraphicsElementItem*> SchematicWindow::vp_Elements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_Groups;
QVector<GraphicsLinkItem*> SchematicWindow::vp_Links;
QVector<GraphicsPortItem*> SchematicWindow::vp_Ports;
QVector<GraphicsElementItem*> SchematicWindow::vp_LonelyElements;
unsigned char SchematicWindow::uchElementSelectionFlashCounter = 1;
unsigned char SchematicWindow::uchGroupSelectionFlashCounter = 1;
QGraphicsScene* SchematicWindow::p_QGraphicsScene = nullptr;
qreal SchematicWindow::dbObjectZPos;
QMenu* SchematicWindow::p_Menu = nullptr;
Qt::BrushStyle SchematicWindow::iLStyle, SchematicWindow::iDStyle;
GraphicsElementItem* SchematicWindow::p_GraphicsElementItem = nullptr;
bool SchematicWindow::bCleaningSceneNow = true;
SchematicView* SchematicWindow::p_SchematicView = nullptr;

//== ФУНКЦИИ КЛАССОВ.
//== Класс окна обзора.
// Конструктор.
SchematicWindow::SchematicWindow(QWidget* p_parent) : QMainWindow(p_parent)
{
	LOG_CTRL_INIT;
	//
	oQBrushLight.setColor(QColor(170, 170, 170, 255));
	oQBrushLight.setStyle(Qt::SolidPattern);
	oQBrushDark.setColor(QColor(64, 64, 64, 255));
	oQBrushDark.setStyle(Qt::SolidPattern);
	oQPenWhite.setColor(Qt::white);
	oQPenBlack.setColor(Qt::black);
	oQPenWhiteTransparent.setColor(QColor(255, 255, 255, 96));
	oQPenBlackTransparent.setColor(QColor(0, 0, 0, 96));
	oQPenElementFrameFlash.setColor(Qt::white);
	oQPenElementFrameFlash.setWidth(3);
	oQPenGroupFrameFlash.setColor(Qt::white);
	oQPenGroupFrameFlash.setWidth(3);
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
	oScene.setSceneRect(-3000, -3000, 6000, 6000);
	oScene.setItemIndexMethod(QGraphicsScene::NoIndex);
	p_ui->oSchematicView->setScene(&oScene);
	p_ui->oSchematicView->SetSchematicViewFrameChangedCB(SchematicViewFrameChangedCallback);
	p_ui->oSchematicView->setBackgroundBrush(QBrush(SchBackgroundActive, Qt::SolidPattern));
	//
	connect(&oQTimerSelectionFlashing, SIGNAL(timeout()), this, SLOT(UpdateSelectionFlash()));
	oQTimerSelectionFlashing.start(6);
	//
	oPolygonForScaler.push_front(QPoint(-SCALER_DIM, 0));
	oPolygonForScaler.push_front(QPoint(0, -SCALER_DIM));
	oPolygonForScaler.push_front(QPoint(0, 0));
	oPolygonForScaler.push_front(QPoint(-SCALER_DIM, 0));
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
#ifdef WIN32
		if(bSelectionPresent)
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
void SchematicWindow::closeEvent(QCloseEvent *event)
{
	p_UISettings->setValue("Geometry", saveGeometry());
	p_UISettings->setValue("WindowState", saveState());
	if((p_MainWindow != nullptr) && bRefClose == false)
	{
		p_MainWindow->UncheckSchemaCheckbox();
	}
	//
	QMainWindow::closeEvent(event);
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
	iDStyle = SchematicWindow::oQBrushDark.style();
	SchematicWindow::oQBrushLight.setStyle(iStyle);
	SchematicWindow::oQBrushDark.setStyle(iStyle);
}

// Отмена временного стиля кистей общего пользования.
void SchematicWindow::RestoreBrushesStyles()
{
	SchematicWindow::oQBrushLight.setStyle(iLStyle);
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
							PROTO_C_SCH_READY, (char*)&oPSchReadyFrame, sizeof(PSchReadyFrame)));
		}
	}
}
