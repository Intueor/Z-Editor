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
	explicit Batch_Rename_Dialog(QWidget* p_parent = nullptr);
	/// Деструктор.
	~Batch_Rename_Dialog();

private:
	Ui::Batch_Rename_Dialog* p_ui; ///< Указатель на интерфейс.
};

#endif // BATCH_RENAME_DIALOG_H
