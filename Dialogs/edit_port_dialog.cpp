//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include "edit_port_dialog.h"
#include "ui_edit_port_dialog.h"
#include "../Z-Hub/main-hub.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
QVector<Edit_Port_Dialog::PortInfo>* Edit_Port_Dialog::pv_PortsInt;
int* Edit_Port_Dialog::p_iNumberInt;

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога настройки порта.
// Конструктор.
Edit_Port_Dialog::Edit_Port_Dialog(char* p_chDialogCaption, QVector<PortInfo>* pv_Ports, int* p_iNumber, QWidget* p_parent) :
	QDialog(p_parent),
	p_ui(new Ui::Edit_Port_Dialog)
{
	p_ui->setupUi(this);
	setWindowTitle(p_chDialogCaption);
	p_ui->spinBox->setValue(*p_iNumber);
	p_ui->spinBox->selectAll();
	pv_PortsInt = pv_Ports;
	p_iNumberInt = p_iNumber;
}

// Деструктор.
Edit_Port_Dialog::~Edit_Port_Dialog()
{
	delete p_ui;
}

// Принято.
void Edit_Port_Dialog::accept()
{
	*p_iNumberInt = p_ui->spinBox->value();
	done(DIALOGS_ACCEPT);
}

// Отменено.
void Edit_Port_Dialog::reject()
{
	done(DIALOGS_REJECT);
}
