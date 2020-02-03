//== ВКЛЮЧЕНИЯ.
#include "main-window.h"
#include <QApplication>

//== ФУНКЦИИ.
// Точка входа в приложение.
int main(int argc, char *argv[])
							///< \param[in] argc Заглушка.
							///< \param[in] argv Заглушка.
							///< \return Общий результат работы.
{
	int iExecResult;
	QApplication oApplication(argc, argv);
	oApplication.setAttribute(Qt::AA_DisableWindowContextHelpButton);
	MainWindow wMainWindow;
	SchematicWindow wSchematicWindow;
	//
	if(wMainWindow.iInitRes == RETVAL_OK)
	{
		setlocale(LC_NUMERIC, "en_US.UTF-8");
		wMainWindow.p_SchematicWindow = &wSchematicWindow; // Передача главному окну указателя на окно схематического обзора для управления.
		wSchematicWindow.p_MainWindow = &wMainWindow; // И наоборот.
		wMainWindow.SetSchWindowSignalConnections(); // Соединение сигналов с окном схемы.
		if(wMainWindow.p_UISettings->value("Schema").toBool()) // Если в процессе инициализации главного окна был принят флаг открытия, то первичный показ.
		{
			wSchematicWindow.show();
		}
		wMainWindow.show();
		iExecResult = oApplication.exec();
	}
	else iExecResult = RETVAL_ERR;
	return iExecResult;
}
