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
	/// Подъём элементов группы на первый план с сортировкой.
	static void SortGroupElementsToTop(GraphicsGroupItem* p_GraphicsGroupItem,
									   GraphicsElementItem* p_GraphicsElementItemExclude = nullptr, bool bWithSelectedDiff = true,
									   bool bBlokingPatterns = true, bool bSend = true);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] p_GraphicsElementItemExclude Указатель на элемент для исключения или nullptr.
							///< \param[in] bWithSelectedDiff При true - вынос выбранных элементов на передний план.
							///< \param[in] bBlokingPatterns При true - включение блокировочных паттернов на элементы.
							///< \param[in] bSend При true - отправка на сервер и клиентам.
	/// Сортировка вектора элементов по Z-позиции с приоритетом по выборке.
	static void SortElementsByZPos(QVector<GraphicsElementItem*>& avp_Elements,
								   GraphicsElementItem* p_GraphicsElementItemExclude,
								   QVector<GraphicsElementItem*>* pvp_SortedElements,
								   QVector<GraphicsElementItem*>* pvp_SelectionSortedElements = nullptr);
							///< \param[in] avp_Elements Ссылка на вектор указателей на элементы.
							///< \param[in] p_GraphicsElementItemExclude Указатель на элемент для исключения или nullptr.
							///< \param[in] pvp_SortedElements Указатель на вектор отсортированных элементов для заполнения.
							///< \param[in] pvp_SelectionSortedElements Указатель на вектор отсортированных элементов выборки для заполнения.
	/// Сортировка вектора групп по Z-позиции с приоритетом по выборке.
	static void SortGroupsByZPos(QVector<GraphicsGroupItem*>& avp_Groups,
								 GraphicsGroupItem* p_GraphicsGroupItemExclude,
								 QVector<GraphicsGroupItem*>* pvp_SortedGroups,
								 QVector<GraphicsGroupItem*>* pvp_SelectionSortedGroups = nullptr);
							///< \param[in] аvp_Groups Ссылка на вектор указателей на группы.
							///< \param[in] bWithSelectedDiff При true - вынос выбранных пользователем элементов на передний план.
							///< \param[in] p_GraphicsGroupItemExclude Указатель на группу для исключения или nullptr.
	/// Поднятие группы на первый план.
	static void GroupToTop(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend = true,
						   GraphicsElementItem* p_GraphicsElementItemExclude = nullptr,
						   bool bBlokingPatterns = true, bool bSendElements = true);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер и клиентам.
							///< \param[in] p_GraphicsElementItemExclude Указатель на исключаемый элемент или nullptr.
							///< \param[in] bBlokingPatterns При true - включение блокировочных паттернов на элементы.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
	/// Переопределение функции обработки нажатия мыши.
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
							///< \param[in] event Указатель на событие.
	/// Переопределение функции обработки перемещения мыши.
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
							///< \param[in] event Указатель на событие.
	/// Переопределение функции обработки отпускания мыши.
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
							///< \param[in] event Указатель на событие.
	/// Обновление выбранных граф. объектов.
	static void UpdateSelected(GraphicsGroupItem* p_GraphicsGroupItem, unsigned short ushBits);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] ushBits Биты флагов необходимых обновлений.
	/// Установка параметров группы по структуре.
	static void SetGroupParameters(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupBase& a_PSchGroupBase);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] a_PSchGroupBase Ссылка на объект структуры с данными по группе.
	/// Входное обновление параметров группы по структуре.
	static void IncomingUpdateGroupParameters(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupVars& a_SchGroupVars);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] a_SchGroupVars Ссылка на объект структуры с изменениями по группе.
	/// Установка паттерна блокировки на группу.
	static void SetBlockingPattern(GraphicsGroupItem* p_GraphicsGroupItem, bool bValue);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] bValue Вкл\Выкл блокировки.
	/// Отпускание группы и подготовка отправки.
	static void ReleaseGroupAndPrepareForSending(GraphicsGroupItem* p_GraphicsGroupItem, GraphicsElementItem* p_GraphicsElementItemExclude = nullptr);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] p_GraphicsElementItemExclude Указатель на исключаемый элемент.

private:
	/// Перемещение группы.
	static void MoveGroup(GraphicsGroupItem* p_GraphicsGroupItem, QPointF& a_QPointFRes, bool bMoveBody = true);
							///< \param[in] p_GraphicsGroupItem Указатель на перемещаемую группу.
							///< \param[in] a_QPointFRes Ссылка на смещение для группы.
							///< \param[in] bMoveBody При true - смещать и тело группы.
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
	QLabel* p_QLabel; ///< Указатель на объект метки названия группы.

private:
	LOGDECL_MULTIOBJECT
	LOGDECL_PTHRD_INCLASS_ADD
	QGraphicsProxyWidget* p_QGraphicsProxyWidget; ///< Указатель на виджет-посредник для сцены.
};

#endif // GRAPHICSGROUPITEM_H
