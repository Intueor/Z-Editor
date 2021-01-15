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
bool Edit_Port_Dialog::bFromDelete = false;

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
	if(p_QTableWidgetItemSelected)
	{
		oQTimer.singleShot(0, this, SLOT(UpdateTable()));
	}
	else p_ui->pushButton_Delete_Pseudonym->setEnabled(false);
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
	p_ui->pushButton_Delete_Pseudonym->setEnabled(true);
}

// Возвращение фокуса по умолчанию на кнопку "Принять".
void Edit_Port_Dialog::ReturnDefaultToAccept()
{
	p_ui->pushButton_Accept->setDefault(true);
}

// Смена значения.
void Edit_Port_Dialog::on_spinBox_valueChanged(int arg1)
{
	bool bFoundPseudonym = false;
	for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
	{
		QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(iF, 0);
		if(p_QTableWidgetItem->data(ROLE_PORT_NUMBER).toInt() == arg1)
		{
			p_QTableWidgetItemSelected = p_QTableWidgetItem;
			oQTimer.singleShot(0, this, SLOT(UpdateTable()));
			bFoundPseudonym = true;
		}
		else p_ui->tableWidget_Pseudonyms->setItemSelected(p_QTableWidgetItem, false);
	}
	if(!bFoundPseudonym) p_ui->pushButton_Delete_Pseudonym->setEnabled(false);
}

// Смена ячейки псевдонима.
void Edit_Port_Dialog::on_tableWidget_Pseudonyms_currentCellChanged(int currentRow, int currentColumn, int, int)
{
	if(!bFromDelete)
	{
		QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(currentRow, currentColumn);
		//
		p_ui->spinBox->setValue(p_QTableWidgetItem->data(ROLE_PORT_NUMBER).toInt());
	}
	else bFromDelete = false;
}

// Смена значения поисковой строки.
void Edit_Port_Dialog::on_lineEdit_Search_textEdited(const QString &arg1)
{
	if(!arg1.isEmpty())
	{
		QList<QTableWidgetItem*> vp_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->findItems(arg1, Qt::MatchStartsWith | Qt::MatchCaseSensitive);
		//
		if(!vp_QTableWidgetItem.isEmpty())
		{
			p_QTableWidgetItemSelected = vp_QTableWidgetItem.first();
			oQTimer.singleShot(0, this, SLOT(UpdateTable()));
		}
	}
}

// Удаление псевдонима.
void Edit_Port_Dialog::on_pushButton_Delete_Pseudonym_clicked()
{
	int iRow = p_ui->tableWidget_Pseudonyms->currentRow();
	//
	if(iRow != -1)
	{
		bFromDelete = true;
		p_ui->tableWidget_Pseudonyms->removeRow(iRow);
		for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
		{
			p_ui->tableWidget_Pseudonyms->item(iF, 0)->setSelected(false);
		}
		p_ui->pushButton_Delete_Pseudonym->setEnabled(false);
	}
}

// Смена позиции курсора в строке (смотрим для снятия фокуса с кнопок - чтобы случайный Enter не сработал).
void Edit_Port_Dialog::on_lineEdit_Search_cursorPositionChanged(int, int)
{
	p_ui->pushButton_Accept->setDefault(false);
}

// Обработка нажатия на Enter.
void Edit_Port_Dialog::on_lineEdit_Search_returnPressed()
{
	p_ui->tableWidget_Pseudonyms->setFocus();
	oQTimer.singleShot(0, this, SLOT(ReturnDefaultToAccept()));
	p_ui->lineEdit_Search->deselect();
}

// Обработка ухода со строки.
void Edit_Port_Dialog::on_lineEdit_Search_editingFinished()
{
	oQTimer.singleShot(0, this, SLOT(ReturnDefaultToAccept()));
	p_ui->lineEdit_Search->deselect();
}

// Обработка смены выбранности.
void Edit_Port_Dialog::on_lineEdit_Search_selectionChanged()
{
	p_ui->pushButton_Accept->setDefault(false);
}
