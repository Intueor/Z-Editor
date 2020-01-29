//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include "ui_main-window.h"

//== МАКРОСЫ.
#define LOG_NAME				"main-window"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
LOGDECL_INIT_INCLASS(MainWindow)
LOGDECL_INIT_PTHRD_INCLASS_OWN_ADD(MainWindow)
int MainWindow::iInitRes;
Ui::MainWindow* MainWindow::p_ui = new Ui::MainWindow;
QSettings* MainWindow::p_UISettings = nullptr;
const char* MainWindow::cp_chUISettingsName = E_MAINWINDOW_UI_CONF_PATH;
QLabel* MainWindow::p_QLabelStatusBarText;
SchematicWindow* MainWindow::p_SchematicWindow = nullptr;
bool MainWindow::bSchemaIsOpened = false;

//== ФУНКЦИИ КЛАССОВ.
//== Класс главного окна.
// Конструктор.
MainWindow::MainWindow(QWidget* p_parent) :
	QMainWindow(p_parent)
{
	// Для избежания ошибки при доступе из другого потока.
	qRegisterMetaType<QVector<int>>("QVector<int>");
	//
	LOG_CTRL_INIT;
	LOG_P_0(LOG_CAT_I, "START.");
	iInitRes = RETVAL_OK;
	p_UISettings = new QSettings(cp_chUISettingsName, QSettings::IniFormat);
	p_ui->setupUi(this);
	p_QLabelStatusBarText = new QLabel(this);
	p_ui->statusBar->addWidget(p_QLabelStatusBarText);
	p_QLabelStatusBarText->setText(tr("Инициализация."));
	if(IsFileExists((char*)cp_chUISettingsName))
	{
		LOG_P_2(LOG_CAT_I, "Restore UI states.");
		// Splitters.

		// MainWidow.
		if(!restoreGeometry(p_UISettings->value("Geometry").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, "Can`t restore Geometry UI state.");
			RETVAL_SET(RETVAL_ERR);
		}
		if(!restoreState(p_UISettings->value("WindowState").toByteArray()))
		{
			LOG_P_1(LOG_CAT_E, "Can`t restore WindowState UI state.");
			RETVAL_SET(RETVAL_ERR);
		}
		bSchemaIsOpened = p_UISettings->value("Schema").toBool();
	}
	else
	{
		LOG_P_0(LOG_CAT_W, "mainwidow_ui.ini is missing and will be created by default at the exit from program.");
	}
	p_ui->actionSchematic->setChecked(bSchemaIsOpened);
	p_QLabelStatusBarText->setText(tr("Готов к работе."));
}

// Деструктор.
MainWindow::~MainWindow()
{
	if(RETVAL == RETVAL_OK)
	{
		LOG_P_0(LOG_CAT_I, "EXIT.")
	}
	else
	{
		LOG_P_0(LOG_CAT_E, "EXIT WITH ERROR: " << RETVAL);
	}
	LOG_CLOSE;
	delete p_ui;
}

// Процедуры при закрытии окна приложения.
void MainWindow::closeEvent(QCloseEvent *event)
{
	p_QLabelStatusBarText->setText(tr("Выключение."));
	p_ui->statusBar->repaint();
	// Main.
	p_UISettings->setValue("Geometry", saveGeometry());
	p_UISettings->setValue("WindowState", saveState());
	p_UISettings->setValue("Schema", bSchemaIsOpened);
	// Splitters.

	// Other.

	//
	if(p_SchematicWindow != 0)
	{
		if(p_SchematicWindow->isVisible())
			p_SchematicWindow->RefClose();
	}
	QMainWindow::closeEvent(event);
}

// Установка соединения сигнала на обновление граф. окна с граф. окном.
void MainWindow::SetSchViewSignalConnection()
{
	connect(this, SIGNAL(RemoteUpdateSchView()), p_SchematicWindow, SLOT(UpdateScene()));
}

// При переключении кнопки 'Schematic'.
void MainWindow::on_actionSchematic_triggered(bool checked)
{
	bSchemaIsOpened = checked;
	p_UISettings->setValue("Schema", bSchemaIsOpened);
	if(bSchemaIsOpened)
	{
		p_SchematicWindow->show();
	}
	else
	{
		p_SchematicWindow->hide();
	}
}

// Для внешнего отключения чекбокса кнопки 'Схема'.
void MainWindow::UncheckSchemaCheckbox()
{
	p_ui->actionSchematic->setChecked(false);
	bSchemaIsOpened = false;
	p_UISettings->setValue("Schema", bSchemaIsOpened);
}
