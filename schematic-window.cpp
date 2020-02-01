//== ВКЛЮЧЕНИЯ.
#include "schematic-window.h"
#include "ui_schematic-window.h"
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
QGraphicsScene* SchematicWindow::p_QGraphicsScene = nullptr;
bool SchematicWindow::bRefClose = false;

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
	p_ui->oSchematicView->setBackgroundBrush(QBrush(QColor(24, 36, 28, 255), Qt::SolidPattern));
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
}

// Обновление от таймера мерцания выбранных элементов.
void SchematicWindow::UpdateSelectionFlash()
{

}

// Очистка сцены.
void SchematicWindow::ClearScene()
{
	p_QGraphicsScene->clear();
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
