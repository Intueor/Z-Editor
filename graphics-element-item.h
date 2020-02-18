#ifndef GRAPHICSELEMENTITEM_H
#define GRAPHICSELEMENTITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>
#include <QGroupBox>
#include "graphics-group-item.h"
#include "graphics-port-item.h"
#include "graphics-link-item.h"
#include "graphics-frame-item.h"
#include "graphics-scaler-item.h"
#include "../Z-Hub/Server/protocol.h"
#include "../Z-Hub/logger.h"

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
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget) override;
							///< \param[in] p_Painter Указатель на отрисовщик.
							///< \param[in] p_Option Указатель на опции стиля.
							///< \param[in] p_Widget Указатель на виджет.
	/// Поднятие элемента на первый план и подготовка отсылки по запросу.
	static void ElementToTopAPFS(GraphicsElementItem* p_Element, bool bAddElementGroupChange = false, bool bAddBusyOrZPosToSending = true,
												 bool bBlokingPattern = true, bool bSend = true);
							///< \param[in] p_Element Указатель на граф. элемент.
							///< \param[in] bAddElementGroupID При true - передача элементом изменения текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - установка флага занятости, иначе - отправка z-позиций.
							///< \param[in] bBlokingPattern При true - включение блокировочного паттерна на элемент.
							///< \param[in] bSend При true - отправка.
	/// Переопределение функции обработки нажатия мыши.
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
							///< \param[in] event Указатель на событие.
	/// Переопределение функции обработки перемещения мыши.
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
							///< \param[in] event Указатель на событие.
	/// Переопределение функции обработки отпускания мыши.
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
							///< \param[in] event Указатель на событие.
	/// Обновление фрейма группы по геометрии включённых элементов.
	static void UpdateGroupFrameByElements(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на корректируемую группу.
	/// Обновление выбранных граф. объектов.
	static void UpdateSelected(GraphicsElementItem* p_GraphicsElementItem, unsigned short ushBits,
							   GraphicsPortItem* p_GraphicsPortItem = nullptr,
							   GraphicsLinkItem *p_GraphicsLinkItem = nullptr, bool bIsIncoming = false);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] ushBits Биты флагов необходимых обновлений.
							///< \param[in] p_GraphicsPortItem Указатель на граф. объект порта, при необходимости.
							///< \param[in] p_GraphicsLinkItem Указатель на граф. объект линка, при необходимости.
							///< \param[in] bIsIncoming Признак того, что изменения пришли извне.
	/// Установка параметров элемента по структуре.
	static void SetElementParameters(GraphicsElementItem* p_GraphicsElementItem, PSchElementBase& a_PSchElementBase);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] a_PSchElementBase Ссылка на объект структуры с данными по элементу.
	/// Входное обновление параметров элемента по структуре.
	static void IncomingUpdateElementParameters(GraphicsElementItem* p_GraphicsElementItem, PSchElementVars &a_SchElementVars);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] a_SchElementVars Ссылка на объект структуры с изменениями по элементу.
	/// Установка паттерна блокировки на элемент.
	static void SetBlockingPattern(GraphicsElementItem* p_GraphicsElementItem, bool bValue);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] bValue Вкл\Выкл блокировки.
	/// Добавление объекта порта в лист.
	static void AddPort(GraphicsLinkItem* p_GraphicsElementItem, bool bSrc, GraphicsElementItem* p_Parent);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] bSrc Признак запрса создания на источник (false - приёмник).
							///< \param[in] p_Parent Указатель на объект родителя-элемента.
	/// Отпускание элемента и подготовка отправки по запросу.
	static void ReleaseElementAPFS(GraphicsElementItem* p_GraphicsElementItem, bool bWithGroup = true, bool bWithPosition = true);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] bWithElements При true - отпускать и содержащую группу.
							///< \param[in] bWithPosition При true - передатьна сервер координаты элемента.
	/// Удаление всех графических элементов портов с элемента по ID.
	static void RemovePortsByID(unsigned long long ullID);
							///< \param[in] ullID ID элемента.
	/// Добавление свободных элементов в группу и подготовка к отправке по запросу.
	static bool AddFreeSelectedElementsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
											   GraphicsElementItem* p_GraphicsElementItemInitial = nullptr);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] p_GraphicsGroupItem Указатель на инициирующий граф. элемент (по необходимости).
							///< \return true, если добавлялись пакеты в буфер.
protected:
	/// Переопределение функции шага событий элемента.
	void advance(int iStep) override;
							///< \param[in] iStep Текущий шаг сцены.

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

private:
	LOGDECL_MULTIOBJECT
	LOGDECL_PTHRD_INCLASS_ADD
	GraphicsScalerItem* p_GraphicsScalerItem; ///< Указатель на графический объект скалера.
	QGraphicsProxyWidget* p_QGraphicsProxyWidget; ///< Указатель на виджет-посредник для сцены.
	static QString strAddGroupName; ///< Рабочая строка для добавления имени группы.
	static QVector<GraphicsElementItem*>* vp_NewElementsForGroup; ///< Вектор для заполнения новыми элементами для группы, очищается при добавлении.
};

#endif // GRAPHICSELEMENTITEM_H
