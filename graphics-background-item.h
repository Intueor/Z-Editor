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
	/// Переопределение функции определения формы.
	QPainterPath shape() const override;
							///< \return Фигура.
	/// Переопределение функции рисования скалера.
	void paint(QPainter* p_Painter, const QStyleOptionGraphicsItem*, QWidget*) override;
							///< \param[in] p_Painter Указатель на отрисовщик.
private:
	static QPen oQPenDarkGray; ///< Тёмно-серый карандаш.
};

#endif // GRAPHICSBACKGROUNDITEM_H
