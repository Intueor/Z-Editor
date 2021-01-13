#ifndef SET_PROPOSED_NUMBER_DIALOG_H
#define SET_PROPOSED_NUMBER_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class Set_Proposed_Number_Dialog;
}

//== КЛАССЫ.
/// Класс диалога установки предложенной строки.
class Set_Proposed_Number_Dialog : public QDialog
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit Set_Proposed_Number_Dialog(char* p_chDialogCaption, int iMin, int iMax, int iStep, int* p_iNumber, QWidget* p_parent = nullptr);
								///< \param[in] p_chDialogCaption Указатель на массив строки с именем.
								///< \param[in] iMin Минимальное значение.
								///< \param[in] iMax Максимальное значение.
								///< \param[in] iStep Шаг.
								///< \param[in,out] p_iNumber Указатель на переменную числа для начального отображения и последующего заполнения.
								///< \param[in] p_parent Указатель на родительский виджет.
	/// Деструктор.
	~Set_Proposed_Number_Dialog();

private slots:
	/// Принято.
	void accept();
	/// Отменено.
	void reject();

private:
	Ui::Set_Proposed_Number_Dialog* p_ui; ///< Указатель на интерфейс.
	static int* p_iNumberInt; ///< Внутренний указатель на число.
};

#endif // SET_PROPOSED_NUMBER_DIALOG_H
