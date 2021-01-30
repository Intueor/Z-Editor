//== ВКЛЮЧЕНИЯ.
#include "edit_links_dialog.h"
#include "ui_edit_links_dialog.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога редактора линков.
// Конструктор.
Edit_Links_Dialog::Edit_Links_Dialog(QWidget* p_Parent) :
	QDialog(p_Parent),
	p_ui(new Ui::Edit_Links_Dialog)
{
	p_ui->setupUi(this);
}

// Деструктор.
Edit_Links_Dialog::~Edit_Links_Dialog()
{
	delete p_ui;
}
