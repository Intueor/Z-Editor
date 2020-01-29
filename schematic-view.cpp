//== ВКЛЮЧЕНИЯ.
#include <math.h>
#include "schematic-view.h"
#include "schematic-window.h"

//== МАКРОСЫ.
#define SCH_INTERNAL_POS_UNCHANGED          -2147483647

//== ДЕКЛАРАЦИИ СТАТИЧЕСКИХ ПЕРЕМЕННЫХ.
bool SchematicView::bLMousePressed = false;
int SchematicView::iXInt = SCH_INTERNAL_POS_UNCHANGED;
int SchematicView::iYInt = SCH_INTERNAL_POS_UNCHANGED;

//== ФУНКЦИИ КЛАССОВ.
//== Класс виджета обзора.
// Конструктор.
SchematicView::SchematicView(QWidget* parent) : QGraphicsView(parent)
{
	srand(time(NULL));
	setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
}

// Определение области видимости.
QRectF SchematicView::GetVisibleRect()
{
	QPointF pntA = this->mapToScene(0, 0);
	QPointF pntB = this->mapToScene(this->viewport()->width(), this->viewport()->height());
	return QRectF(pntA, pntB);
}

