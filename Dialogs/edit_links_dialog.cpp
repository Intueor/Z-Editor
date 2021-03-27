//== ВКЛЮЧЕНИЯ.
#include <QScrollBar>
#include "edit_links_dialog.h"
#include "ui_edit_links_dialog.h"
#include "Dialogs/edit_port_dialog.h"

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
QVector<GraphicsLinkItem*> Edit_Links_Dialog::vp_GraphicsLinkItems;
bool Edit_Links_Dialog::bInitialized;

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

// Конструирование строк для таблицы.
void Edit_Links_Dialog::ConstructTableRow(GraphicsLinkItem* p_GraphicsLinkItem, QString& a_strItemLeft, QString& a_strItemRight)
{
	QString strHelperSrc;
	QString strHelperDst;
	//
	for(int iP = 0; iP != SchematicWindow::v_PSchPseudonyms.count(); iP++)
	{
		const PSchPseudonym* p_PSchPseudonym = &SchematicWindow::v_PSchPseudonyms.at(iP);
		//
		if(p_PSchPseudonym->ushiPort == p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort)
		{
			strHelperSrc = QString(p_PSchPseudonym->m_chName);
		}
		if(p_PSchPseudonym->ushiPort == p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort)
		{
			strHelperDst = QString(p_PSchPseudonym->m_chName);
		}
		if((!strHelperSrc.isEmpty()) & (!strHelperDst.isEmpty())) break;
	}
	if(strHelperSrc.isEmpty()) strHelperSrc.setNum(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiSrcPort);
	if(strHelperDst.isEmpty()) strHelperDst.setNum(p_GraphicsLinkItem->oPSchLinkBaseInt.oPSchLinkVars.ushiDstPort);
	if(p_GraphicsLinkItem->p_GraphicsElementItemSrc->bSelected)
	{

		a_strItemLeft = QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName) + " [" +
						  strHelperSrc + "]";
		a_strItemRight = "[" + strHelperDst + "] " +
						 QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName);
	}
	else
	{
		a_strItemLeft = QString(p_GraphicsLinkItem->p_GraphicsElementItemDst->oPSchElementBaseInt.m_chName) + " [" +
						  strHelperDst + "]";
		a_strItemRight = "[" + strHelperSrc + "] " +
						 QString(p_GraphicsLinkItem->p_GraphicsElementItemSrc->oPSchElementBaseInt.m_chName);
	}
}

// Конструктор.
Edit_Links_Dialog::Edit_Links_Dialog(QWidget* p_Parent) :
	QDialog(p_Parent),
	p_ui(new Ui::Edit_Links_Dialog)
{
	int iRows;
	double dbInitColumnWidth;
	double dbDirectionCompensationWidth;
	double dbElementCompensationWidth;
	int iElementWidth;
	int iLastColumnCompensation;
	QPixmap oQPixmapR = QPixmap(":/icons/arrow-right.png");
	QPixmap oQPixmapL = oQPixmapR.transformed(QTransform().scale(-1, 1));
	//
	bInitialized = false;
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
	p_ui->tableWidget_Links->setRowCount(iRows);
	for(int iL = 0; iL != iRows; iL++)
	{
		QString strItemLeft;
		QString strItemRight;
		QTableWidgetItem* p_QTableWidgetItem;
		GraphicsLinkItem* p_GraphicsLinkItem = vp_GraphicsLinkItems.at(iL);
		QLabel* p_Label = new QLabel();
		QWidget* p_Widget = new QWidget();
		QHBoxLayout* p_Layout;
		//
		if(p_GraphicsLinkItem->p_GraphicsElementItemSrc->bSelected) p_Label->setPixmap(oQPixmapR);
		else p_Label->setPixmap(oQPixmapL);
		ConstructTableRow(p_GraphicsLinkItem, strItemLeft, strItemRight);
		p_QTableWidgetItem = new QTableWidgetItem(strItemLeft);
		p_QTableWidgetItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		p_ui->tableWidget_Links->setItem(iL, 0, p_QTableWidgetItem);
		//
		p_Label->setMaximumSize(EDIT_LINK_DIRECTION_WIDTH, p_ui->tableWidget_Links->rowHeight(0) / 4);
		p_Label->setScaledContents(true);
		p_Layout = new QHBoxLayout(p_Widget);
		p_Layout->addWidget(p_Label);
		p_Layout->setAlignment(Qt::AlignCenter);
		p_Layout->setContentsMargins(0,0,0,0);
		p_Widget->setLayout(p_Layout);
		p_ui->tableWidget_Links->setCellWidget(iL, 1, p_Widget);
		//
		p_QTableWidgetItem = new QTableWidgetItem(strItemRight);
		p_ui->tableWidget_Links->setItem(iL, 2, p_QTableWidgetItem);
	}
	this->show();
	dbInitColumnWidth = p_ui->tableWidget_Links->contentsRect().width() / 3.0f;
	dbDirectionCompensationWidth = dbInitColumnWidth - EDIT_LINK_DIRECTION_WIDTH;
	dbElementCompensationWidth = (dbDirectionCompensationWidth / 2.0f) - (p_ui->tableWidget_Links->verticalScrollBar()->sizeHint().width() / 2.0f);
	iElementWidth = dbInitColumnWidth + dbElementCompensationWidth;
	if(dbElementCompensationWidth > (int)dbElementCompensationWidth) iLastColumnCompensation = 1; else iLastColumnCompensation = 0;
	p_ui->tableWidget_Links->setColumnWidth(0, iElementWidth);
	p_ui->tableWidget_Links->setColumnWidth(1, EDIT_LINK_DIRECTION_WIDTH);
	p_ui->tableWidget_Links->setColumnWidth(2, iElementWidth + iLastColumnCompensation - 1);
	p_ui->tableWidget_Links->selectRow(0);
	bInitialized = true;
}

// Деструктор.
Edit_Links_Dialog::~Edit_Links_Dialog()
{
	delete p_ui;
	vp_GraphicsLinkItems.clear();
}

// При активации ячейки.
void Edit_Links_Dialog::on_tableWidget_Links_cellActivated(int iRow, int iColumn)
{
	if(bInitialized)
	{
		GraphicsLinkItem* p_GraphicsLinkItem = vp_GraphicsLinkItems.at(iRow);
		//
		if(iColumn == 1)
		{
			return;
		}
		else
		{
			bool bAccepted;
			if(p_GraphicsLinkItem->p_GraphicsElementItemSrc->bSelected)
			{	// Источник слева, приёмник справа.
				if(iColumn == 0)
				{	// Выбрано слева.
					bAccepted = SchematicView::PortMenuOperationsAPFS(MENU_SELECTED_PORT, p_GraphicsLinkItem->p_GraphicsPortItemSrc);
				}
				else
				{	// Выбрано справа.
					bAccepted = SchematicView::PortMenuOperationsAPFS(MENU_SELECTED_PORT, p_GraphicsLinkItem->p_GraphicsPortItemDst);
				}
			}
			else
			{	// Приёмник слева, источник справа.
				if(iColumn == 0)
				{	// Выбрано слева.
					bAccepted = SchematicView::PortMenuOperationsAPFS(MENU_SELECTED_PORT, p_GraphicsLinkItem->p_GraphicsPortItemDst);
				}
				else
				{	// Выбрано справа.
					bAccepted = SchematicView::PortMenuOperationsAPFS(MENU_SELECTED_PORT, p_GraphicsLinkItem->p_GraphicsPortItemSrc);
				}
			}
			if(bAccepted)
			{
				QTableWidgetItem* p_QTableWidgetItem = p_ui->tableWidget_Links->item(iRow, iColumn);
				QString strItemLeft;
				QString strItemRight;
				//
				ConstructTableRow(p_GraphicsLinkItem, strItemLeft, strItemRight);
				if(iColumn == 0)
				{	// Выбрано слева.
					p_QTableWidgetItem->setText(strItemLeft);
				}
				else
				{	// Выбрано справа.
					p_QTableWidgetItem->setText(strItemRight);
				}
				TrySendBufferToServer;
			}
		}
	}
}

// При выборе создания линка.
void Edit_Links_Dialog::on_pushButton_Create_clicked()
{

}
