//== ВКЛЮЧЕНИЯ.
#include "schematic-window.h"
#include "ui_schematic-window.h"
#include <math.h>
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
bool SchematicWindow::bRefClose = false;
QVector<GraphicsElementItem*> SchematicWindow::vp_SelectedElements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_SelectedGroups;
QVector<GraphicsElementItem*> SchematicWindow::vp_Elements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_Groups;
QVector<GraphicsLinkItem*> SchematicWindow::vp_Links;
QVector<GraphicsPortItem*> SchematicWindow::vp_Ports;
QVector<PSchPseudonym> SchematicWindow::vp_PSchPseudonyms;
QVector<GraphicsElementItem*> SchematicWindow::vp_LonelyElements;
QVector<GraphicsGroupItem*> SchematicWindow::vp_LonelyGroups;
QGraphicsScene* SchematicWindow::p_QGraphicsScene = nullptr;
SafeMenu* SchematicWindow::p_SafeMenu = nullptr;
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
		LOG_P_0(LOG_CAT_W, "[SchematicWindow_ui.ini] is missing and will be created by default at the exit from program.");
	}
	//
	oScene.setSceneRect(0 - (SCENE_DIM_X / 2), 0 - (SCENE_DIM_Y / 2), SCENE_DIM_X, SCENE_DIM_Y);
	oScene.setItemIndexMethod(QGraphicsScene::NoIndex);
	p_ui->oSchematicView->setScene(&oScene);
	p_ui->oSchematicView->SetSchematicViewFrameChangedCB(SchematicViewFrameChangedCallback);
	p_ui->oSchematicView->setBackgroundBrush(QBrush(SchBackgroundActive, Qt::SolidPattern));
}

// Деструктор.
SchematicWindow::~SchematicWindow()
{
	delete p_ui;
	p_QGraphicsScene = nullptr;
}

// Очистка сцены.
void SchematicWindow::ClearScene()
{
	p_QGraphicsScene->clear();
	vp_Elements.clear();
	vp_Groups.clear();
	vp_Links.clear();
	vp_Ports.clear();
	vp_PSchPseudonyms.clear();
	vp_LonelyElements.clear();
	vp_SelectedElements.clear();
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
