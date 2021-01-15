//== ВКЛЮЧЕНИЯ.
#include <QLineEdit>
#include "safe-searching-line-edit.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс безопасной строки поиска.
// Конструктор 1.
SafeSearchingLineEdit::SafeSearchingLineEdit(QWidget* p_parent) :
	QLineEdit(p_parent), bLastButtDefStatus(false), bLastButtEnStatus(false),
	p_QPushButtonForNotDefault(nullptr), p_QPushButtonForDisable(nullptr), bUserInside(false) {}

// Конструктор 2.
SafeSearchingLineEdit::SafeSearchingLineEdit(const QString& a_strText, QWidget* p_parent) :
	QLineEdit(a_strText, p_parent), bLastButtDefStatus(false), bLastButtEnStatus(false),
	p_QPushButtonForNotDefault(nullptr), p_QPushButtonForDisable(nullptr), bUserInside(false) {}

// Событие входа фокуса.
void SafeSearchingLineEdit::focusInEvent(QFocusEvent* p_Event)
{
	if(p_QPushButtonForNotDefault)
	{
		bLastButtDefStatus = p_QPushButtonForNotDefault->isDefault();
		oQTimer.singleShot(0, this, SLOT(SetButtonSetNotDefault()));
	}
	if(p_QPushButtonForDisable)
	{
		bLastButtEnStatus = p_QPushButtonForDisable->isEnabled();
		oQTimer.singleShot(0, this, SLOT(SetButtonSetDisabled()));
	}
	bUserInside = true;
	QLineEdit::focusInEvent(p_Event);
}

// Событие входа фокуса.
void SafeSearchingLineEdit::focusOutEvent(QFocusEvent* p_Event)
{
	if(p_QPushButtonForNotDefault)
	{
		oQTimer.singleShot(0, this, SLOT(SetButtonRestoreDefault()));
	}
	if(p_QPushButtonForDisable)
	{
		oQTimer.singleShot(0, this, SLOT(SetButtonRestoreEnabled()));
	}
	bUserInside = false;
	this->clear();
	QLineEdit::focusOutEvent(p_Event);
}

// Выключение кнопок.
void SafeSearchingLineEdit::SetButtonSetNotDefault()
{
	p_QPushButtonForNotDefault->setDefault(false);
}

// Включение кнопок.
void SafeSearchingLineEdit::SetButtonSetDisabled()
{
	p_QPushButtonForDisable->setEnabled(false);
}

// Выключение кнопок.
void SafeSearchingLineEdit::SetButtonRestoreDefault()
{
	p_QPushButtonForNotDefault->setDefault(bLastButtDefStatus);
}

// Включение кнопок.
void SafeSearchingLineEdit::SetButtonRestoreEnabled()
{
	p_QPushButtonForDisable->setEnabled(bLastButtEnStatus);
}
