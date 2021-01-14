#ifndef EDIT_PORT_DIALOG_H
#define EDIT_PORT_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class Edit_Port_Dialog;
}

//== КЛАССЫ.
/// Класс диалога настройки порта.
class Edit_Port_Dialog : public QDialog
{
	Q_OBJECT

public:
	struct PortInfo
	{
		unsigned short int ushiPortNumber;
		QString strPseudonym;
	};

public:
	/// Конструктор.
	explicit Edit_Port_Dialog(char* p_chDialogCaption, QVector<PortInfo>* pv_Ports, int* p_iNumber, QWidget* p_parent = nullptr);
								///< \param[in] p_chDialogCaption Указатель на массив строки с именем.
								///< \param[in] p_iNumber Указатель на переменную числа для начального отображения и последующего заполнения..
								///< \param[in] p_parent Указатель на родительский виджет.
	/// Деструктор.
	~Edit_Port_Dialog();

private slots:
	/// Принято.
	void accept();
	/// Отменено.
	void reject();

private:
	Ui::Edit_Port_Dialog* p_ui; ///< Указатель на интерфейс.
	static QVector<PortInfo>* pv_PortsInt; ///< Указатель на список информации по портам.
	static int* p_iNumberInt; ///< Внутренний указатель на число.
};

#endif // EDIT_PORT_DIALOG_H
