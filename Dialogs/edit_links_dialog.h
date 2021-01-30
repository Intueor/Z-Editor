#ifndef EDIT_LINKS_DIALOG_H
#define EDIT_LINKS_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class Edit_Links_Dialog;
}

//== КЛАССЫ.
/// Класс диалога редактора линков.
class Edit_Links_Dialog : public QDialog
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit Edit_Links_Dialog(QWidget* p_Parent = nullptr);
								///< \param[in] p_Parent Указатель на родительский виджет.
	/// Деструктор.
	~Edit_Links_Dialog();

private:
	Ui::Edit_Links_Dialog* p_ui;
};

#endif // EDIT_LINKS_DIALOG_H
