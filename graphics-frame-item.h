#ifndef GRAPHICSFRAMEITEM_H
#define GRAPHICSFRAMEITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>
#include "../Z-Hub/logger.h"

//== КЛАССЫ.
/// Класс отображения фрейма.
class GraphicsFrameItem : public QGraphicsItem
{
public:
	/// Конструктор.
	explicit GraphicsFrameItem(unsigned short ushKindOfItem, GraphicsElementItem* p_ElementParent = nullptr,
							   GraphicsGroupItem* p_GroupParent = nullptr);
							///< \param[in] ushKindOfItem Тип родителя.
							///< \param[in] p_ElementParent Указатель на родитель-элемент.
							///< \param[in] p_GroupParent Указатель на родитель-группу.
	/// Переопределение функции сообщения о вмещающем прямоугольнике.
	QRectF boundingRect() const override;
							///< \return Вмещающий прямоугольник.
	/// Переопределение функции рисования фрейма.
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget) override;
							///< \param[in] p_Painter Указатель на отрисовщик.
							///< \param[in] p_Option Указатель на опции стиля.
							///< \param[in] p_Widget Указатель на виджет.

protected:
	/// Переопределение функции шага событий элемента.
	void advance(int iStep) override;
							///< \param[in] iStep Текущий шаг сцены.
private:
	LOGDECL_MULTIOBJECT
	LOGDECL_PTHRD_INCLASS_ADD
	GraphicsElementItem* p_ElementParentInt; ///< Внутренний указатель на родитель-элемент.
	GraphicsGroupItem* p_GroupParentInt; ///< Внутренний указатель на родитель-элемент.
	unsigned short ushKindOfItemInt; ///< Внутренняя переменная типа родителя.
};

#endif // GRAPHICSFRAMEITEM_H
