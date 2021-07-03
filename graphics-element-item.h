#ifndef GRAPHICSELEMENTITEM_H
#define GRAPHICSELEMENTITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>
#include <QGroupBox>
#include "graphics-group-item.h"
#include "graphics-port-item.h"
#include "graphics-frame-item.h"
#include "graphics-scaler-item.h"
#include "../Z-Hub/Server/protocol.h"

//== КЛАССЫ.
/// Класс графического элемента.
class GraphicsElementItem : public QGraphicsItem
{
public:
	/// Конструктор.
	explicit GraphicsElementItem(PSchElementBase* p_PSchElementBase);
							///< \param[in] p_PSchElementBase Указатель на структуру с параметрами элемента для копии внутрь.
	/// Деструктор.
	~GraphicsElementItem();
	/// Переопределение функции сообщения о вмещающем прямоугольнике.
	QRectF boundingRect() const override;
							///< \return Вмещающий прямоугольник.
	/// Переопределение функции рисования элемента.
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem*, QWidget*) override;
							///< \param[in] p_Painter Указатель на отрисовщик.

	/// Переопределение функции определения формы.
	QPainterPath shape() const override;
							///< \return Фигура.
	/// Для внешнего вызова базового метода.
	void OBMousePressEvent(QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Для внешнего вызова базового метода.
	void OBMouseMoveEvent(QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Для внешнего вызова базового метода.
	void OBMouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
protected:
	/// Переопределение функции обработки нажатия мыши.
	void mousePressEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки перемещения мыши.
	void mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки отпускания мыши.
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
public:
	PSchElementBase oPSchElementBaseInt; ///< Внутренний объект структуры с параметрами элемента.
	QBrush oQBrush; ///< Кисть для элемента.
	QPalette oQPalette; ///< Палитра для элемента.
	bool bIsPositivePalette; ///< Признак положительной окраски палитры.
	DbPoint oDbPointDimIncrements; ///< Переменная для передачи отклонений при скалировании на обновление.
	bool bSelected; ///< Признак выбранного элемента.
	GraphicsFrameItem* p_GraphicsFrameItem; ///< Указатель на графический объект рамки.
	GraphicsGroupItem* p_GraphicsGroupItemRel; ///< Всп. указатель на группу.
	QGroupBox* p_QGroupBox; ///< Указатель на объект основного групбокса.
	GraphicsScalerItem* p_GraphicsScalerItem; ///< Указатель на графический объект скалера.
	QGraphicsProxyWidget* p_QGraphicsProxyWidget; ///< Указатель на виджет-посредник для сцены.
	bool bPortsForMin; ///< true - при наличии портов на элементе (проверяется при минимизации).
	QString strPreparedName; ///< Строка с подготовленным для отображения названием объекта.
	QWidget* p_QWidgetKeyReceiver; ///< Указатель на виджет обработки ввода клавиатуры, фикс бага Qt (заполняется при создании откликом библиотеки).
	int iLibraryNumber; ///< Номер библиотеки в списке библиотек.
};

#endif // GRAPHICSELEMENTITEM_H
