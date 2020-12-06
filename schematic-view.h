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
	struct EGPointersVariant
	{
		GraphicsElementItem* p_GraphicsElementItem;
		GraphicsGroupItem* p_GraphicsGroupItem;
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
	static void DeleteGroupRecursiveAPFS(GraphicsGroupItem* p_GraphicsGroupItem, bool bInitial = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bInitial true - при входе от пользователя в рекурсию.
	/// Подготовка отсылки параметров и удаление элемента (а так же добавка его группы в лист).
	static void DeleteElementAPFS(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	// Удаление линка.
	static void DeleteLinkAPFS(GraphicsLinkItem* p_GraphicsLinkItem, bool bFromElement = false, bool bRemoveFromClient = true);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \param[in] bFromElement Признак вызова из элемента.
							///< \param[in] bRemoveFromClient Флаг для удаления с клиента.
	/// Остоединение выбранного от группы и подготовка отправки всех изменеий на сервер.
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
	/// Поднятие элемента на первый план, блокировка и подготовка отсылки по запросу.
	static void ElementToTopOrBusyAPFS(GraphicsElementItem* p_Element, bool bAddElementGroupChange = false,
									   bool bAddBusyOrZPosToSending = true, bool bSend = true, bool bToTop = true);
							///< \param[in] p_Element Указатель на граф. элемент.
							///< \param[in] bAddElementGroupID При true - передача элементом изменения текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - установка флага занятости, иначе - отправка z-позиций.
							///< \param[in] bSend При true - отправка.
							///< \param[in] bToTop При true - поднятие на передний план на клиенте.
	/// Обновление фрейма группы по геометрии контента рекурсивно.
	static void UpdateGroupFrameByContentRecursively(GraphicsGroupItem* p_GraphicsGroupItem);
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
	/// Отпускание занятого клиентом.
	static void ReleaseOccupiedAPFS();
	/// Удаление всех графических элементов портов с элемента по ID.
	static void RemovePortsByID(unsigned long long ullID);
							///< \param[in] ullID ID элемента.
	/// Добавление свободных элементов в группу и подготовка к отправке по запросу.
	static void AddFreeSelectedElementsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem,
												   GraphicsElementItem* p_GraphicsElementItemInitial = nullptr);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] p_GraphicsGroupItem Указатель на инициирующий граф. элемент (по необходимости).
	///  Добавление свободных групп в группу и подготовка к отправке по запросу.
	static void AddFreeSelectedGroupsToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
	/// Выбор элемента.
	static void SelectElement(GraphicsElementItem* p_GraphicsElementItem, bool bLastState = true);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Отмена выбора элемента.
	static void DeselectElement(GraphicsElementItem* p_GraphicsElementItem, bool bLastState = true);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Сортировка векторов элементов и групп по Z-позиции с приоритетом по выборке.
	static void SortObjectsByZPos(QVector<GraphicsElementItem*>& avp_Elements, GraphicsElementItem* p_GraphicsElementItemExclude,
								  QVector<GraphicsGroupItem*>& avp_Groups, GraphicsGroupItem* p_GraphicsGroupItemExclude,
								  QVector<EGPointersVariant>* pv_EGPointersVariants);
							///< \param[in] avp_Elements Ссылка на вектор указателей на элементы или nullptr.
							///< \param[in] p_GraphicsElementItemExclude Указатель на элемент для исключения или nullptr.
							///< \param[in] avp_Groups Ссылка на вектор указателей на группы или nullptr.
							///< \param[in] p_GraphicsGroupItemExclude Указатель на элемент для исключения или nullptr.
							///< \param[in] pv_EGPointersVariants Указатель на вектор отсортированных вариантов для заполнения.
	/// Сортировка вектора групп по Z-позиции с приоритетом по выборке.
	static void SortGroupsByZPos(QVector<GraphicsGroupItem*>& avp_Groups,
								 GraphicsGroupItem* p_GraphicsGroupItemExclude,
								 QVector<GraphicsGroupItem*>* pvp_SortedGroups,
								 QVector<GraphicsGroupItem*>* pvp_SelectionSortedGroups = nullptr);
							///< \param[in] аvp_Groups Ссылка на вектор указателей на группы.
							///< \param[in] bWithSelectedDiff При true - вынос выбранных пользователем элементов на передний план.
							///< \param[in] p_GraphicsGroupItemExclude Указатель на группу для исключения или nullptr.
	/// Поднятие Z-значений ветки группы с фокусом и очистка списка.
	static void PullUpZOfBranch(QVector<GraphicsGroupItem*>& avp_GraphicsGroupItemsBranch);
							///< \param[in] avp_GraphicsGroupItemsBranch Ссылка на вектор с веткой групп.
	/// Блокировка вертикалей и поднятие выбранного элемента.
	static void BlockingVerticalsAndPopupElement(GraphicsElementItem* p_GraphicsElementItem, GraphicsGroupItem* p_GraphicsGroupItem = nullptr,
												 bool bSend = true, bool bAddNewElementsToGroupSending = false,
												 bool bAddNewGroupsToGroupSending = false,
												 bool bAddBusyOrZPosToSending = true, bool bAddFrame = false,
												 bool bSendElements = true, bool bAffectSelected = true);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер.
							///< \param[in] bAddNewElementsToGroupSending При true - передача сод. элементами параметра текущей группы.
							///< \param[in] bAddNewGroupsToGroupSending При true - передача содержащимеся группами параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - уст. флагов занятости, иначе - отпр. z-поз. (для нового содерж.)
							///< \param[in] bAddFrame При true - передача фреймов групп.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
							///< \param[in] bAffectSelected При true - обработка занятостей выбранных элементов кроме текущего.
	/// Блокировка вертикалей и поднятие выбранной группы.
	static void BlockingVerticalsAndPopupGroup(GraphicsGroupItem* p_GraphicsGroupItem,
											   bool bSend = true, bool bAddNewElementsToGroupSending = false,
											   bool bAddNewGroupsToGroupSending = false,
											   bool bAddBusyOrZPosToSending = true, bool bAddFrame = false, bool bSendElements = true);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер.
							///< \param[in] bAddNewElementsToGroupSending При true - передача содержащимеся элементами параметра текущей группы.
							///< \param[in] bAddNewGroupsToGroupSending При true - передача содержащимеся группами параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - уст. флагов занятости, иначе - отпр. z-поз. (для нового содерж.)
							///< \param[in] bAddFrame При true - передача фреймов групп.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
	// Поднятие ветки групп на первый план и подготовка к отсылке по запросу рекурсивно.
	static void GroupsBranchToTopAPFSRecursively(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend = true,
												 bool bAddNewElementsToGroupSending = false, bool bAddNewGroupsToGroupSending =  false,
												 bool bAddBusyOrZPosToSending = true, bool bAddFrame = false,
												 GraphicsElementItem* p_GraphicsElementItemExclude = nullptr,
												 GraphicsGroupItem* p_GraphicsGroupItemExclude = nullptr,
												 bool bSendElements = true, bool bToTop = true);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер.
							///< \param[in] bAddNewElementsToGroupSending При true - передача содержащимеся элементами параметра текущей группы.
							///< \param[in] bAddNewGroupsToGroupSending При true - передача содержащимеся группами параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - установка флага занятости, иначе - отправка z-позиций рекурсивно.
							///< \param[in] bAddFrame При true - передача фрейма группы.
							///< \param[in] p_GraphicsElementItemExclude Указатель на исключаемый элемент или nullptr.
							///< \param[in] p_GraphicsGroupItemExclude Указатель на исключаемую группу.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
							///< \param[in] bToTop При true - поднятие содержимого.
	/// Выбор группы.
	static void SelectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Отмена выбора группы.
	static void DeselectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Перемещение группы рекурсивно.
	static void MoveGroupRecursively(GraphicsGroupItem* p_GraphicsGroupItem, QPointF& a_QPointFRes, bool bMoveBody = true);
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
	/// Обработчик функции возврата вместилища элемента и его видов.
	static QRectF ElementBoundingHandler(const GraphicsElementItem* pc_GraphicsElementItem);
							///< \param[in] pc_GraphicsElementItem Указатель на элемент.
							///< \return Прямоугольник по размеру элемента.
	/// Обработчик функции возврата формы элемента и его видов.
	static QPainterPath ElementShapeHandler(const GraphicsElementItem* pc_GraphicsElementItem);
							///< \param[in] pc_GraphicsElementItem Указатель на элемент.
							///< \return Форма элемента.
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
	/// Создание группы с выбранным.
	static void CreateGroupFromSelected();
	/// Обработчик события отпусканеия мыши на группе.
	static void GroupMouseReleaseEventHandler(GraphicsGroupItem* p_GraphicsGroupItem, QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_Event Указатель на событие.
	/// Обработчик функции возврата вместилища группы.
	static QRectF GroupBoundingHandler(const GraphicsGroupItem* pc_GraphicsGroupItem);
							///< \param[in] pc_GraphicsGroupItem Указатель на группу.
							///< \return Прямоугольник по размеру группы.
	/// Обработчик функции рисования группы.
	static void GroupPaintHandler(GraphicsGroupItem* p_GraphicsGroupItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора группы.
	static void GroupConstructorHandler(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupBase* p_PSchGroupBase);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_PSchGroupBase Указатель на структуру с параметрами элемента для копии внутрь.

	/// Рекурсивное удаление пустых групп.
	static bool GroupCheckEmptyAndRemoveRecursively(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \return true, если было удаление.
	/// Обработчик функции возврата вместилища рамки.
	static QRectF FrameBoundingHandler(const GraphicsFrameItem* pc_GraphicsFrameItem);
							///< \param[in] GraphicsFrameItem Указатель на фрейм.
							///< \return Прямоугольник по размеру рамки.
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
	/// Обработчик функции возврата вместилища порта.
	static QRectF PortBoundingHandler();
							///< \return Прямоугольник по размеру порта.
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
	/// Обработчик функции возврата вместилища скалера и его видов.
	static QRectF ScalerBoundingHandler(const GraphicsScalerItem* pc_GraphicsScalerItem);
							///< \param[in] pc_GraphicsElementItem Указатель на скалер.
							///< \return Прямоугольник по размеру элемента.
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
	static void PrepareForRemoveElementFromScene(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
private:
	static bool bLMousePressed; ///< Признак нажатия на ЛКМ.
	static int iXInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по X.
	static int iYInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по Y.
	static CBSchematicViewFrameChanged pf_CBSchematicViewFrameChangedInt; ///< Указатель на кэлбэк изменения окна обзора.
	static QPointF pntMouseClickMapped; ///< Точка клика в координатах сцены.
	static QPointF pntMouseMoveMapped; ///< Точка смещения от клика в координатах сцены.
	qreal rScaleFactor; ///<
	qreal rFactor; ///<
	static QVector<GraphicsGroupItem*> v_AlreadyMovedGroups; ///< Список указателей на уже перемещённые за цикл группы.
	static QVector<EGPointersVariant> v_OccupiedByClient; ///< Список занятого клиентом.
	static QPolygonF oQPolygonFForRectScaler; ///< Объект полигона для отрисовки скалера прямоугольника.
	static QPolygonF oQPolygonFForTriangleScaler; ///< Объект полигона для отрисовки скалера треугольника.
	static double dbFrameDimIncSubCorr; ///< Служебная переменная размераотступа рамки минус коррекция.
	static double dbFrameDimIncTwiceSubCorr; ///< Служебная переменная двойного размера отступа рамки минус коррекция.
	static double dbFrameDimIncTwiceSubDoubleCorr; ///< Служебная переменная двойного размера отступа рамки минус двойная коррекция.
	static double dbFrameDimIncNegPlusCorr; ///< Служебная переменная отрицательного размера отступа рамки плюс коррекция.
	static double dbFrameDimIncNegPlusHalfCorr; ///< Служебная переменная отрицательного размера отступа рамки плюс пол коррекции.
	static double dbPortDimNeg; ///< Служебная переменная отрицательного размера порта.
	static double dbPortDimD; ///< Служебная переменная диаметра порта.

public:
	static GraphicsPortItem* p_GraphicsPortItemActive; ///< Указатель на текущий выбранный порт или nullptr.
	static QGraphicsRectItem* p_QGraphicsRectItemSelectionDash; ///< Прямоугольник выборки, линии.
	static QGraphicsRectItem* p_QGraphicsRectItemSelectionDot; ///< Прямоугольник выборки, точки.
	static bool bShiftAndLMBPressed; ///< Признак нажатия на Shift при клике ЛКМ.
	static GraphicsLinkItem* p_GraphicsLinkItemNew; ///< Указатель на новосозданный линк.
	static bool bPortAltPressed; ///< Флаг нажатого модификатора Alt для порта.
	static bool bPortLMBPressed; ///< Флаг нажатой ЛКМ для порта.
	static DbPoint oDbPointPortRB; ///< Точка правого нижнего края элемента для порта.
	static DbPoint oDbPointPortCurrent; ///< Текщая точка для порта.
	static DbPoint oDbPointPortOld; ///< Старая точка для порта.
	static DbPoint oDbPointPortInitialClick; ///< Точка нажатия для порта.
	static bool bPortFromElement; ///< Флаг запроса от элемента для порта.
	static bool bPortMenuExecuted; ///< Флаг выполненного меню для отмены ховера для порта.
	static constexpr QPointF pntTrR = QPointF(0.8660253746435073f, 0.5f); ///< Правая вершина треугольника.
	static constexpr QPointF pntTrT = QPointF(0.0, -1.0f); ///< Центральная вершина треугольника.
	static constexpr QPointF pntTrL = QPointF(-0.8660253746435073f, 0.5f); ///< Левая вершина треугольника.
	static constexpr double dbSqrtFromTwoDivByTwo = 0.7071067811865475244f; ///< Для определения 45гр. на окружности.
};

#endif // SCHEMATICVIEW_H
