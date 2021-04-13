#ifndef CREATE_LINK_DIALOG_H
#define CREATE_LINK_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>
#include <QLabel>

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
	/// Заполнение строки порта.
	static void FillPortLabel(QLabel* p_QLabel, int iPort);
												///< \param[in] QLabel Указатель на строку порта.
												///< \param[in] iPort Номер порта.

private:
	Ui::Create_Link_Dialog* p_ui; ///< Указатель на интерфейс.
	static int iSelectedSrcPort; ///< Номер выбранного порта источника.
	static int iSelectedDstPort; ///< Номер выбранного порта цели.
};

#endif // CREATE_LINK_DIALOG_H
