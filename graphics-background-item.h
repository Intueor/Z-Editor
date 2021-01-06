#ifndef GRAPHICSBACKGROUNDITEM_H
#define GRAPHICSBACKGROUNDITEM_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsItem>

//== КЛАССЫ.
/// Класс отображения скалера.
class GraphicsBackgroundItem : public QGraphicsItem
{
public:
	/// Конструктор.
	explicit GraphicsBackgroundItem();
	/// Деструктор.
	~GraphicsBackgroundItem();
	/// Переопределение функции сообщения о вмещающем прямоугольнике.
	QRectF boundingRect() const override;
							///< \return Вмещающий прямоугольник.
	/// Переопределение функции рисования скалера.
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem* p_Option, QWidget* p_Widget) override;
							///< \param[in] p_Painter Указатель на отрисовщик.
							///< \param[in] p_Option Указатель на опции стиля.
							///< \param[in] p_Widget Указатель на виджет.
private:
	static QPen oQPenDarkGray; ///< Тёмно-серый карандаш.
};

#endif // GRAPHICSBACKGROUNDITEM_H
