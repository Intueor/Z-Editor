//== ВКЛЮЧЕНИЯ.
#include <QLineEdit>
#include <QList>
#include <QVector>
#include "safe-searching-line-edit.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс безопасной строки поиска.
// Конструктор 1.
SafeSearchingLineEdit::SafeSearchingLineEdit(QWidget* p_Parent) :
	QLineEdit(p_Parent), bUserInside(false), bLastButtDefStatus(false),
	p_QPushButtonForNotDefault(nullptr), plp_QPushButtonsForDisable(nullptr) {}

// Конструктор 2.
SafeSearchingLineEdit::SafeSearchingLineEdit(const QString& a_strText, QWidget* p_Parent) :
	QLineEdit(a_strText, p_Parent), bUserInside(false), bLastButtDefStatus(false),
	p_QPushButtonForNotDefault(nullptr), plp_QPushButtonsForDisable(nullptr) {}

// Событие входа фокуса.
void SafeSearchingLineEdit::focusInEvent(QFocusEvent* p_Event)
{
	if(p_QPushButtonForNotDefault)
	{
		bLastButtDefStatus = p_QPushButtonForNotDefault->isDefault();
		oQTimer.singleShot(0, this, SLOT(SetButtonNotDefaultus()));
	}
	if(plp_QPushButtonsForDisable)
	{
		vLastButtEnStatuses.clear();
		for(int iF = 0; iF != plp_QPushButtonsForDisable->count(); iF++)
		{
			vLastButtEnStatuses.append(plp_QPushButtonsForDisable->at(iF)->isEnabled());
		}
		oQTimer.singleShot(0, this, SLOT(SetButtonsDisabled()));
	}
	bUserInside = true;
	QLineEdit::focusInEvent(p_Event);
}

// Событие входа фокуса.
void SafeSearchingLineEdit::focusOutEvent(QFocusEvent* p_Event)
{
	if(p_QPushButtonForNotDefault)
	{
		oQTimer.singleShot(0, this, SLOT(RestoreButtonDefault()));
	}
	if(plp_QPushButtonsForDisable)
	{
		oQTimer.singleShot(0, this, SLOT(RestoreButtonsEnabled()));
	}
	bUserInside = false;
	this->clear();
	QLineEdit::focusOutEvent(p_Event);
}

// Выключение кнопок.
void SafeSearchingLineEdit::SetButtonNotDefaultus()
{
	p_QPushButtonForNotDefault->setDefault(false);
}

// Включение кнопок.
void SafeSearchingLineEdit::SetButtonsDisabled()
{
	for(int iF = 0; iF != plp_QPushButtonsForDisable->count(); iF++)
	{
		plp_QPushButtonsForDisable->at(iF)->setEnabled(false);
	}
}

// Выключение кнопок.
void SafeSearchingLineEdit::RestoreButtonDefault()
{
	p_QPushButtonForNotDefault->setDefault(bLastButtDefStatus);
}

// Включение кнопок.
void SafeSearchingLineEdit::RestoreButtonsEnabled()
{
	for(int iF = plp_QPushButtonsForDisable->count() - 1; iF != -1; iF--)
	{
		plp_QPushButtonsForDisable->at(iF)->setEnabled(vLastButtEnStatuses.at(iF));
	}
}

// Изменение запомненного статуса включённости кнопки по указателю в процессе редактирования.
void SafeSearchingLineEdit::SetRecordedButtonEnabledStatus(QPushButton* p_QPushButton, bool bStatus)
{
	if(plp_QPushButtonsForDisable->contains(p_QPushButton))
	{
		int iN = plp_QPushButtonsForDisable->indexOf(p_QPushButton);
		//
		if(!vLastButtEnStatuses.isEmpty()) vLastButtEnStatuses.replace(iN, bStatus);
	}
}

// Проверка текущего статуса редактирования.
bool SafeSearchingLineEdit::IsUserInside()
{
	return bUserInside;
}
