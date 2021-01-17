//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include <QApplication>
#include <QStyleFactory>

//== ФУНКЦИИ.
// Точка входа в приложение.
int main(int argc, char *argv[])
							///< \param[in] argc Заглушка.
							///< \param[in] argv Заглушка.
							///< \return Общий результат работы.
{
	int iExecResult;
	QApplication oApplication(argc, argv);
	//oApplication.setAttribute(Qt::AA_DisableWindowContextHelpButton);
	oApplication.setStyle(QStyleFactory::create("Fusion"));
	SchematicWindow wSchematicWindow;
	MainWindow wMainWindow;
	//
	if(wMainWindow.iInitRes == RETVAL_OK)
	{
		setlocale(LC_NUMERIC, "en_US.UTF-8");
		wMainWindow.p_SchematicWindow = &wSchematicWindow; // Передача главному окну указателя на окно схематического обзора для управления.
		wSchematicWindow.p_MainWindow = &wMainWindow; // И наоборот.
		if(wMainWindow.p_UISettings->value("Schema").toBool()) // Если при инициализации гл. окна был принят флаг открытия, то первичный показ.
		{
			wSchematicWindow.show();
		}
		wMainWindow.show();
		iExecResult = oApplication.exec();
	}
	else iExecResult = RETVAL_ERR;
	return iExecResult;
}
