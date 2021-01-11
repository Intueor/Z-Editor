#ifndef GRAPHICSLINKITEM_H
#define GRAPHICSLINKITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>
#include "../Z-Hub/Server/protocol.h"
#include "graphics-port-item.h"

//== ПРЕД-ДЕКЛАРАЦИИ.
class GraphicsPortItem;

//== КЛАССЫ.
/// Класс графического линка.
class GraphicsLinkItem : public QGraphicsItem
{
public:
	/// Конструктор.
	explicit GraphicsLinkItem(PSchLinkBase* p_PSchLinkBase);
							///< \param[in] p_PSchLinkBase Указатель на структуру с параметрами линка для копии внутрь.
	/// Деструктор.
	~GraphicsLinkItem();
	/// Переопределение функции сообщения о вмещающем прямоугольнике.
	QRectF boundingRect() const override;
							///< \return Вмещающий прямоугольник.
	/// Переопределение функции рисования линка.
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem*, QWidget*) override;
							///< \param[in] p_Painter Указатель на отрисовщик.
public:
	PSchLinkBase oPSchLinkBaseInt; ///< Внутренний объект структуры с параметрами линка.
	GraphicsPortItem* p_GraphicsPortItemSrc; ///< Внутренний указатель на граф. объект структуры с параметрами порта источника.
	GraphicsPortItem* p_GraphicsPortItemDst; ///< Внутренний указатель на граф. объект структуры с параметрами порта приёмника.
	GraphicsElementItem* p_GraphicsElementItemSrc; ///< Указатель на виджет источника.
	GraphicsElementItem* p_GraphicsElementItemDst; ///< Указатель на виджет приёмника.
	PSchElementVars* p_PSchElementVarsSrc; ///< Указатель на элемент схемы источника.
	PSchElementVars* p_PSchElementVarsDst; ///< Указатель на элемент схемы приёмника.
	bool bExcludeFromPaint; ///< Флаг отказа от отрисовки.
};

#endif // GRAPHICSLINKITEM_H
