#ifndef BATCH_RENAME_DIALOG_H
#define BATCH_RENAME_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class Batch_Rename_Dialog;
}

//== КЛАССЫ.
/// Класс диалога пакетного переименования.
class Batch_Rename_Dialog : public QDialog
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit Batch_Rename_Dialog(char* p_chText, unsigned char uchMaxLength, QWidget* p_parent = nullptr);
								///< \param[in,out] p_chText Указатель на массив строки для начального отображения и последующего заполнения.
								///< \param[in] uchMaxLength Максимальная длина строки.
								///< \param[in] p_parent Указатель на родительский виджет.
	/// Деструктор.
	~Batch_Rename_Dialog();

private slots:
	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

public:
	Ui::Batch_Rename_Dialog* p_ui; ///< Указатель на интерфейс.

private:
	static char* p_chTextInt; ///< Внутренний указатель на текст.
	static unsigned char uchMaxLengthInt; ///< Внутренняя переменная макс. длины строки.
};

#endif // BATCH_RENAME_DIALOG_H
