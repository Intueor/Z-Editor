#ifndef SCHEMATICVIEW_H
#define SCHEMATICVIEW_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsView>
#include <QWheelEvent>
#include "../Z-Hub/Server/protocol.h"

//== ОПРЕДЕЛЕНИЯ ТИПОВ.
typedef void (*CBSchematicViewFrameChanged)(QRectF oQRectFVisibleFrame);

//== ПРЕД-ДЕКЛАРАЦИИ.
class GraphicsElementItem;
class GraphicsGroupItem;
class GraphicsLinkItem;
class GraphicsPortItem;
class GraphicsFrameItem;
class GraphicsScalerItem;

//== КЛАССЫ.
/// Класс виджета обзора.
class SchematicView : public QGraphicsView
{
	Q_OBJECT

public:
	struct DbPointPair
	{
		DbPoint dbSrc;
		DbPoint dbDst;
	};
	struct CalcPortHelper
	{
		QRectF oQRectF;
		DbPointPair oDbPointPair;
	};

public:
	/// Конструктор.
	explicit SchematicView(QWidget* parent = nullptr);
							///< \param[in] parent Указатель на родительский виджет.
	/// Переопределение функции обработки перемещения мыши.
	void mouseMoveEvent(QMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Определение области видимости.
	static QRectF GetVisibleRect();
							///< \return Структура определения прямоугольника области видимости.
	/// Установка указателя кэлбэка изменения окна обзора.
	static void SetSchematicViewFrameChangedCB(CBSchematicViewFrameChanged pf_CBSchematicViewFrameChanged);
								///< \param[in] pf_CBSchematicViewFrameChanged Указатель на пользовательскую функцию.
	/// Подготовка отсылки параметров и удаление группы.
	static void DeleteGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
	/// Подготовка отсылки параметров и удаление элемента (а так же добавка его группы в лист).
	static void DeleteElementAPFS(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	// Удаление линка.
	static void DeleteLinkAPFS(GraphicsLinkItem* p_GraphicsLinkItem, bool bFromElement = false, bool bRemoveFromClient = true);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \param[in] bFromElement Признак вызова из элемента.
							///< \param[in] bRemoveFromClient Флаг для удаления с клиента.
	/// Остоединение выбранных элементов от группы и подготовка отправки всех изменеий на сервер.
	static bool DetachSelectedAPFS();
							///< \return true, если добавлялись пакеты в буфер.
	/// Удаление выбранного и подготовка отправки по запросу.
	static void DeleteSelectedAPFS();
	/// Создание нового элемента и подготовка отсылки параметров.
	static GraphicsElementItem* CreateNewElementAPFS(char* p_chNameBase, QPointF a_pntMapped, unsigned long long ullIDGroup = 0);
							///< \param[in] p_chNameBase Указатель на строку с базой нового имени.
							///< \param[in] a_pntMapped Ссылка на координаты.
							///< \param[in] ullIDGroup ИД группы или ноль для отдельного элемента.
							///< \return Указатель на новый графический элемент.
	/// Обновление Z-позиции линков.
	static void UpdateLinksZPos();
	/// Замена линка.
	static bool ReplaceLink(GraphicsLinkItem* p_GraphicsLinkItem, GraphicsElementItem* p_GraphicsElementItemNew, bool bIsSrc,
							DbPoint oDbPortPos, bool bFromElement);
							///< \param[in] p_GraphicsLinkItem Указатель на старый линк.
							///< \param[in] p_GraphicsElementItemNew Указатель на элемент, на который будет создан новый линк.
							///< \param[in] bIsSrc При true - новый элемент будет источником.
							///< \param[in] oDbPortPos Координаты порта в сцене на момент обращения.
							///< \param[in] bFromElement Флаг запроса работы с портом от элемента (при создании нового линка).
							///< \return true при удаче.
	/// Прикрепление позиции граф. порта к краям элемента.
	static DbPoint BindToInnerEdge(GraphicsElementItem* p_GraphicsElementItemNew, DbPoint oDbPortPosInitial);
							///< \param[in] p_GraphicsElementItemNew Указатель на элемент.
							///< \param[in] oDbPortPosIn Изначальная позиция порта.
							///< \return Позиция порта на крае элемента.
	/// Получение длины строки выбранным шрифтом в пикселях.
	static int GetStringWidthInPixels(const QFont& a_Font, QString& a_strText);
							///< \param[in] a_Font Ссылка на шрифт.
							///< \param[in] a_strText Ссылка на строку.
							///< \return Ширина в пикселях.
	/// Поднятие элемента на первый план и подготовка отсылки по запросу.
	static void ElementToTopAPFS(GraphicsElementItem* p_Element, bool bAddElementGroupChange = false, bool bAddBusyOrZPosToSending = true,
												 bool bBlokingPattern = true, bool bSend = true);
							///< \param[in] p_Element Указатель на граф. элемент.
							///< \param[in] bAddElementGroupID При true - передача элементом изменения текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - установка флага занятости, иначе - отправка z-позиций.
							///< \param[in] bBlokingPattern При true - включение блокировочного паттерна на элемент.
							///< \param[in] bSend При true - отправка.
	/// Обновление фрейма группы по геометрии контента.
	static void UpdateGroupFrameByContent(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на корректируемую группу.
	/// Обновление выбранных параметров в элементе.
	static void UpdateSelectedInElement(GraphicsElementItem* p_GraphicsElementItem, unsigned short ushBits,
							   GraphicsPortItem* p_GraphicsPortItem = nullptr,
							   GraphicsLinkItem *p_GraphicsLinkItem = nullptr, bool bIsIncoming = false);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] ushBits Биты флагов необходимых обновлений.
							///< \param[in] p_GraphicsPortItem Указатель на граф. объект порта, при необходимости.
							///< \param[in] p_GraphicsLinkItem Указатель на граф. объект линка, при необходимости.
							///< \param[in] bIsIncoming Признак того, что изменения пришли извне.
	/// Обновление выбранных параметров в группе.
	static void UpdateSelectedInGroup(GraphicsGroupItem* p_GraphicsGroupItem, unsigned short ushBits);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] ushBits Биты флагов необходимых обновлений.
	/// Установка паттерна блокировки на элемент.
	static void SetElementBlockingPattern(GraphicsElementItem* p_GraphicsElementItem, bool bValue);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] bValue Вкл\Выкл блокировки.
	/// Установка паттерна блокировки на группу.
	static void SetGroupBlockingPattern(GraphicsGroupItem* p_GraphicsGroupItem, bool bValue);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] bValue Вкл\Выкл блокировки.
	/// Отпускание элемента и подготовка отправки по запросу.
	static void ReleaseElementAPFS(GraphicsElementItem* p_GraphicsElementItem, bool bWithGroup = true, bool bWithFrame = false);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] bWithElements При true - отпускать и содержащую группу.
							///< \param[in] bWithFrame При true - передать на сервер фрейм элемента.
	/// Удаление всех графических элементов портов с элемента по ID.
	static void RemovePortsByID(unsigned long long ullID);
							///< \param[in] ullID ID элемента.
	/// Добавление свободных элементов в группу и подготовка к отправке по запросу.
	static bool AddFreeSelectedElementsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
											   GraphicsElementItem* p_GraphicsElementItemInitial = nullptr);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] p_GraphicsGroupItem Указатель на инициирующий граф. элемент (по необходимости).
							///< \return true, если добавлялись пакеты в буфер.
	/// Выбор элемента.
	static void SelectElement(GraphicsElementItem* p_GraphicsElementItem, bool bLastState = true);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Отмена выбора элемента.
	static void DeselectElement(GraphicsElementItem* p_GraphicsElementItem, bool bLastState = true);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Подъём элементов группы на первый план с сортировкой и подготовкой отсылки.
	static void SortGroupElementsToTopAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
										   bool bAddNewElementsToGroupSending = false, bool bAddBusyOrZPosToSending = true,
										   GraphicsElementItem* p_GraphicsElementItemExclude = nullptr, bool bWithSelectedDiff = true,
										   bool bBlokingPatterns = true, bool bSend = true);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bAddNewElementsToGroupSending При true - передача включёнными в список элементами параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - установка флага занятости, иначе - отправка z-позиций рекурсивно.
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
	/// Поднятие группы на первый план и подготовка к отсылке по запросу.
	static void GroupToTopAPFS(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend = true,
							   bool bAddNewElementsToGroupSending = false, bool bAddBusyOrZPosToSending = true, bool bAddFrame = false,
							   GraphicsElementItem* p_GraphicsElementItemExclude = nullptr,
							   bool bBlokingPatterns = true, bool bSendElements = true);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер.
							///< \param[in] bAddNewElementsToGroupSending При true - передача содержащимеся элементами параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - установка флага занятости, иначе - отправка z-позиций рекурсивно.
							///< \param[in] bAddFrame При true - передача фрейма группы.
							///< \param[in] p_GraphicsElementItemExclude Указатель на исключаемый элемент или nullptr.
							///< \param[in] bBlokingPatterns При true - включение блокировочных паттернов на элементы.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
	/// Отпускание группы и подготовка отправки по запросу.
	static void ReleaseGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem, GraphicsElementItem* p_GraphicsElementItemExclude = nullptr,
								 bool bWithFrame = true, bool bWithElementFrames = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_GraphicsElementItemExclude Указатель на исключаемый элемент.
							///< \param[in] bWithFrame При true - передать на сервер фрейм группы.
							///< \param[in] bWithElementFrames При true - передать на сервер фреймы элементов.
	/// Выбор группы.
	static void SelectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Отмена выбора группы.
	static void DeselectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Перемещение группы.
	static void MoveGroup(GraphicsGroupItem* p_GraphicsGroupItem, QPointF& a_QPointFRes, bool bMoveBody = true);
							///< \param[in] p_GraphicsGroupItem Указатель на перемещаемую группу.
							///< \param[in] a_QPointFRes Ссылка на смещение для группы.
							///< \param[in] bMoveBody При true - смещать и тело группы.
	/// Обновление Z-позиции линка по данным элементов.
	static void UpdateLinkZPositionByElements(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на граф. линк.
	/// Обновление позиции линка по данным элементов.
	static void UpdateLinkPositionByElements(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на граф. линк.
	/// Помощник коррекции точки порта по краю элемента.
	static void BindPortToOuterEdgeHelper();
	/// Установка порта в позицию.
	static void SetPortToPos(GraphicsPortItem* p_GraphicsPortItem);
							///< \param[in] p_GraphicsLinkItem Указатель на граф. порт.

	/// Обработчик события нажатия мыши на элемент.
	static void ElementMousePressEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события перемещения мыши с элементом.
	static void ElementMouseMoveEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события отпусканеия мыши на элементе.
	static void ElementMouseReleaseEventHandler(GraphicsElementItem* p_GraphicsElementItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик функции рисования элемента.
	static void ElementPaintHandler(GraphicsElementItem* p_GraphicsElementItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора элемента.
	static void ElementConstructorHandler(GraphicsElementItem* p_GraphicsElementItem, PSchElementBase* p_PSchElementBase);
							///< \param[in] GraphicsElementItem Указатель на элемент.
							///< \param[in] p_PSchElementBase Указатель на структуру с параметрами элемента для копии внутрь.

	/// Обработчик события нажатия мыши на группу.
	static void GroupMousePressEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события перемещения мыши с группой.
	static void GroupMouseMoveEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события отпусканеия мыши на группе.
	static void GroupMouseReleaseEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик функции рисования группы.
	static void GroupPaintHandler(GraphicsGroupItem* p_GraphicsGroupItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора группы.
	static void GroupConstructorHandler(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupBase* p_PSchGroupBase);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_PSchGroupBase Указатель на структуру с параметрами элемента для копии внутрь.

	/// Обработчик функции рисования фрейма.
	static void FramePaintHandler(GraphicsFrameItem* p_GraphicsFrameItem, QPainter* p_Painter);
							///< \param[in] GraphicsFrameItem Указатель на фрейм.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора фрейма.
	static void FrameConstructorHandler(GraphicsFrameItem* p_GraphicsFrameItem, unsigned short ushKindOfItem,
										GraphicsElementItem* p_ElementParent, GraphicsGroupItem* p_GroupParent, GraphicsPortItem* p_PortParent);
							///< \param[in] p_GraphicsFrameItem Указатель на фрейм.
							///< \param[in] ushKindOfItem Тип родителя.
							///< \param[in] p_ElementParent Указатель на родитель-элемент.
							///< \param[in] p_GroupParent Указатель на родитель-группу.
							///< \param[in] p_PortParent Указатель на родитель-порт.
	/// Обработчик вместилища фрейма.
	static QRectF FrameBoundingRectHandler(GraphicsFrameItem* p_GraphicsFrameItem);
							///< \param[in] p_GraphicsFrameItem Указатель на фрейм.
							///< \return Вмещающий прямоугольник.

	/// Обработчик функции рисования линка.
	static void LinkPaintHandler(GraphicsLinkItem* p_GraphicsLinkItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора линка.
	static void LinkConstructorHandler(GraphicsLinkItem* p_GraphicsLinkItem, PSchLinkBase* p_PSchLinkBase);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \param[in] p_PSchLinkBase Указатель на структуру с параметрами линка для копии внутрь.
	/// Вычисление квадрата и вместилища линии линка.
	static CalcPortHelper CalcLinkLineWidthHeight(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \return Вмещающий прямоугольник и точки портов.
	/// Вычисление точек портов.
	static DbPointPair CalcPortsCoords(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \return Точки портов.

	/// Обработчик события нажатия мыши на порт.
	static void PortMousePressEventHandler(GraphicsPortItem* p_GraphicsPortItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsPortItem Указатель на порт.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события перемещения мыши с портом.
	static void PortMouseMoveEventHandler(GraphicsPortItem* p_GraphicsPortItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsPortItem Указатель на порт.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события отпусканеия мыши на порте.
	static void PortMouseReleaseEventHandler(GraphicsPortItem* p_GraphicsPortItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsPortItem Указатель на порт.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик функции рисования порта.
	static void PortPaintHandler(GraphicsPortItem* p_GraphicsPortItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsPortItem Указатель на порт.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора порта.
	static void PortConstructorHandler(GraphicsPortItem* p_GraphicsPortItem, GraphicsLinkItem* p_GraphicsLinkItem,
									   bool bSrc, GraphicsElementItem* p_Parent);
							///< \param[in] p_GraphicsPortItem Указатель на порт.
							///< \param[in] p_GraphicsLinkItem Указатель на граф. структуру с параметрами линка.
							///< \param[in] bSrc Признак порта на источнике.
							///< \param[in] p_Parent Указатель на родителя.
	/// Обработчик нахождения курсора над портом.
	static void PortHoverEnterEventHandler(GraphicsPortItem* p_GraphicsPortItem);
							///< \param[in] p_GraphicsPortItem Указатель на порт.
	/// Обработчик ухода курсора с порта.
	static void PortHoverLeaveEventHandler(GraphicsPortItem* p_GraphicsPortItem);
							///< \param[in] p_GraphicsPortItem Указатель на порт.

	/// Обработчик события нажатия мыши на скалер.
	static void ScalerMousePressEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события перемещения мыши с скалером.
	static void ScalerMouseMoveEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик события отпусканеия мыши на скалере.
	static void ScalerMouseReleaseEventHandler(GraphicsScalerItem* p_GraphicsScalerItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик функции рисования скалера.
	static void ScalerPaintHandler(GraphicsScalerItem* p_GraphicsScalerItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора скалера.
	static void ScalerConstructorHandler(GraphicsScalerItem* p_GraphicsScalerItem, GraphicsElementItem* p_Parent);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] p_Parent Указатель на родителя.
protected:
	/// Переопределение функции обработки событий колёсика.
	void wheelEvent(QWheelEvent* p_Event);
							///< \param[in] p_Event Указатель на событие колёсика.
	/// Переопределение функции обработки нажатия на кнопку мыши.
	void mousePressEvent(QMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки отпускания кнопки мыши.
	void mouseReleaseEvent(QMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
	/// Переопределение функции обработки перетаскивания вида.
	void scrollContentsBy(int iX, int iY);
							///< \param[in] iX Координаты X.
							///< \param[in] iY Координаты Y.
	/// Переопределение функции обработки нажатия на клавиши.
	void keyPressEvent(QKeyEvent* p_Event);
							///< \param[in] p_Event Указатель на событие клавиши.
private:
	/// Подготовка удаления графического элемента из сцены и группы, возврат флага на удаление группы элемента.
	static bool PrepareForRemoveElementFromScene(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \return true, если был удалён пустой графический объект группы.
private:
	static bool bLMousePressed; ///< Признак нажатия на ЛКМ.
	static int iXInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по X.
	static int iYInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по Y.
	static CBSchematicViewFrameChanged pf_CBSchematicViewFrameChangedInt; ///< Указатель на кэлбэк изменения окна обзора.
	static QPointF pntMouseClickMapped; ///< Точка клика в координатах сцены.
	static QPointF pntMouseMoveMapped; ///< Точка смещения от клика в координатах сцены.
	qreal rScaleFactor; ///<
	qreal rFactor; ///<

public:
	static GraphicsPortItem* p_GraphicsPortItemActive; ///< Указатель на текущий выбранный порт или nullptr.
	static QGraphicsRectItem* p_QGraphicsRectItemSelectionDash; ///< Прямоугольник выборки, линии.
	static QGraphicsRectItem* p_QGraphicsRectItemSelectionDot; ///< Прямоугольник выборки, точки.
	static bool bShiftAndLMBPressed; ///< Признак нажатия на Shift при клике ЛКМ.
	static QVector<GraphicsElementItem*>* vp_NewElementsForGroup; ///< Вектор для заполнения новыми элементами для группы, очищается при добавлении.
	static GraphicsLinkItem* p_GraphicsLinkItemNew; ///< Указатель на новосозданный линк.
	static bool bPortAltPressed; ///< Флаг нажатого модификатора Alt для порта.
	static bool bPortLMBPressed; ///< Флаг нажатой ЛКМ для порта.
	static DbPoint oDbPointPortRB; ///< Точка правого нижнего края элемента для порта.
	static DbPoint oDbPointPortCurrent; ///< Текщая точка для порта.
	static DbPoint oDbPointPortOld; ///< Старая точка для порта.
	static DbPoint oDbPointPortInitialClick; ///< Точка нажатия для порта.
	static bool bPortFromElement; ///< Флаг запроса от элемента для порта.
	static bool bPortMenuExecuted; ///< Флаг выполненного меню для отмены ховера для порта.
};

#endif // SCHEMATICVIEW_H
