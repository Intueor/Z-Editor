#ifndef GRAPHICSPORTITEM_H
#define GRAPHICSPORTITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>
#include "graphics-link-item.h"
#include "graphics-element-item.h"

//== КЛАССЫ.
/// Класс графического отображения порта.
class GraphicsPortItem : public QGraphicsItem
{
public:
	/// Конструктор.
	explicit GraphicsPortItem(GraphicsLinkItem* p_GraphicsLinkItem, bool bSrc, GraphicsElementItem* p_Parent);
							///< \param[in] p_GraphicsLinkItem Указатель на граф. структуру с параметрами линка.
							///< \param[in] bIsSrc Признак порта на источнике.
							///< \param[in] p_Parent Указатель на родителя.
	/// Деструктор.
	~GraphicsPortItem();
	/// Переопределение функции сообщения о вмещающем прямоугольнике.
	QRectF boundingRect() const override;
							///< \return Вмещающий прямоугольник.
	/// Переопределение функции рисования элемента.
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget) override;
							///< \param[in] p_Painter Указатель на отрисовщик.
							///< \param[in] p_Option Указатель на опции стиля.
							///< \param[in] p_Widget Указатель на виджет.
	/// Переопределение функции обработки нажатия мыши.
	void mousePressEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки перемещения мыши.
	void mouseMoveEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки отпускания мыши.
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки нахождения курсора над портом.
	void hoverEnterEvent(QGraphicsSceneHoverEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки ухода курсора с порта.
	void hoverLeaveEvent(QGraphicsSceneHoverEvent* p_Event) override;
							///< \param[in] p_Event Указатель на событие.
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
	/// Переопределение функции шага событий элемента.
	void advance(int iStep) override;
							///< \param[in] iStep Текущий шаг сцены.

public:
	PSchLinkVars* p_PSchLinkVarsInt; ///< Внутренняя ссылка на объект структуры с параметрами соотв. линка.
	GraphicsElementItem* p_ParentInt; ///< Внутренний указатель на родителя.
	GraphicsLinkItem* p_GraphicsLinkItemInt; ///< Внутренний указатель на линк.
	bool bIsSrc; ///< Признак порта-источника.
	GraphicsFrameItem* p_GraphicsFrameItem; ///< Указатель на графический объект рамки.
	ushort ushiPortInt; ///< Внутренняя переменная номера порта.
	SchEGGraph* p_SchEGGraph; ///< Указтель на граф. представление элемента.
	DbPoint oDbPAlterPos; ///< Альтернативное положение порта для обмена с минимизированным статусом.
};

#endif // GRAPHICSPORTITEM_H
