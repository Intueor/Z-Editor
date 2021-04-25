#ifndef CREATE_LINK_DIALOG_H
#define CREATE_LINK_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>
#include <QLabel>
#include <QListWidgetItem>
#include <QTimer>
#include "z-editor-defs.h"

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
	explicit Create_Link_Dialog(unsigned long long ullIDInitial = OVERMAX_ID, QWidget* p_Parent = nullptr);
												///< \param[in] ullIDInitial ИД начального элемента или OVERMAX_ID.
												///< \param[in] p_Parent Указатель на родительский виджет.
	/// Деструктор.
	~Create_Link_Dialog();

private slots:
	/// Принято.
	void accept();
	/// Отменено.
	void reject();
	/// При нажатии выбора порта-источника.
	void on_pushButton_Src_Port_clicked();
	/// При нажатии выбора порта-приёмника.
	void on_pushButton_Dst_Port_clicked();
	/// При смене строки в листе элементов-источников.
	void on_listWidget_Src_currentRowChanged(int iCurrentRow);
												///< \param[in] iCurrentRow Текущая строка.
	/// При смене строки в листе элементов-приёмников.
	void on_listWidget_Dst_currentRowChanged(int iCurrentRow);
												///< \param[in] iCurrentRow Текущая строка.
	/// При нажатии на кнопку "Принять".
	void on_pushButton_Ok_clicked();
	/// При выходе из редактирования поиска элемента-источника.
	void on_lineEdit_SS_editingFinished();
	/// При нажатии Enter в поиске элемента-источника.
	void on_lineEdit_SS_returnPressed();
	/// При выходе из редактирования поиска элемента-приёмника.
	void on_lineEdit_SD_editingFinished();
	/// При нажатии Enter в поиске элемента-приёмника.
	void on_lineEdit_SD_returnPressed();
	/// При изменении в тексте в поиске элемента-источника.
	void on_lineEdit_SS_textEdited(const QString &arg1);
												///< \param[in] arg1 Ссылка на строку поиска.
	/// При изменении в тексте в поиске элемента-приёмника.
	void on_lineEdit_SD_textEdited(const QString &arg1);
												///< \param[in] arg1 Ссылка на строку поиска.

private:
	/// Сброс портов.
	void ResetPorts();
	/// Заполнение строки порта.
	static void FillPortLabel(QLabel* p_QLabel, int iPort);
												///< \param[in] QLabel Указатель на строку порта.
												///< \param[in] iPort Номер порта.
	/// Установка строки в листе по тексту из поиска.
	void SetRowBySearch(const QString& arg1, QListWidget* p_listWidget);
												///< \param[in] arg1 Ссылка на строку поиска.
												///< \param[in] p_listWidget Указатель на виджет листа элементов.

private:
	Ui::Create_Link_Dialog* p_ui; ///< Указатель на интерфейс.
	static int iElements; ///< Кол-во элементов.
	static int iSelectedSrcPort; ///< Номер выбранного порта источника.
	static int iSelectedDstPort; ///< Номер выбранного порта цели.
	QTimer oQTimer; ///< Таймер для разрешения Cancel по умолчанию при выходе из редактирования строки поиска.
};

#endif // CREATE_LINK_DIALOG_H
