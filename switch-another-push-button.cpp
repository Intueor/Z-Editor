//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include <QEvent>
#include "switch-another-push-button.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс кнопки-переключателя заданных кнопок.
// Конструктор 1.
SwitchAnotherPushButton::SwitchAnotherPushButton(QWidget* p_Parent) :
	QPushButton(p_Parent), bReady(false), pvp_QPushButtonsForSwitching(nullptr), bLastEnabledState(true) {}

// Конструктор 2.
SwitchAnotherPushButton::SwitchAnotherPushButton(const QString& a_strText, QWidget* p_Parent) :
	QPushButton(a_strText, p_Parent), bReady(false), pvp_QPushButtonsForSwitching(nullptr), bLastEnabledState(true) {}

// Перехват событий.
bool SwitchAnotherPushButton::event(QEvent* p_Event)
{
	if(bReady)
	{
		if(p_Event->type() == QEvent::EnabledChange)
		{
			if(pvp_QPushButtonsForSwitching)
			{
				for(int iF = 0; iF != pvp_QPushButtonsForSwitching->count(); iF++)
				{
					QPushButton* p_QPushButton = pvp_QPushButtonsForSwitching->at(iF);
					//
					if(p_QPushButton)
					{
						if(p_QPushButton->isEnabled()) p_QPushButton->setEnabled(false);
						else p_QPushButton->setEnabled(true);
					}
				}
			}
		}
	}
	return QPushButton::event(p_Event);
}
