#ifndef GRAPHICSGROUPITEM_H
#define GRAPHICSGROUPITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>
#include <QLabel>
#include "../Z-Hub/Server/protocol.h"
#include "graphics-frame-item.h"
#include "graphics-element-item.h"

//== КЛАССЫ.
/// Класс графической группы.
class GraphicsGroupItem : public QGraphicsItem
{
public:
	/// Структура для передачи отсортированных элементов.
	struct SortedElements
	{
		QVector<GraphicsElementItem*> vp_SortedElements;
		QVector<GraphicsElementItem*> vp_SelectionSortedElements;
	};
	/// Структура для передачи отсортированных групп.
	struct SortedGroups
	{
		QVector<GraphicsGroupItem*> vp_SortedGroups;
		QVector<GraphicsGroupItem*> vp_SelectionSortedGroups;
	};

public:
	/// Конструктор.
	explicit GraphicsGroupItem(PSchGroupBase* p_PSchGroupBase);
							///< \param[in] p_PSchGroupBase Указатель на структуру с параметрами группы для копии внутрь.
	/// Деструктор.
	~GraphicsGroupItem();
	/// Переопределение функции сообщения о вмещающем прямоугольнике.
	QRectF boundingRect() const override;
							///< \return Вмещающий прямоугольник.
	/// Переопределение функции рисования группы.
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
	/// Переопределение функции шага событий группы.
	void advance(int iStep) override;
							///< \param[in] iStep Текущий шаг сцены.

public:
	PSchGroupBase oPSchGroupBaseInt; ///< Внутренний объект структуры с параметрами группы.
	QBrush oQBrush; ///< Кисть для группы.
	QPalette oQPalette; ///< Палитра для группы.
	bool bIsPositivePalette; ///< Признак положительной окраски палитры.
	DbPoint oDbPointDimIncrements; ///< Переменная для передачи отклонений при скалировании на обновление.
	bool bSelected; ///< Признак выбранной группы.
	GraphicsFrameItem* p_GraphicsFrameItem; ///< Указатель на графический объект рамки.
	QVector<GraphicsElementItem*> vp_ConnectedElements; ///< Вектор указателей на графические элементы, включённые в группу.
	QVector<GraphicsGroupItem*> vp_ConnectedGroups; ///< Вектор указателей на графические группы, включённые в группу.
	GraphicsGroupItem* p_GraphicsGroupItemRel; ///< Всп. указатель на группу-родителя.
	QLabel* p_QLabel; ///< Указатель на объект метки названия группы.
	QGraphicsProxyWidget* p_QGraphicsProxyWidget; ///< Указатель на виджет-посредник для сцены.
};

#endif // GRAPHICSGROUPITEM_H
