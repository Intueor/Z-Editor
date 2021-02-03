//== ВКЛЮЧЕНИЯ.
#include <QScrollBar>
#include "edit_links_dialog.h"
#include "ui_edit_links_dialog.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
QVector<GraphicsLinkItem*> Edit_Links_Dialog::vp_GraphicsLinkItems;

//== ФУНКЦИИ КЛАССОВ.
//== Класс диалога редактора линков.
// Рекурсивное добавление объектов группы в таблицу.
void Edit_Links_Dialog::AddGroupObjectsToTableRecursively(GraphicsGroupItem* p_GraphicsGroupItem)
{
	for(int iE = 0; iE != p_GraphicsGroupItem->vp_ConnectedElements.count(); iE++)
	{
		GraphicsElementItem* p_GraphicsElementItem = p_GraphicsGroupItem->vp_ConnectedElements.at(iE);
		//
		for(int iL = 0; iL != SchematicWindow::vp_Links.count(); iL++)
		{
			GraphicsLinkItem* p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iL);
			//
			if((p_GraphicsLinkItem->p_GraphicsElementItemSrc == p_GraphicsElementItem) ||
					(p_GraphicsLinkItem->p_GraphicsElementItemDst == p_GraphicsElementItem))
			{
				if(!vp_GraphicsLinkItems.contains(p_GraphicsLinkItem))
				{
					vp_GraphicsLinkItems.append(p_GraphicsLinkItem);
				}
			}
		}
	}
	for(int iG = 0; iG != p_GraphicsGroupItem->vp_ConnectedGroups.count(); iG++)
	{
		AddGroupObjectsToTableRecursively(p_GraphicsGroupItem->vp_ConnectedGroups.at(iG));
	}
}

// Конструктор.
Edit_Links_Dialog::Edit_Links_Dialog(QWidget* p_Parent) :
	QDialog(p_Parent),
	p_ui(new Ui::Edit_Links_Dialog)
{
	int iRows;
	int iInitColumnWidth;
	int iPortCompensationWidth;
	int iDirectionCompensationWidth;
	int iCompensationOverall;
	double dbElementCompensationWidth;
	int iElementWidth;
	int iLastColumnCompensation;
	//
	vp_GraphicsLinkItems.clear();
	p_ui->setupUi(this);
	//
	for(int iF = 0; iF != SchematicWindow::vp_SelectedGroups.count(); iF++)
	{
		AddGroupObjectsToTableRecursively(SchematicWindow::vp_SelectedGroups.at(iF));
	}
	//
	for(int iE = 0; iE != SchematicWindow::vp_SelectedElements.count(); iE++)
	{
		GraphicsElementItem* p_GraphicsElementItem = SchematicWindow::vp_SelectedElements.at(iE);
		//
		for(int iL = 0; iL != SchematicWindow::vp_Links.count(); iL++)
		{
			GraphicsLinkItem* p_GraphicsLinkItem = SchematicWindow::vp_Links.at(iL);
			//
			if((p_GraphicsLinkItem->p_GraphicsElementItemSrc == p_GraphicsElementItem) ||
					(p_GraphicsLinkItem->p_GraphicsElementItemDst == p_GraphicsElementItem))
			{
				if(!vp_GraphicsLinkItems.contains(p_GraphicsLinkItem))
				{
					vp_GraphicsLinkItems.append(p_GraphicsLinkItem);
				}
			}
		}
	}
	iRows = vp_GraphicsLinkItems.count();
	p_ui->tableWidget_Links->setColumnCount(5);
	p_ui->tableWidget_Links->setRowCount(iRows);
	for(int iL = 0; iL != iRows; iL++)
	{
		QString strItemSelected;
		QString strItemAnother;
		QString strItemSelectedPort;
		QString strItemAnotherPort;
		QString strDirection;
		QTableWidgetItem* p_QTableWidgetItem;
		GraphicsLinkItem* p_GraphicsLinkItem = vp_GraphicsLinkItems.at(iL);
		//
		if(p_GraphicsLinkItem->p_GraphicsElementItemSrc->bSelected)
		{
			strItemSelectedPort = strItemSelected.setNum(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort).rightJustified(5) + " ";
			strItemAnotherPort = strItemSelected.setNum(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort).rightJustified(5) + " ";
			strItemSelected = QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName);
			strItemAnother = QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName);
			strDirection = "=>";
		}
		else
		{
			strItemSelectedPort = strItemSelected.setNum(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort).rightJustified(5) + " ";
			strItemAnotherPort = strItemSelected.setNum(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort).rightJustified(5) + " ";
			strItemSelected = QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName);
			strItemAnother = QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName);
			strDirection = "<=";
		}
		p_QTableWidgetItem = new QTableWidgetItem(strItemSelected);
		p_ui->tableWidget_Links->setItem(iL, 0, p_QTableWidgetItem);
		p_QTableWidgetItem = new QTableWidgetItem(strItemSelectedPort);
		p_QTableWidgetItem->setTextAlignment(Qt::AlignRight);
		p_ui->tableWidget_Links->setItem(iL, 1, p_QTableWidgetItem);
		p_QTableWidgetItem = new QTableWidgetItem(strDirection);
		p_QTableWidgetItem->setTextAlignment(Qt::AlignCenter);
		p_ui->tableWidget_Links->setItem(iL, 2, p_QTableWidgetItem);
		p_QTableWidgetItem = new QTableWidgetItem(strItemAnotherPort);
		p_QTableWidgetItem->setTextAlignment(Qt::AlignRight);
		p_ui->tableWidget_Links->setItem(iL, 3, p_QTableWidgetItem);
		p_QTableWidgetItem = new QTableWidgetItem(strItemAnother);
		p_ui->tableWidget_Links->setItem(iL, 4, p_QTableWidgetItem);
	}
	iInitColumnWidth = p_ui->tableWidget_Links->columnWidth(0);
	iPortCompensationWidth = iInitColumnWidth - EDIT_LINK_PORT_WIDTH;
	iDirectionCompensationWidth = iInitColumnWidth - EDIT_LINK_DIRECTION_WIDTH;
	iCompensationOverall = iPortCompensationWidth + iDirectionCompensationWidth + iPortCompensationWidth;
	dbElementCompensationWidth = (iCompensationOverall / 2.0f) - (p_ui->tableWidget_Links->verticalScrollBar()->sizeHint().width() / 2.0f);
	iElementWidth = iInitColumnWidth + dbElementCompensationWidth;
	if(dbElementCompensationWidth > (int)dbElementCompensationWidth) iLastColumnCompensation = 1; else iLastColumnCompensation = 0;
	p_ui->tableWidget_Links->setColumnWidth(0, iElementWidth);
	p_ui->tableWidget_Links->setColumnWidth(1, EDIT_LINK_PORT_WIDTH);
	p_ui->tableWidget_Links->setColumnWidth(2, EDIT_LINK_DIRECTION_WIDTH);
	p_ui->tableWidget_Links->setColumnWidth(3, EDIT_LINK_PORT_WIDTH);
	p_ui->tableWidget_Links->setColumnWidth(4, iElementWidth + iLastColumnCompensation);
}

// Деструктор.
Edit_Links_Dialog::~Edit_Links_Dialog()
{
	delete p_ui;
	vp_GraphicsLinkItems.clear();
}
