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

private:
	Ui::Edit_Links_Dialog* p_ui; ///< Указатель на интерфейс.
	static QVector<GraphicsLinkItem*> vp_GraphicsLinkItems; ///< Список линков для обработки.
};

#endif // EDIT_LINKS_DIALOG_H
