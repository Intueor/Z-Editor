//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include "set-password-dialog.h"
#include "ui_set-password-dialog.h"
#include "../Z-Hub/main-hub.h"
#include "../Z-Hub/Server/net-hub-defs.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
char* Set_Password_Dialog::p_chPasswordInt;

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога установки пароля.
// Конструктор.
Set_Password_Dialog::Set_Password_Dialog(char* p_chPassword, QWidget* p_parent) :
	QDialog(p_parent),
	p_ui(new Ui::Set_Password_Dialog)
{
	p_ui->setupUi(this);
	p_ui->retranslateUi(this);
	p_ui->Accept_buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText(tr("Принять"));
	p_ui->Accept_buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setText(tr("Отмена"));
	p_ui->Password_lineEdit->setText(QString(p_chPassword));
	p_chPasswordInt = p_chPassword;
}

// Деструктор.
Set_Password_Dialog::~Set_Password_Dialog()
{
	delete p_ui;
}

// Принято.
void Set_Password_Dialog::accept()
{
	CopyStrArray((char*)p_ui->Password_lineEdit->text().toStdString().c_str(), p_chPasswordInt, AUTH_PASSWORD_STR_LEN);
	this->done(DIALOGS_ACCEPT);
}

// Отменено.
void Set_Password_Dialog::reject()
{
	this->done(DIALOGS_REJECT);
}
