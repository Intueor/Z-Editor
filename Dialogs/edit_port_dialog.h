#ifndef EDIT_PORT_DIALOG_H
#define EDIT_PORT_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>
#include <QSpinBox>
#include <QTimer>
#include <QTableWidgetItem>

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
	/// Исполнение таймера центровки таблицы.
	void UpdateTable();
	/// Смена значения.
	void on_spinBox_valueChanged(int arg1);
								///< \param[in] arg1 Число.
	/// Смена ячейки псевдонима.//
	void on_tableWidget_Pseudonyms_currentCellChanged(int currentRow, int currentColumn, int, int);
								///< \param[in] currentRow Строка.
								///< \param[in] currentRow Колонка.
	/// Смена значения поисковой строки.
	void on_Safe_Searching_Line_Edit_textEdited(const QString &arg1);
								///< \param[in] arg1 Ссылка на строку.
	/// Удаление псевдонима.
	void on_SwitchAnotherPushButton_Delete_Pseudonym_clicked();
	/// Обработка нажатия на Enter.
	void on_Safe_Searching_Line_Edit_returnPressed();
	/// Обработка ухода из редактора.
	void on_Safe_Searching_Line_Edit_editingFinished();
	/// Создание псевдонима.
	void on_pushButton_Set_New_Pseudonym_clicked();
	/// Обработка изменённой ячейки.
	void on_tableWidget_Pseudonyms_itemChanged(QTableWidgetItem* p_Item);
								//< \param[in] p_Item Указатель на ячейку.
	/// При выборе контекстного меню в таблице псевдонимов.
	void on_tableWidget_Pseudonyms_customContextMenuRequested(const QPoint& a_Pos);
								//< \param[in] a_Pos Ссылка координаты выбранного объекта.

private:
	/// Уход из редактирования поиска.
	void LeaveSearch();

private:
	Ui::Edit_Port_Dialog* p_ui; ///< Указатель на интерфейс.
	static QVector<PortInfo>* pv_PortsInt; ///< Указатель на список информации по портам.
	static int* p_iNumberInt; ///< Внутренний указатель на число.
	static QTableWidgetItem* p_QTableWidgetItemSelected; ///< Выбранная ячейка для установки в таблице.
	QTimer oQTimer; ///< Таймер для установки текущей ячейки в таблице (уродство).
	static bool bBlockSpinBoxSync; ///< При true - блокировка обновления числа в спинбоксе по выбранной ячейке таблицы.
	static bool bFromConstructor; ///< Признак захода в смену строки из конструктора.
	static QList<QPushButton*> vp_QPushButtondForDisabling; ///< Список указателей на отключаемые кнопки.
	static QList<QPushButton*> vp_QPushButtondSwitchable; ///< Список указателей на переключаемые кнопки.
	static QVector<PortInfo> v_Ports; ///< Список портов для внутреннего пользования.
};

#endif // EDIT_PORT_DIALOG_H
