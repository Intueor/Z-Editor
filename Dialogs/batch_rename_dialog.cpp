//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include "batch_rename_dialog.h"
#include "ui_batch_rename_dialog.h"
#include "../Z-Hub/main-hub.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
unsigned char Batch_Rename_Dialog::uchMaxLengthInt;
char* Batch_Rename_Dialog::p_chTextInt = nullptr;

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога пакетного переименования.
// Конструктор.
Batch_Rename_Dialog::Batch_Rename_Dialog(char* p_chText, unsigned char uchMaxLength, QWidget* p_parent) :
	QDialog(p_parent),
	p_ui(new Ui::Batch_Rename_Dialog)
{
	p_ui->setupUi(this);
	p_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Принять");
	p_ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setText("Отмена");
	p_ui->BaseName_lineEdit->setText(p_chText);
	p_ui->BaseName_lineEdit->selectAll();
	p_chTextInt = p_chText;
	uchMaxLengthInt = uchMaxLength;
}

// Деструктор.
Batch_Rename_Dialog::~Batch_Rename_Dialog()
{
	delete p_ui;
}

void Batch_Rename_Dialog::on_buttonBox_accepted()
{
	CopyStrArray((char*)p_ui->BaseName_lineEdit->text().toStdString().c_str(), p_chTextInt, uchMaxLengthInt);
	done(DIALOGS_ACCEPT);
}

void Batch_Rename_Dialog::on_buttonBox_rejected()
{
	done(DIALOGS_REJECT);
}
