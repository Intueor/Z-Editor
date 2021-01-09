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
class GraphicsBackgroundItem;

//== КЛАССЫ.
/// Класс виджета обзора.
class SchematicView : public QGraphicsView
{
	Q_OBJECT

private:
	struct DbPointPair
	{
		DbPoint dbSrc;
		DbPoint dbDst;
	};
	struct CalcPortHelper
	{
		unsigned char uchSrcCorner;
		unsigned char uchDstCorner;
		DbPoint oDbPointWH;
		DbPointPair oDbPointPairPortsCoords;
	};
	struct EGPointersVariant
	{
		GraphicsElementItem* p_GraphicsElementItem;
		GraphicsGroupItem* p_GraphicsGroupItem;
	};
	struct GroupBranchSegment
	{
		unsigned int uiDeepth;
		GraphicsGroupItem* p_GraphicsGroupItem;
	};
	struct GroupsBranch
	{
		GraphicsGroupItem* p_GraphicsGroupItemRoot;
		QVector<GroupBranchSegment> v_GroupBranchSegments;
	};

public:
	/// Конструктор.
	explicit SchematicView(QWidget* parent = nullptr);
							///< \param[in] parent Указатель на родительский виджет.
	/// Деструктор.
	~SchematicView();
	/// Установка позиции подложки.
	static void SetBackgroundPos();
	/// Переопределение функции обработки перемещения мыши.
	void mouseMoveEvent(QMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
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
	static QRectF ElementBoundingHandler(const GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \return Прямоугольник по размеру элемента.
	/// Обработчик функции возврата формы элемента и его видов.
	static QPainterPath ElementShapeHandler(const GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
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
	static QRectF GroupBoundingHandler(const GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \return Прямоугольник по размеру группы.
	/// Обработчик функции рисования группы.
	static void GroupPaintHandler(GraphicsGroupItem* p_GraphicsGroupItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик конструктора группы.
	static void GroupConstructorHandler(GraphicsGroupItem* p_GraphicsGroupItem, PSchGroupBase* p_PSchGroupBase);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] p_PSchGroupBase Указатель на структуру с параметрами элемента для копии внутрь.
	/// Обработчик функции возврата вместилища рамки.
	static QRectF FrameBoundingHandler(const GraphicsFrameItem* p_GraphicsFrameItem);
							///< \param[in] p_GraphicsFrameItem Указатель на фрейм.
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
	static QRectF ScalerBoundingHandler(const GraphicsScalerItem* p_GraphicsScalerItem);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \return Прямоугольник по размеру элемента.
	/// Обработчик функции рисования скалера.
	static void ScalerPaintHandler(GraphicsScalerItem* p_GraphicsScalerItem, QPainter* p_Painter);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] p_Painter Указатель на отрисовщик.
	/// Обработчик функции возврата формы скалера и его видов.
	static QPainterPath ScalerShapeHandler(const GraphicsScalerItem* p_GraphicsScalerItem);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \return Форма скалера.
	/// Обработчик конструктора скалера.
	static void ScalerConstructorHandler(GraphicsScalerItem* p_GraphicsScalerItem, GraphicsElementItem* p_Parent);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] p_Parent Указатель на родителя.
	/// Вычисление квадрата и вместилища линии линка.
	static CalcPortHelper CalcLinkLineWidthHeight(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \return Вмещающий прямоугольник и точки портов.
	/// Расстановка размеров, статусов групп и их содержания после загрузки.
	static void AfterLoadingPlacement();
	/// Установка паттерна блокировки на элемент.
	static void SetElementBlockingPattern(GraphicsElementItem* p_GraphicsElementItem, bool bValue);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] bValue Вкл\Выкл блокировки.
	/// Обновление фрейма группы по геометрии контента рекурсивно вверх.
	static void UpdateGroupFrameByContentRecursivelyUpstream(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на корректируемую группу.
	/// Обновление Z-позиции линка по данным элементов.
	static void UpdateLinkZPositionByElements(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на граф. линк.
	/// Установка паттерна блокировки на группу.
	static void SetGroupBlockingPattern(GraphicsGroupItem* p_GraphicsGroupItem, bool bValue);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. линк.
							///< \param[in] bValue Вкл\Выкл блокировки.
	/// Рекурсивное удаление пустых групп.
	static void GroupCheckEmptyAndRemoveRecursively(GraphicsGroupItem* p_GraphicsGroupItem, bool bRemoveFromSelectedVec = false);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bRemoveFromSelectedVec При true - удаление удаляемых групп из вектора выбранных (выз. из отсоедин.).
	/// Удаление всех графических элементов портов с элемента по ID.
	static void RemovePortsByID(unsigned long long ullID);
							///< \param[in] ullID ID элемента.
	/// Определение области видимости.
	static QRectF GetVisibleRect();
							///< \return Структура определения прямоугольника области видимости.
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
	/// Установка указателя кэлбэка изменения окна обзора.
	static void SetSchematicViewFrameChangedCB(CBSchematicViewFrameChanged pf_CBSchematicViewFrameChanged);
								///< \param[in] pf_CBSchematicViewFrameChanged Указатель на пользовательскую функцию.
	/// Создание подложки.
	static void CreateBackground();
public slots:
	/// Обновление от таймера мерцания выбранных элементов.
	static void UpdateSelectionFlash();

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
	/// Расформирование выбранных групп и подготовка отправки всех изменеий на сервер.
	static void DisbandSelectedGroupsAPFS();
	/// Остоединение выбранного от группы и подготовка отправки всех изменеий на сервер.
	static void DetachSelectedAPFS();
	/// Подготовка всех фреймов групп дерева к отправке рекурсивно.
	static void GetTreeGroupFramesRecursivelyAPFS(GraphicsGroupItem* p_GraphicsGroupItemRoot);
							///< \param[in] p_GraphicsGroupItem Указатель на группу-корень.
	/// Подготовка всех фреймов групп дерева к отправке рекурсивно.
	static void GetTreesOfGroupFramesAPFS(QVector <GraphicsGroupItem*> &avp_GraphicsGroupItemsRoots);
							///< \param[in] avp_GraphicsGroupItemsRoots Ссылка на вектор с деревьями.
	/// Удаление выбранного и подготовка отправки по запросу.
	static void DeleteSelectedAPFS();
	/// Создание нового элемента и подготовка отсылки параметров.
	static GraphicsElementItem* CreateNewElementAPFS(char* p_chName, QPointF pntMapped,
													 unsigned long long ullIDGroup = 0, unsigned char uchSettings = 0);
							///< \param[in] p_chName Указатель на строку именем.
							///< \param[in] pntMapped Координаты.
							///< \param[in] ullIDGroup ИД группы (привязка и сопутствующие операции - в CreateNewElementInGroupAPFS).
							///< \param[in] uchSettings Установки типа, минимизации, занятости (по протоколу).
							///< \return Указатель на новый графический элемент.
	/// Создание нового элемента в группе и подготовка отсылки параметров.
	static void CreateNewElementInGroupAPFS(char* p_chName, GraphicsGroupItem* p_GraphicsGroupItem,
											QPointF pntMapped, unsigned char uchSettings = 0);
							///< \param[in] p_chName Указатель на строку именем.
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] pntMapped Координаты.
							///< \param[in] uchSettings Установки типа, минимизации, занятости (по протоколу).
	/// Обновление Z-позиции линков.
	static void UpdateLinksZPos();
	/// Замена линка.
	static bool ReplaceLink(GraphicsLinkItem* p_GraphicsLinkItem, GraphicsElementItem* p_GraphicsElementItem, bool bIsSrc,
							DbPoint oDbPortPos, bool bFromElement);
							///< \param[in] p_GraphicsLinkItem Указатель на старый линк.
							///< \param[in] p_GraphicsElementItem Указатель на элемент, на который будет создан новый линк.
							///< \param[in] bIsSrc При true - новый элемент будет источником.
							///< \param[in] oDbPortPos Координаты порта в коорд. элемента на момент обращения.
							///< \param[in] bFromElement Флаг запроса работы с портом от элемента (при создании нового линка).
							///< \return true при удаче.
	/// Пересчёт позиции граф. порта к краям элемента.
	static DbPoint BindToEdge(GraphicsElementItem* p_GraphicsElementItem, DbPoint oDbPortPos);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] oDbPortPos Позиция порта в коорд. элемента.
							///< \return Позиция порта на крае элемента в координатах элемента.
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
	/// Отпускание занятого клиентом.
	static void ReleaseOccupiedAPFS();
	/// Добавление выбранного в группу и подготовка к отправке по запросу.
	static void AddFreeSelectedToGroupAPFS(GraphicsGroupItem* p_GraphicsGroupItem);
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
												 bool bSend = true, bool bAddNewElementsToGroupsRelationSending = false,
												 bool bAddNewGroupsToGroupsRelationSending = false,
												 bool bAddBusyOrZPosToSending = true, bool bAddFrame = false,
												 bool bSendElements = true, bool bAffectSelected = true);
							///< \param[in] p_GraphicsElementItem Указатель на граф. элемент.
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер.
							///< \param[in] bAddNewElementsToGroupsRelationSending При true - передача сод. элементами параметра текущей группы.
							///< \param[in] bAddNewGroupsToGroupsRelationSending При true - передача содержащимеся гр.параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - уст. флагов занятости, иначе - отпр. z-поз. (для нового содерж.)
							///< \param[in] bAddFrame При true - передача фреймов групп.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
							///< \param[in] bAffectSelected При true - обработка занятостей выбранных элементов кроме текущего.
	/// Блокировка вертикалей и поднятие выбранной группы.
	static void BlockingVerticalsAndPopupGroup(GraphicsGroupItem* p_GraphicsGroupItem,
											   bool bSend = true, bool bAddNewElementsToGroupsRelationSending = false,
											   bool bAddNewGroupsToGroupsRelationSending = false,
											   bool bAddBusyOrZPosToSending = true, bool bAddFrame = false, bool bSendElements = true);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер.
							///< \param[in] bAddNewElementsToGroupsRelationSending При true - передача содержащимеся эл. пар. текущей группы.
							///< \param[in] bAddNewGroupsToGroupsRelationSending При true - передача содержащимеся гр. параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - уст. флагов занятости, иначе - отпр. z-поз. (для нового содерж.)
							///< \param[in] bAddFrame При true - передача фреймов групп.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
	// Поднятие ветки групп на первый план и подготовка к отсылке по запросу рекурсивно.
	static void GroupsBranchToTopAPFSRecursively(GraphicsGroupItem* p_GraphicsGroupItem, bool bSend = true,
												 bool bAddNewElementsToGroupsRelationSending = false,
												 bool bAddNewGroupsToGroupsRelationSending = false,
												 bool bAddBusyOrZPosToSending = true, bool bAddFrame = false,
												 GraphicsElementItem* p_GraphicsElementItemExclude = nullptr,
												 GraphicsGroupItem* p_GraphicsGroupItemExclude = nullptr,
												 bool bSendElements = true, bool bToTop = true,
												 int iGroupsToGroupsGenerationsBeforeExclude = 0);
							///< \param[in] p_GraphicsGroupItem Указатель на граф. группу.
							///< \param[in] bSend При true - отправка на сервер.
							///< \param[in] bAddNewElementsToGroupsRelationSending При true - передача содержащимеся эл. пар. текущей группы.
							///< \param[in] bAddNewGroupsToGroupsRelationSending При true - передача содержащимеся гр. параметра текущей группы.
							///< \param[in] bAddBusyOrZPosToSending При true - установка флага занятости, иначе - отправка z-позиций рекурсивно.
							///< \param[in] bAddFrame При true - передача фрейма группы.
							///< \param[in] p_GraphicsElementItemExclude Указатель на исключаемый элемент или nullptr.
							///< \param[in] p_GraphicsGroupItemExclude Указатель на исключаемую группу.
							///< \param[in] bSendElements При true - отправка поднятых и отсортированных элементов.
							///< \param[in] bToTop При true - поднятие содержимого.
							///< \param[in] iGroupsToGroupsGenerationsBeforeExclude Количство поколений вглубь ветки до отсечки отправки завис.
	/// Выбор группы.
	static void SelectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Отмена выбора группы.
	static void DeselectGroup(GraphicsGroupItem* p_GraphicsGroupItem, bool bLastState = true);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] bLastState Последнее значение выбранности.
	/// Обновление вертикали фреймов групп рекурсивно.
	static void UpdateVerticalOfGroupFramesRecursively(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на стартовую группу.
	/// Перемещение группы рекурсивно.
	static void MoveGroupRecursively(GraphicsGroupItem* p_GraphicsGroupItem, QPointF& a_QPointFRes, bool bMoveBody = true);
							///< \param[in] p_GraphicsGroupItem Указатель на перемещаемую группу.
							///< \param[in] a_QPointFRes Ссылка на смещение для группы.
							///< \param[in] bMoveBody При true - смещать и тело группы.
	/// Обновление позиции линка по данным элементов.
	static void UpdateLinkPositionByElements(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на граф. линк.
	/// Установка порта в позицию.
	static void SetPortToPos(GraphicsPortItem* p_GraphicsPortItem, DbPoint dbPortPos);
							///< \param[in] p_GraphicsPortItem Указатель на граф. порт.
							///< \param[in] dbPortPos Позиция порта в коорд. элемента.
	/// Проверка наличия портов в выборке и по элементу (опционально).
	static bool CheckPortsInSelection(GraphicsElementItem* p_GraphicsElementItem = nullptr);
							///< \param[in] p_GraphicsElementItem Указатель на элемент или nullprt.
							///< \return true - при обнаружении.
	/// Вычисление точек портов в координатах сцены.
	static DbPointPair CalcPortsSceneCoords(GraphicsLinkItem* p_GraphicsLinkItem);
							///< \param[in] p_GraphicsLinkItem Указатель на линк.
							///< \return Точки портов.
	/// Вычисление формы скалера окружности.
	static QPainterPath CalcCicrleScalerPath(const GraphicsScalerItem* p_GraphicsScalerItem);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \return Форма скалера.
	/// Подготовка удаления графического элемента из сцены и группы, возврат флага на удаление группы элемента.
	static void PrepareForRemoveElementFromScene(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	/// Действия при совместном нажатии кнопок мыши.
	static bool DoubleButtonsPressControl(QGraphicsSceneMouseEvent* p_Event);
							///< \param[in] p_Event Указатель на событие.
							///< \return true при совместном нажатии.
	/// Действия при отпускании кнопок мыши после совместного нажатия.
	static bool DoubleButtonsReleaseControl();
							///< \return true при снятии совместного нажатия.
	/// Рекурсивные операции по минимизации группы.
	static void GroupMinOperationsRecursively(GraphicsGroupItem* p_GraphicsGroupItem, bool bNextHiding = false, bool bHiderFound = false);
							///< \param[in] p_GraphicsGroupItem Указатель на группу - корень (для пользователя).
							///< \param[in] bNextHiding Служебная для рекурсии - статус скрытия следующей группы.
							///< \param[in] bHiderFound Служебная для рекурсии - true - найдена скрывающая группа.
	/// Установка портов групп после смены статуса минимизации.
	static void SetPortsPlacementAfterGroupsMinChanges();
	/// Путешествие по ветке группы до корня.
	static GraphicsGroupItem* GetRootOfGroup(GraphicsGroupItem* p_GraphicsGroupItemParent);
							///< \param[in] p_GraphicsGroupItem Указатель на группу-родителя элемента или группы.
							///< \return Указатель на корень.
	/// Проверка на включённость в состав групп в выборке.
	static bool TestSelectedForNesting(GraphicsGroupItem* p_GraphicsGroupItemExclude = nullptr);
							///< \param[in] p_GraphicsGroupItemExclude Указатель на группу для исключения из поиска.
	/// Работа с диалогом пакетного переименования файлов.
	static void BacthRenameDialogProcedures();
	/// Вызов диалога смены цвета.
	static unsigned int GetColor(unsigned int uiLastColor);
							///< \param[in] uiLastColor Начальный цвет.
							///< \return Цвет.
	/// Задание цвета выборке и подготовка отправки на сервер.
	static void ChangeColorOfSelectedAPFS(unsigned int uiNewColor);
							///< \param[in] uiLastColor Цвет.
	/// Определение типа палитры подложки.
	static bool CheckBkgPaletteType(unsigned int uiColor);
							///< \param[in] uiColor Цвет.
							///< \return true - при позитивной палитре.
	/// Установка стиля элемента в зависимости от типа палитры.
	static void SetElementPalette(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	/// Установка стиля группы в зависимости от типа палитры.
	static void SetGroupPalette(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
	/// Установка тултипа в зависимости от типа элемента.
	static void SetElementTooltip(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	/// Подготовка имени внешнего порта.
	static void PrepareNameWithExtPort(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	/// Установка временного стиля кистей общего пользования.
	static void SetTempBrushesStyle(Qt::BrushStyle iStyle);
							///< \param[in] iStyle Стиль.
	/// Отмена временного стиля кистей общего пользования.
	static void RestoreBrushesStyles();
	/// Вычисление смещения для прилипания к сетке.
	static DbPoint CalcSnapShift(double dbX, double dbY);
							///< \param[in] dbX Вхоные Х.
							///< \param[in] dbX Вхоные Y.
							///< \return Смещение.
	/// Смещение групп и содержимого рекурсивно.
	static void ShiftGroupWithContentRecursively(GraphicsGroupItem* p_GraphicsGroupItem, DbPoint oDbPointShift);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
							///< \param[in] oDbPointShift Смещение.
	/// Коррекция позиции элемента по сетке.
	static void ElementSnapCorrection(GraphicsElementItem* p_GraphicsElementItem);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
	/// Коррекция позиции группы по сетке.
	static void GroupSnapCorrection(GraphicsGroupItem* p_GraphicsGroupItem);
							///< \param[in] p_GraphicsGroupItem Указатель на группу.
	/// Процедуры обработки скалирования.
	static void ScalingProcedures(GraphicsScalerItem* p_GraphicsScalerItem, DbPoint oDbPointPosNow, DbPoint oDbPointPosPrev);
							///< \param[in] p_GraphicsScalerItem Указатель на скалер.
							///< \param[in] oDbPointPosNow Текущее положение скалера.
							///< \param[in] oDbPointPosPrev Старое положение скалера (до обработанного события).
	/// Просчёт вектора Безье-отклонения линка к левому верхнему углу.
	static DbPoint LinkAttachCalcLT(unsigned char uchPortOrientation, DbPoint& a_DbPointWH);
							///< \param[in] uchPortOrientation Ориентацмя порта.
							///< \param[in] a_DbPointWH Ссылка на размер бокса линка.
							///< \return Вектор.
	/// Просчёт вектора Безье-отклонения линка к правому нижнему углу.
	static DbPoint LinkAttachCalcRB(unsigned char uchPortOrientation, DbPoint& a_DbPointWH);
							///< \param[in] uchPortOrientation Ориентацмя порта.
							///< \param[in] a_DbPointWH Ссылка на размер бокса линка.
							///< \return Вектор.
	/// Просчёт вектора Безье-отклонения линка к левому нижнему углу.
	static DbPoint LinkAttachCalcLB(unsigned char uchPortOrientation, DbPoint& a_DbPointWH);
							///< \param[in] uchPortOrientation Ориентацмя порта.
							///< \param[in] a_DbPointWH Ссылка на размер бокса линка.
							///< \return Вектор.
	/// Просчёт вектора Безье-отклонения линка к правому вехнему углу.
	static DbPoint LinkAttachCalcRT(unsigned char uchPortOrientation, DbPoint& a_DbPointWH);
							///< \param[in] uchPortOrientation Ориентацмя порта.
							///< \param[in] a_DbPointWH Ссылка на размер бокса линка.
							///< \return Вектор.
	/// Получение ориентации порта на элементе.
	static unsigned char GetPortOnElementOrientation(GraphicsElementItem* p_GraphicsElementItem, DbPoint& a_DbPortGraphPos);
							///< \param[in] p_GraphicsElementItem Указатель на элемент.
							///< \param[in] a_DbPortGraphPos Ссылка на координата порта в коорд. элемента.
							///< \return Ориентация (гор.\верт.).

public:
	static bool bLoading; ///< Флаг блокировки отрисовок при загрузке сцены.
	static GraphicsPortItem* p_GraphicsPortItemActive; ///< Указатель на текущий выбранный порт или nullptr.
	static qreal dbObjectZPos; ///< Крайняя Z-позиция.
	static GraphicsBackgroundItem* p_GraphicsBackgroundItemInt; ///< Указатель на подкладку.
	static double dbSnapStep; ///< Шаг залипания к сетке.

private:
	static QBrush oQBrushDark; ///< Чёрная кисть общего пользования.
	static QBrush oQBrushLight; ///< Белая кисть общего пользования.
	static QBrush oQBrushGray; ///< Серая кисть общего пользования.
	static QPen oQPenWhite; ///< Белый карандаш общего пользования.
	static QPen oQPenBlack; ///< Чёрный карандаш общего пользования.
	static QPen oQPenWhiteTransparent; ///< Белый полупрозрачный карандаш общего пользования.
	static QPen oQPenBlackTransparent; ///< Чёрный полупрозрачный карандаш общего пользования.
	static QPen oQPenElementFrameFlash; ///< Карандаш мерцания рамки элемента общего пользования.
	static QPen oQPenGroupFrameFlash; ///< Карандаш мерцания рамки группы общего пользования.
	static QPen oQPenPortFrameFlash; ///< Карандаш мерцания рамки порта общего пользования.
	static QPen oQPenSelectionDash; ///< Карандаш прямоугольной выборки, линии.
	static QPen oQPenSelectionDot; ///< Карандаш прямоугольной выборки, точки.
	static Qt::BrushStyle iLStyle, iDStyle, iGStyle; ///< Внутренние переменные для хранения основных стилей кистей при замене на временные.
	static unsigned char uchElementSelectionFlashCounter; ///< Счётчик таймера мерцания выбранных элементов.
	static unsigned char uchGroupSelectionFlashCounter; ///< Счётчик таймера мерцания выбранных групп.
	static unsigned char uchPortSelectionFlashCounter; ///< Счётчик таймера мерцания выбранного порта.
	static QTimer oQTimerSelectionFlashing; ///< Таймер мерцания выбранных элементов.
	static GraphicsFrameItem* p_GraphicsFrameItemForPortFlash; ///< Указатель на рамку порта под курсором для мигания или nullptr.
	static int iXInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по X.
	static int iYInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по Y.
	static CBSchematicViewFrameChanged pf_CBSchematicViewFrameChangedInt; ///< Указатель на кэлбэк изменения окна обзора.
	static QPointF pntMouseClickMapped; ///< Точка клика в координатах сцены.
	static QPointF pntMouseMoveMapped; ///< Точка смещения от клика в координатах сцены.
	static QVector<GraphicsGroupItem*> v_AlreadyMovedGroups; ///< Список указателей на уже перемещённые за цикл группы.
	static QVector<EGPointersVariant> v_OccupiedByClient; ///< Список занятого клиентом.
	static QPolygonF oQPolygonFForRectScaler; ///< Объект полигона для отрисовки скалера прямоугольника.
	static QPolygonF oQPolygonFForTriangleScaler; ///< Объект полигона для отрисовки скалера треугольника.
	static double dbFrameDimIncSubCorr; ///< Служебная переменная размера отступа рамки минус коррекция.
	static double dbFrameDimIncTwiceSubCorr; ///< Служебная переменная двойного размера отступа рамки минус коррекция.
	static double dbFrameDimIncTwiceSubDoubleCorr; ///< Служебная переменная двойного размера отступа рамки минус двойная коррекция.
	static double dbFrameDimIncNegPlusCorr; ///< Служебная переменная отрицательного размера отступа рамки плюс коррекция.
	static double dbFrameDimIncNegPlusHalfCorr; ///< Служебная переменная отрицательного размера отступа рамки плюс пол коррекции.
	static double dbPortDimNeg; ///< Служебная переменная отрицательного размера порта.
	static double dbPortDimD; ///< Служебная переменная диаметра порта.
	static double dbMinTriangleR; ///< Служебная переменная радиуса минимизированного транслятора.
	static double dbMinElementR; ///< Служебная переменная радиуса минимизированного элемента.
	static double dbMinElementD; ///< Служебная переменная диаметра минимизированного элемента.
	static double dbMinGroupD; ///< Служебная переменная диаметра минимизированной группы.
	static double dbMinGroupR; ///< Служебная переменная радиуса минимизированной группы.
	static double dbMinCircleR; ///< Служебная переменная радиуса минимизированного приёмника.
	static double dbMinCircleD; ///< Служебная переменная диаметра минимизированного приёмника.
	static double dbMinTriangleDerc; ///< Служебная переменная коррекции положения минимизированного транслятора.
	static double dbMinTriangleRSubMinTriangleDerc; ///< Служебная переменная положения порта на минимизированном трансляторе.
	static double dbMinCircleRPlusFrameDimIncSubCorr; ///< Служебная радиуса миним. эл. плюс переменная размера отступа рамки минус коррекция.
	static double dbMinElementDPlusFrameDimIncTwiceSubDoubleCorr; ///< Сл. миним. диам. эл. плюс разм. отступа рамки минус двойная коррекция.
	static constexpr QPointF pntTrR = QPointF(0.8660253746435073f, 0.5f); ///< Правая вершина треугольника.
	static constexpr QPointF pntTrT = QPointF(0.0, -1.0f); ///< Центральная вершина треугольника.
	static constexpr QPointF pntTrL = QPointF(-0.8660253746435073f, 0.5f); ///< Левая вершина треугольника.
	static QPointF pntMinTrR; ///< Правая вершина минимизированного треугольника.
	static QPointF pntMinTrT; ///< Центральная вершина минимизированного треугольника.
	static QPointF pntMinTrL; ///< Левая вершина минимизированного треугольника.
	static QPointF pntMinFrameTrR; ///< Правая вершина минимизированного треугольника рамки.
	static QPointF pntMinFrameTrT; ///< Центральная вершина минимизированного треугольника рамки.
	static QPointF pntMinFrameTrL; ///< Левая вершина минимизированного треугольника рамки.
	static constexpr double dbSqrtFromTwoDivByTwo = 0.7071067811865475244f; ///< Для определения 45гр. на окружности.
	static QVector<GraphicsPortItem*> pv_GraphicsPortItemsCollected; ///< Вектор портов из пройденных элементов при смене статуса минимизации.
	static QVector<GraphicsElementItem*> vp_SelectedForDeleteElements; ///< Вектор с указателями на выбранные для удаления граф. элементы.
	static QVector<GraphicsGroupItem*> vp_SelectedForDeleteGroups; ///< Вектор с указателями на выбранные для удаления граф. группы.
	static QGraphicsRectItem* p_QGraphicsRectItemSelectionDash; ///< Прямоугольник выборки, линии.
	static QGraphicsRectItem* p_QGraphicsRectItemSelectionDot; ///< Прямоугольник выборки, точки.
	static bool bShiftAndLMBPressed; ///< Признак нажатия на Shift при клике ЛКМ.
	static GraphicsLinkItem* p_GraphicsLinkItemNew; ///< Указатель на новосозданный линк.
	static bool bPortAltPressed; ///< Флаг нажатого модификатора Alt для порта.
	static bool bLMBPressed; ///< Флаг нажатой ЛКМ.
	static bool bViewLMBPressed; ///< Флаг нажатой ЛКМ на подложке.
	static bool bFieldMenuReady; ///< Флаг готового меню на подложке.
	static bool bElementMenuReady; ///< Флаг готового меню для элемента.
	static bool bGroupMenuReady; ///< Флаг готового меню для группы.
	static bool bPortMenuReady; ///< Флаг готового меню для порта.
	static bool bPortMenuInExecution; ///< Флаг исполняющегося меню для порта.
	static bool bRMBPressed; ///< Флаг нажатой ПКМ.
	static DbPoint oDbPointPortRB; ///< Точка правого нижнего края элемента для порта.
	static DbPoint oDbPointPortCurrent; ///< Текщая точка для порта.
	static DbPoint oDbPointPortInitialClick; ///< Точка нажатия для порта.
	static bool bPortFromElement; ///< Флаг запроса от элемента для порта.
	static bool bPortMenuExecuted; ///< Флаг выполненного меню для отмены ховера для порта.
	static unsigned char uchWheelMul; ///< Позиция колёсика мыши от начала работы.
	static QVector<double> v_dbSnaps; ///< Список шагов сетки.
};

#endif // SCHEMATICVIEW_H
