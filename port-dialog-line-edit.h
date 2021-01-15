#ifndef PORTDIALOGLINEEDIT_H
#define PORTDIALOGLINEEDIT_H

//== ВКЛЮЧЕНИЯ.
#include <QLineEdit>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class PortDialogLineEdit;
}

//== КЛАССЫ.
/// Класс редактирования строки в диалоге порта.
class PortDialogLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	/// Конструктор.
	PortDialogLineEdit(QWidget* p_parent = nullptr);
								///< \param[in] p_parent Указатель на родителя.
};

#endif // PORTDIALOGLINEEDIT_H
