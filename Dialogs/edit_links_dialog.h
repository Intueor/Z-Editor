#ifndef EDIT_LINKS_DIALOG_H
#define EDIT_LINKS_DIALOG_H

//== ВКЛЮЧЕНИЯ.
#include <QDialog>
#include <QtWidgets/QTableWidget>
#include "schematic-window.h"

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
	class Edit_Links_Dialog;
}

//== КЛАССЫ.
/// Класс диалога редактора линков.
class Edit_Links_Dialog : public QDialog
{
	Q_OBJECT

public:
	/// Рекурсивное добавление объектов группы в таблицу.
	static void AddGroupObjectsToTableRecursively(GraphicsGroupItem* p_GraphicsGroupItem);
								///< \param[in] p_GraphicsGroupItem Указатель на группу.
	/// Конструктор.
	explicit Edit_Links_Dialog(QWidget* p_Parent = nullptr);
								///< \param[in] p_Parent Указатель на родительский виджет.
	/// Деструктор.
	~Edit_Links_Dialog();

private slots:
	/// При активации ячейки.
	void on_tableWidget_Links_cellActivated(int iRow, int iColumn);
								///< \param[in] iRow Строка.
								///< \param[in] iColumn Колонка.
private:
	/// Конструирование строк для таблицы.
	static void ConstructTableRow(GraphicsLinkItem* p_GraphicsLinkItem, QString& a_strItemLeft, QString& a_strItemRight);
								///< \param[in] p_GraphicsLinkItem Указатель на линк.
								///< \param[out] a_strItemLeft Ссылка на строку для заполнения значением левого элемента.
								///< \param[out] a_strItemRight Ссылка на строку для заполнения значением правого элемента.
private:
	Ui::Edit_Links_Dialog* p_ui; ///< Указатель на интерфейс.
	static QVector<GraphicsLinkItem*> vp_GraphicsLinkItems; ///< Список линков для обработки.
	static bool bInitialized; ///< Признак инициализированности диалога.
};

#endif // EDIT_LINKS_DIALOG_H
