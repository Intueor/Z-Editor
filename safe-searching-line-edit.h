#ifndef SafeSearchingLineEdit_H
#define SafeSearchingLineEdit_H

//== ВКЛЮЧЕНИЯ.
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class SafeSearchingLineEdit;
}

//== КЛАССЫ.
/// Класс безопасной строки поиска.
class SafeSearchingLineEdit : public QLineEdit
{
	Q_OBJECT
public:
	/// Конструктор 1.
	explicit SafeSearchingLineEdit(QWidget* p_parent = nullptr);
	/// Конструктор 2.
	explicit SafeSearchingLineEdit(const QString& a_strText, QWidget* p_parent = nullptr);
protected:
	/// Событие входа фокуса.
	void focusInEvent(QFocusEvent* p_Event);
	/// Событие выхода фокуса.
	void focusOutEvent(QFocusEvent* p_Event);
private slots:
	// Выключение автовыбора кнопки.
	void SetButtonSetNotDefault();
	// Выключение доступа к кнопоке.
	void SetButtonSetDisabled();
	// Восстановление автовыбора кнопки.
	void SetButtonRestoreDefault();
	// Восстановление доступа к кнопоке.
	void SetButtonRestoreEnabled();
private:
	bool bLastButtDefStatus; ///< Флаг предыдущего статуса автовыбора кнопки.
	bool bLastButtEnStatus; ///< Флаг предыдущего статуса доступности кнопки.
	QTimer oQTimer; ///< Таймер для установки статусов кнопок.

public:
	QPushButton* p_QPushButtonForNotDefault; ///< Указатель на кнопку для снятия автовыбора.
	QPushButton* p_QPushButtonForDisable; ///< Указатель на кнопку для блокировки.
	bool bUserInside; ///< Признак работы пользователя.
};

#endif // SafeSearchingLineEdit_H
