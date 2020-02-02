#ifndef SET_PASSWORD_DIALOG_H
#define SET_PASSWORD_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class Set_Password_Dialog;
}

//== КЛАССЫ.
/// Класс диалога установки пароля.
class Set_Password_Dialog : public QDialog
{
	Q_OBJECT

public:
	/// Конструктор.
    explicit Set_Password_Dialog(char* p_chPassword, QWidget* p_parent = nullptr);
									///< \param[in,out] p_chPassword Указатель на массив строки с паролем.
									///< \param[in] p_parent Указатель на родительский виджет.
	/// Деструктор.
	~Set_Password_Dialog();

private slots:
	/// Принято.
	void accept();
	/// Отменено.
	void reject();

private:
	Ui::Set_Password_Dialog *p_ui; //< Указатель на UI.
	static char* p_chPasswordInt; //< Внутренний указатель на массив строки пароля.
};

#endif // SET_PASSWORD_DIALOG_H
