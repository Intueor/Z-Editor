//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include "set_proposed_number_dialog.h"
#include "ui_set_proposed_number_dialog.h"
#include "../Z-Hub/main-hub.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
int* Set_Proposed_Number_Dialog::p_iNumberInt;

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога установки предложенной строки.
// Конструктор.
Set_Proposed_Number_Dialog::Set_Proposed_Number_Dialog(char* p_chDialogCaption, int iMin, int iMax, int iStep, int* p_iNumber, QWidget* p_parent) :
	QDialog(p_parent),
	p_ui(new Ui::Set_Proposed_Number_Dialog)
{
	p_ui->setupUi(this);
	p_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Принять");
	p_ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setText("Отмена");
	setWindowTitle(p_chDialogCaption);
	p_ui->spinBox->setRange(iMin, iMax);
	p_ui->spinBox->setSingleStep(iStep);
	p_ui->spinBox->setValue(*p_iNumber);
	p_ui->spinBox->selectAll();
	p_iNumberInt = p_iNumber;
}

// Деструктор.
Set_Proposed_Number_Dialog::~Set_Proposed_Number_Dialog()
{
	delete p_ui;
}

// Принято.
void Set_Proposed_Number_Dialog::accept()
{
	*p_iNumberInt = p_ui->spinBox->value();
	done(DIALOGS_ACCEPT);
}

// Отменено.
void Set_Proposed_Number_Dialog::reject()
{
	done(DIALOGS_REJECT);
}
