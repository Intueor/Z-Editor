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
	explicit SafeSearchingLineEdit(QWidget* p_Parent = nullptr);
								///< \param[in] p_Parent Указатель на родителя.
	/// Конструктор 2.
	explicit SafeSearchingLineEdit(const QString& a_strText, QWidget* p_Parent = nullptr);
								///< \param[in] a_strText Ссылка на инициализирующую строку.
								///< \param[in] p_Parent Указатель на родителя.
	/// Изменение запомненного статуса включённости кнопки по указателю в процессе редактирования.
	void SetRecordedButtonEnabledStatus(QPushButton* p_QPushButton, bool bStatus);
								///< \param[in] p_QPushButton Указатель на кнопку.
								///< \param[in] bStatus Требуемый статус.
	/// Проверка текущего статуса редактирования.
	bool IsUserInside();
								///< \return true - внури.
protected:
	/// Событие входа фокуса.
	void focusInEvent(QFocusEvent* p_Event);
								///< \param[in] p_Event Указатель на событие.
	/// Событие выхода фокуса.
	void focusOutEvent(QFocusEvent* p_Event);
								///< \param[in] p_Event Указатель на событие.
private slots:
	// Выключение автовыбора кнопки.
	void SetButtonNotDefaultus();
	// Выключение доступа к кнопокам.
	void SetButtonsDisabled();
	// Восстановление автовыбора кнопки.
	void RestoreButtonDefault();
	// Восстановление доступа к кнопокам.
	void RestoreButtonsEnabled();

private:
	bool bUserInside; ///< Признак работы пользователя.
	bool bLastButtDefStatus; ///< Флаг предыдущего статуса автовыбора кнопки.
	QVector<bool> vLastButtEnStatuses; ///< Список флагов предыдущих статусов доступности кнопок.
	QTimer oQTimer; ///< Таймер для установки статусов кнопок.

public:
	QPushButton* p_QPushButtonForNotDefault; ///< Указатель на кнопку для снятия автовыбора.
	QList<QPushButton*>* plp_QPushButtonsForDisable; ///< Список указателей на кнопки для блокировки.
};

#endif // SafeSearchingLineEdit_H
