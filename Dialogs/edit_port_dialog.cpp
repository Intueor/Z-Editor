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
bool Edit_Port_Dialog::bBlockSpinBoxSync = false;
bool Edit_Port_Dialog::bFromConstructor = false;
QList<QPushButton*> Edit_Port_Dialog::vp_QPushButtondForDisabling;
QList<QPushButton*> Edit_Port_Dialog::vp_QPushButtondSwitchable;

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
	p_ui->Safe_Searching_Line_Edit->p_QPushButtonForNotDefault = p_ui->pushButton_Accept;
	vp_QPushButtondForDisabling.append(p_ui->SwitchAnotherPushButton_Delete_Pseudonym);
	vp_QPushButtondForDisabling.append(p_ui->pushButton_Set_New_Pseudonym);
	p_ui->Safe_Searching_Line_Edit->plp_QPushButtonsForDisable = &vp_QPushButtondForDisabling;
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
		p_QTableWidgetItem = new QTableWidgetItem(p_PortInfo->strPseudonym);
		p_QTableWidgetItem->setData(ROLE_PORT_NUMBER, p_PortInfo->ushiPortNumber);
		p_ui->tableWidget_Pseudonyms->setItem(iF, 0, p_QTableWidgetItem);
		p_ui->tableWidget_Pseudonyms->setVerticalHeaderItem(iF, new QTableWidgetItem(strItemText));
		if(*p_iNumberInt == p_PortInfo->ushiPortNumber)
		{
			p_QTableWidgetItemSelected = p_QTableWidgetItem;
		}
	}
	vp_QPushButtondSwitchable.append(p_ui->pushButton_Set_New_Pseudonym);
	p_ui->SwitchAnotherPushButton_Delete_Pseudonym->pvp_QPushButtonsForSwitching = &vp_QPushButtondSwitchable;
	if(p_QTableWidgetItemSelected)
	{
		bFromConstructor = true;
		oQTimer.singleShot(0, this, SLOT(UpdateTable()));
	}
	else p_ui->SwitchAnotherPushButton_Delete_Pseudonym->setEnabled(false);
	p_ui->SwitchAnotherPushButton_Delete_Pseudonym->bReady = true;
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
	pv_PortsInt->clear();
	for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
	{
		QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(iF, 0);
		PortInfo oPortInfo;
		//
		oPortInfo.ushiPortNumber = p_QTableWidgetItem->data(ROLE_PORT_NUMBER).toInt();
		oPortInfo.strPseudonym = p_QTableWidgetItem->text();
		pv_PortsInt->append(oPortInfo);
	}
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
	if(p_QTableWidgetItemSelected)
	{
		p_ui->tableWidget_Pseudonyms->setItemSelected(p_QTableWidgetItemSelected, true);
		p_ui->tableWidget_Pseudonyms->scrollToItem(p_QTableWidgetItemSelected);
		p_ui->tableWidget_Pseudonyms->setCurrentCell(p_QTableWidgetItemSelected->row(), 0);
		if(bFromConstructor)
		{
			p_ui->tableWidget_Pseudonyms->setFocus();
			bFromConstructor = false;
			p_ui->SwitchAnotherPushButton_Delete_Pseudonym->setEnabled(true);
		}
		else
		{
			p_ui->spinBox->setValue(p_QTableWidgetItemSelected->data(ROLE_PORT_NUMBER).toInt());
			if(!p_ui->Safe_Searching_Line_Edit->IsUserInside())
			{
				p_ui->SwitchAnotherPushButton_Delete_Pseudonym->setEnabled(true);
			}
		}
	}
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
	if(!bFoundPseudonym)
	{
		p_ui->SwitchAnotherPushButton_Delete_Pseudonym->setEnabled(false);
		p_ui->pushButton_Set_New_Pseudonym->setEnabled(true);
		if(p_ui->Safe_Searching_Line_Edit->IsUserInside())
		{
			p_ui->Safe_Searching_Line_Edit->SetRecordedButtonEnabledStatus(p_ui->SwitchAnotherPushButton_Delete_Pseudonym, false);
			p_ui->Safe_Searching_Line_Edit->SetRecordedButtonEnabledStatus(p_ui->pushButton_Set_New_Pseudonym, true);
		}
	}
}

// Смена ячейки псевдонима.
void Edit_Port_Dialog::on_tableWidget_Pseudonyms_currentCellChanged(int currentRow, int currentColumn, int, int)
{
	if(currentRow != -1)
	{
		if(!bBlockSpinBoxSync)
		{
			QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(currentRow, currentColumn);
			//
			p_ui->spinBox->setValue(p_QTableWidgetItem->data(ROLE_PORT_NUMBER).toInt());
			if(p_ui->Safe_Searching_Line_Edit->IsUserInside())
			{
				p_ui->Safe_Searching_Line_Edit->SetRecordedButtonEnabledStatus(p_ui->pushButton_Set_New_Pseudonym, false);
				p_ui->Safe_Searching_Line_Edit->SetRecordedButtonEnabledStatus(p_ui->SwitchAnotherPushButton_Delete_Pseudonym, true);
			}
		}
		else bBlockSpinBoxSync = false;
	}
}

// Смена значения поисковой строки.
void Edit_Port_Dialog::on_Safe_Searching_Line_Edit_textEdited(const QString &arg1)
{
	if(!arg1.isEmpty())
	{
		for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
		{
			QTableWidgetItem* p_QTableWidgetItemCurrent = p_ui->tableWidget_Pseudonyms->item(iF, 0);
			QString strCurrentText = p_QTableWidgetItemCurrent->text();
			bool bM = true;
			//
			for(int iC = 0; iC != arg1.count(); iC++)
			{
				if((iC > (strCurrentText.count() - 1)) || (arg1.at(iC) != strCurrentText.at(iC)))
				{
					bM = false;
					break;
				}
			}
			if(bM)
			{
				p_QTableWidgetItemSelected = p_QTableWidgetItemCurrent;
				oQTimer.singleShot(0, this, SLOT(UpdateTable()));
				return;
			}
		}
	}
}

// Уход из редактирования поиска.
void Edit_Port_Dialog::LeaveSearch()
{
	int iSpinValue = p_ui->spinBox->value();
	//

	for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
	{
		QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(iF, 0);
		if(p_QTableWidgetItem->data(ROLE_PORT_NUMBER).toInt() == iSpinValue)
		{
			p_ui->SwitchAnotherPushButton_Delete_Pseudonym->setEnabled(true);
			return;
		}
	}
	bBlockSpinBoxSync = true;
	p_ui->tableWidget_Pseudonyms->setCurrentCell(-1, -1);
	p_ui->SwitchAnotherPushButton_Delete_Pseudonym->setEnabled(false);
}

// Удаление псевдонима.
void Edit_Port_Dialog::on_SwitchAnotherPushButton_Delete_Pseudonym_clicked()
{
	int iRow = p_ui->tableWidget_Pseudonyms->currentRow();
	//
	if(iRow != -1)
	{
		bBlockSpinBoxSync = true;
		p_ui->tableWidget_Pseudonyms->removeRow(iRow);
		for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
		{
			p_ui->tableWidget_Pseudonyms->item(iF, 0)->setSelected(false);
		}
		p_ui->SwitchAnotherPushButton_Delete_Pseudonym->setEnabled(false);
		p_ui->tableWidget_Pseudonyms->setCurrentCell(-1, -1);
	}
}

// Обработка нажатия на Enter.
void Edit_Port_Dialog::on_Safe_Searching_Line_Edit_returnPressed()
{
	p_ui->tableWidget_Pseudonyms->setFocus();
	LeaveSearch();
}

// Обработка ухода из редактора.
void Edit_Port_Dialog::on_Safe_Searching_Line_Edit_editingFinished()
{
	LeaveSearch();
}

// Создание псевдонима.
void Edit_Port_Dialog::on_pushButton_Set_New_Pseudonym_clicked()
{
	int iN = p_ui->spinBox->value();
	//
	for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
	{
		QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(iF, 0);
		int iPort = p_QTableWidgetItem->data(ROLE_PORT_NUMBER).toInt();
		//
		if(iPort > iN)
		{
			QString strItemText;
			//
			strItemText.setNum(iN);
			p_ui->tableWidget_Pseudonyms->insertRow(iF);
			p_QTableWidgetItem = new QTableWidgetItem("");
			p_QTableWidgetItem->setData(ROLE_PORT_NUMBER, iN);
			p_ui->tableWidget_Pseudonyms->setItem(iF, 0, p_QTableWidgetItem);
			p_ui->tableWidget_Pseudonyms->setVerticalHeaderItem(iF, new QTableWidgetItem(strItemText));
			p_ui->tableWidget_Pseudonyms->editItem(p_QTableWidgetItem);
			p_QTableWidgetItemSelected = p_QTableWidgetItem;
			UpdateTable();
			break;
		}
	}
}

// Обработка изменённой ячейки.
void Edit_Port_Dialog::on_tableWidget_Pseudonyms_itemChanged(QTableWidgetItem* p_Item)
{
	QString strNum;
	//
	if(p_Item->text().isEmpty())
	{
		strNum.setNum(GenerateID());
		p_Item->setText("Псевдоним [" + strNum + "]");
	}
	else
	{
		unsigned char uchMC = 0;
		//
		for(int iF = 0; iF != p_ui->tableWidget_Pseudonyms->rowCount(); iF++)
		{
			QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Pseudonyms->item(iF, 0);
			//
			if(p_QTableWidgetItem)
			{
				if(p_QTableWidgetItem->text() == p_Item->text())
				{
					uchMC++;
					if(uchMC == 2)
					{
						strNum.setNum(GenerateID());
						p_Item->setText(p_Item->text() + " [" + strNum + "]");
						break;
					}
				}
			}
		}
	}
}
