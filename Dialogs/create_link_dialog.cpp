//== ВКЛЮЧЕНИЯ.
#include "create_link_dialog.h"
#include "ui_create_link_dialog.h"
#include "schematic-window.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
int Create_Link_Dialog::iSelectedSrcPort;
int Create_Link_Dialog::iSelectedDstPort;
int Create_Link_Dialog::iElements;

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога создания линка.
// Конструктор.
Create_Link_Dialog::Create_Link_Dialog(QWidget* p_Parent) : QDialog(p_Parent), p_ui(new Ui::Create_Link_Dialog)
{
	p_ui->setupUi(this);
	iElements = SchematicWindow::vp_Elements.count();
	for(int iF = 0; iF != iElements; iF++)
	{
		GraphicsElementItem* p_GraphicsElementItem = SchematicWindow::vp_Elements.at(iF);
		//
		p_ui->listWidget_Src->addItem(p_GraphicsElementItem->oPSchElementBaseInt.m_chName);
		p_ui->listWidget_Dst->addItem(p_GraphicsElementItem->oPSchElementBaseInt.m_chName);
	}
	p_ui->listWidget_Src->setStyleSheet("QListWidget::item:selected {background-color: palette(Highlight);}");
	p_ui->listWidget_Dst->setStyleSheet("QListWidget::item:selected {background-color: palette(Highlight);}");
	p_ui->listWidget_Src->setCurrentRow(0);
	p_ui->listWidget_Dst->setCurrentRow(1);
	ResetPorts();
}

// Деструктор.
Create_Link_Dialog::~Create_Link_Dialog()
{
	delete p_ui;
}

// Сброс портов.
void Create_Link_Dialog::ResetPorts()
{
	iSelectedSrcPort = PORT_NOT_SELECTED;
	iSelectedDstPort = PORT_NOT_SELECTED;
	p_ui->label_Src_Port->setText(m_chPortNotSelected);
	p_ui->label_Dst_Port->setText(m_chPortNotSelected);
	p_ui->pushButton_Ok->setDisabled(true);
}

// Заполнение строки порта.
void Create_Link_Dialog::FillPortLabel(QLabel* p_QLabel, int iPort)
{
	QString strPortNumber;
	//
	for(int iF = 0; iF != SchematicWindow::v_PSchPseudonyms.count(); iF++)
	{
		const PSchPseudonym* p_PSchPseudonym = &SchematicWindow::v_PSchPseudonyms.at(iF);
		//
		if(p_PSchPseudonym->ushiPort == iPort)
		{
			p_QLabel->setText(p_PSchPseudonym->m_chName);
			return;
		}
	}
	p_QLabel->setText(strPortNumber.setNum(iPort));
}

// При нажатии выбора порта-источника.
void Create_Link_Dialog::on_pushButton_Src_Port_clicked()
{
	QVector<Edit_Port_Dialog::PortInfo> v_PortsInfo;
	Edit_Port_Dialog* p_Edit_Port_Dialog = nullptr;
	bool bAccepted = false;
	//
	p_Edit_Port_Dialog = new Edit_Port_Dialog(const_cast<char*>(m_chMenuPortSrc), v_PortsInfo, &iSelectedSrcPort);
gS:	if(p_Edit_Port_Dialog->exec() == DIALOGS_ACCEPT)
	{
		bAccepted = true;
		if(SchematicView::IsLinkPresent(SchematicWindow::vp_Elements.at(
											p_ui->listWidget_Src->currentRow())->
										oPSchElementBaseInt.oPSchElementVars.ullIDInt,
										iSelectedSrcPort,
										SchematicWindow::vp_Elements.at(p_ui->listWidget_Dst->currentRow())->
										oPSchElementBaseInt.oPSchElementVars.ullIDInt,
										iSelectedDstPort))
		{
			bAccepted = false;
			goto gS;
		}
	}
	if(bAccepted) SchematicView::ActualizePseudonyms(v_PortsInfo);
	FillPortLabel(p_ui->label_Src_Port, iSelectedSrcPort);
}

// При нажатии выбора порта-приёмника.
void Create_Link_Dialog::on_pushButton_Dst_Port_clicked()
{
	QVector<Edit_Port_Dialog::PortInfo> v_PortsInfo;
	Edit_Port_Dialog* p_Edit_Port_Dialog = nullptr;
	bool bAccepted = false;
	//
	p_Edit_Port_Dialog = new Edit_Port_Dialog(const_cast<char*>(m_chMenuPortSrc), v_PortsInfo, &iSelectedDstPort);
gD:	if(p_Edit_Port_Dialog->exec() == DIALOGS_ACCEPT)
	{
		bAccepted = true;
		if(SchematicView::IsLinkPresent(SchematicWindow::vp_Elements.at(
											p_ui->listWidget_Src->currentRow())->
										oPSchElementBaseInt.oPSchElementVars.ullIDInt,
										iSelectedDstPort,
										SchematicWindow::vp_Elements.at(p_ui->listWidget_Dst->currentRow())->
										oPSchElementBaseInt.oPSchElementVars.ullIDInt,
										iSelectedDstPort))
		{
			bAccepted = false;
			goto gD;
		}
	}
	if(bAccepted) SchematicView::ActualizePseudonyms(v_PortsInfo);
	FillPortLabel(p_ui->label_Dst_Port, iSelectedDstPort);
}

// При смене строки в листе элементов-источников.
void Create_Link_Dialog::on_listWidget_Src_currentRowChanged(int iCurrentRow)
{
	int iDstCurrentRow = p_ui->listWidget_Dst->currentRow();
	//
	ResetPorts();
	if(iCurrentRow == iDstCurrentRow)
	{
		if(iDstCurrentRow != (iElements - 1))
		{
			p_ui->listWidget_Dst->setCurrentRow(iDstCurrentRow + 1);
		}
		else
		{
			p_ui->listWidget_Dst->setCurrentRow(iDstCurrentRow - 1);
		}
	}
}
// При смене строки в листе элементов-приёмников.
void Create_Link_Dialog::on_listWidget_Dst_currentRowChanged(int iCurrentRow)
{
	int iSrcCurrentRow = p_ui->listWidget_Src->currentRow();
	//
	ResetPorts();
	if(iCurrentRow == iSrcCurrentRow)
	{
		if(iSrcCurrentRow != (iElements - 1))
		{
			p_ui->listWidget_Src->setCurrentRow(iSrcCurrentRow + 1);
		}
		else
		{
			p_ui->listWidget_Src->setCurrentRow(iSrcCurrentRow - 1);
		}
	}
}


