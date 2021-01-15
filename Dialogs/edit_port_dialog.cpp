//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include "edit_port_dialog.h"
#include "ui_edit_port_dialog.h"
#include "../Z-Hub/main-hub.h"
#include <QScrollBar>

//== МАКРОСЫ.
#define ROLE_PORT_NUMBER	0x100

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
QVector<Edit_Port_Dialog::PortInfo>* Edit_Port_Dialog::pv_PortsInt;
int* Edit_Port_Dialog::p_iNumberInt;
QTableWidgetItem* Edit_Port_Dialog::p_QTableWidgetItemSelected;

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога настройки порта.
// Конструктор.
Edit_Port_Dialog::Edit_Port_Dialog(char* p_chDialogCaption, QVector<PortInfo>* pv_Ports, int* p_iNumber, QWidget* p_parent) :
	QDialog(p_parent),
	p_ui(new Ui::Edit_Port_Dialog)
{
	int iRows = pv_Ports->count();
	//
	p_ui->setupUi(this);
	setWindowTitle(p_chDialogCaption);
	p_ui->spinBox->setValue(*p_iNumber);
	p_ui->spinBox->selectAll();
	pv_PortsInt = pv_Ports;
	p_iNumberInt = p_iNumber;
	p_ui->tableWidget_Pseudonyms->setRowCount(iRows);
	p_ui->tableWidget_Pseudonyms->horizontalHeader()->setStretchLastSection(true);
	for(int iF = 0; iF != iRows; iF++)
	{
		const PortInfo* p_PortInfo = &pv_PortsInt->at(iF);
		QString strItemText;
		QTableWidgetItem* p_QTableWidgetItem;
		//
		strItemText.setNum(p_PortInfo->ushiPortNumber);
		p_ui->tableWidget_Pseudonyms->setVerticalHeaderItem(iF, new QTableWidgetItem(strItemText));
		p_QTableWidgetItem = new QTableWidgetItem(p_PortInfo->strPseudonym);
		p_QTableWidgetItem->setData(ROLE_PORT_NUMBER, p_PortInfo->ushiPortNumber);
		p_ui->tableWidget_Pseudonyms->setItem(iF, 0, p_QTableWidgetItem);
		if(*p_iNumberInt == p_PortInfo->ushiPortNumber)
		{
			p_QTableWidgetItemSelected = p_QTableWidgetItem;
		}
	}
	p_ui->tableWidget_Pseudonyms->resizeColumnsToContents();
	if(p_QTableWidgetItemSelected)
	{
		oQTimer.singleShot(0, this, SLOT(UpdateTable()));
	}
}

// Деструктор.
Edit_Port_Dialog::~Edit_Port_Dialog()
{
	delete p_ui;
}

// Принято.
void Edit_Port_Dialog::accept()
{
	*p_iNumberInt = p_ui->spinBox->value();
	done(DIALOGS_ACCEPT);
}

// Отменено.
void Edit_Port_Dialog::reject()
{
	done(DIALOGS_REJECT);
}

// Исполнение таймера центровки таблицы.
void Edit_Port_Dialog::UpdateTable()
{
	p_ui->tableWidget_Pseudonyms->setItemSelected(p_QTableWidgetItemSelected, true);
	p_ui->tableWidget_Pseudonyms->scrollToItem(p_QTableWidgetItemSelected);
	p_ui->tableWidget_Pseudonyms->setCurrentCell(p_QTableWidgetItemSelected->row(), 0);
}

// Смена значения.
void Edit_Port_Dialog::on_spinBox_valueChanged(int arg1)
{
	for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
	{
		QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(iF, 0);
		if(p_QTableWidgetItem->data(ROLE_PORT_NUMBER) == arg1)
		{
			p_QTableWidgetItemSelected = p_QTableWidgetItem;
			oQTimer.singleShot(0, this, SLOT(UpdateTable()));
		}
		p_ui->tableWidget_Pseudonyms->setItemSelected(p_QTableWidgetItem, false);
	}
}
