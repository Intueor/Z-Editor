#ifndef CREATE_LINK_DIALOG_H
#define CREATE_LINK_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class Create_Link_Dialog;
}

//== КЛАССЫ.
/// Класс диалога создания линка.
class Create_Link_Dialog : public QDialog
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit Create_Link_Dialog(QWidget* p_Parent = nullptr);
												///< \param[in] p_Parent Указатель на родительский виджет.
	/// Деструктор.
	~Create_Link_Dialog();

private slots:
	/// При нажатии выбора порта-источника.
	void on_pushButton_Src_Port_clicked();
	/// При нажатии выбора порта-приёмника.
	void on_pushButton_Dst_Port_clicked();

private:
	Ui::Create_Link_Dialog* p_ui; ///< Указатель на интерфейс.
};

#endif // CREATE_LINK_DIALOG_H
