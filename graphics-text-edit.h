#ifndef GRAPHICSTEXTEDIT_H
#define GRAPHICSTEXTEDIT_H

//== ВКЛЮЧЕНИЯ.
#include <QTextEdit>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class GraphicsTextEdit;
}

//== КЛАССЫ.
/// Класс адаптированного к сцене текстового редактора.
class GraphicsTextEdit : public QTextEdit
{
	Q_OBJECT
public:
	/// Конструктор 1.
	explicit GraphicsTextEdit(QWidget* p_Parent = nullptr);
								///< \param[in] p_Parent Указатель на родителя.
	/// Конструктор 2.
	explicit GraphicsTextEdit(const QString& a_strText, QWidget* p_Parent = nullptr);
								///< \param[in] a_strText Ссылка на инициализирующую строку.
								///< \param[in] p_Parent Указатель на родителя.
protected:
	/// Событие входа фокуса.
	void focusInEvent(QFocusEvent* p_Event);
								///< \param[in] p_Event Указатель на событие.
	/// Событие выхода фокуса.
	void focusOutEvent(QFocusEvent* p_Event);
								///< \param[in] p_Event Указатель на событие.
	/// Событие отрисовки.
	void paintEvent(QPaintEvent* p_Event);
								///< \param[in] p_Event Указатель на событие.
};

#endif // GRAPHICSTEXTEDIT_H
