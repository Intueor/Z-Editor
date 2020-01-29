#ifndef SCHEMATICVIEW_H
#define SCHEMATICVIEW_H

//== ВКЛЮЧЕНИЯ.
#include <QGraphicsView>
#include <QWheelEvent>

//== ОПРЕДЕЛЕНИЯ ТИПОВ.
typedef void (*CBSchematicViewFrameChanged)(QRectF oQRectFVisibleFrame);

//== ПРЕД-ДЕКЛАРАЦИИ.

//== КЛАССЫ.
/// Класс виджета обзора.
class SchematicView : public QGraphicsView
{
	Q_OBJECT

public:
	/// Конструктор.
	explicit SchematicView(QWidget* parent = nullptr);
							///< \param[in] parent Указатель на родительский виджет.
	/// Определение области видимости.
	QRectF GetVisibleRect();
							///< \return Структура определения прямоугольника области видимости.

protected:

private:

private:
	static bool bLMousePressed; ///< Признак нажатия на ЛКМ.
	static int iXInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по X.
	static int iYInt; ///< Внутреннее хранилище коорд. перетаскиваения вида по Y.
	qreal rScaleFactor; ///<
	qreal rFactor; ///<
};

#endif // SCHEMATICVIEW_H
