//== ВКЛЮЧЕНИЯ.
#include "create_link_dialog.h"
#include "ui_create_link_dialog.h"
#include "schematic-window.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога создания линка.
// Конструктор.
Create_Link_Dialog::Create_Link_Dialog(QWidget* p_Parent) : QDialog(p_Parent), p_ui(new Ui::Create_Link_Dialog)
{
	p_ui->setupUi(this);
	for(int iF = 0; iF != SchematicWindow::vp_Elements.count(); iF++)
	{
		GraphicsElementItem* p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
		//
		p_ui->listWidget_Src->addItem(p_GraphicsElementItem->oPSchElementBaseInt.m_chName);
		p_ui->listWidget_Dst->addItem(p_GraphicsElementItem->oPSchElementBaseInt.m_chName);
	}
	p_ui->listWidget_Src->setStyleSheet("QListWidget::item:selected {background-color: palette(Highlight);}");
	p_ui->listWidget_Dst->setStyleSheet("QListWidget::item:selected {background-color: palette(Highlight);}");
}

// Деструктор.
Create_Link_Dialog::~Create_Link_Dialog()
{
	delete p_ui;
}

// При нажатии выбора порта-источника.
void Create_Link_Dialog::on_pushButton_Src_Port_clicked()
{

}

// При нажатии выбора порта-приёмника.
void Create_Link_Dialog::on_pushButton_Dst_Port_clicked()
{

}
