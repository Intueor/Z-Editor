//== ВКЛЮЧЕНИЯ.
#include <QTextEdit>
#include "graphics-text-edit.h"

//== ФУНКЦИИ КЛАССОВ.
//== Класс адаптированного к сцене текстового редактора.
// Конструктор 1.
GraphicsTextEdit::GraphicsTextEdit(QWidget* p_Parent) : QTextEdit(p_Parent) {}

// Конструктор 2.
GraphicsTextEdit::GraphicsTextEdit(const QString& a_strText, QWidget* p_Parent) : QTextEdit(a_strText, p_Parent) {}

// Событие входа фокуса.
void GraphicsTextEdit::focusInEvent(QFocusEvent* p_Event)
{
	this->grabKeyboard();
	QTextEdit::focusInEvent(p_Event);
}

// Событие входа фокуса.
void GraphicsTextEdit::focusOutEvent(QFocusEvent* p_Event)
{
	this->releaseKeyboard();
	QTextEdit::focusOutEvent(p_Event);
}

