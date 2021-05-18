//== ВКЛЮЧЕНИЯ.
#include <QKeyEvent>
#include "application.h"
#include "schematic-view.h"
#include "schematic-window.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс приложения.
// Конструктор.
Application::Application(int& iArgc, char** pp_Argv, int) : QApplication(iArgc, pp_Argv, ApplicationFlags) {}

// Переопределение обработки всех событий из-за болкировки текстового ввода в поле пользовательского редактора внутри элемента.
bool Application::notify(QObject* p_QObject, QEvent* p_QEvent)
{
	if(SchematicWindow::p_SchematicView)
	{
		if(p_QObject == SchematicWindow::p_SchematicView)
		{
			if(!SchematicView::vp_QWidgetsKeyReceivers.contains(static_cast<QWidget*>(p_QObject)))
			{
				if(p_QEvent->type() == QEvent::KeyPress)
				{
					QKeyEvent* p_QKeyEvent = static_cast<QKeyEvent*>(p_QEvent);
					//
					for(int iW = 0; iW != SchematicView::vp_QWidgetsKeyReceivers.count(); iW++)
					{
						QWidget* p_QWidget = SchematicView::vp_QWidgetsKeyReceivers.at(iW);
						//
						if(p_QWidget->hasFocus())
						{
							QCoreApplication::sendEvent(p_QWidget, p_QKeyEvent);
						}
					}
				}
			}
		}
	}
	return QApplication::notify(p_QObject, p_QEvent);
}
