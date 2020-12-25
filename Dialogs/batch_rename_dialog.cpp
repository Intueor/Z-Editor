//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include "batch_rename_dialog.h"
#include "ui_batch_rename_dialog.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога пакетного переименования.
// Конструктор.
Batch_Rename_Dialog::Batch_Rename_Dialog(QWidget* p_parent) :
	QDialog(p_parent),
	p_ui(new Ui::Batch_Rename_Dialog)
{
	p_ui->setupUi(this);
	p_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setText("Принять");
	p_ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)->setText("Отмена");
	p_ui->BaseName_lineEdit->selectAll();
}

// Деструктор.
Batch_Rename_Dialog::~Batch_Rename_Dialog()
{
	delete p_ui;
}
