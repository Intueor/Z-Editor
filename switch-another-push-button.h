#ifndef SWITCHANOTHERPUSHBUTTON_H
#define SWITCHANOTHERPUSHBUTTON_H

//== ВКЛЮЧЕНИЯ.
#include <QPushButton>
#include <QTimer>

//== ПРОСТРАНСТВА ИМЁН.
namespace Ui {
class SwitchAnotherPushButton;
}

//== КЛАССЫ.
/// Класс кнопки-переключателя заданных кнопок.
class SwitchAnotherPushButton : public QPushButton
{
	Q_OBJECT
public:
	/// Конструктор 1.
	explicit SwitchAnotherPushButton(QWidget* p_Parent = nullptr);
									///< \param[in] p_Parent Указатель на родителя.
	/// Конструктор 2.
	explicit SwitchAnotherPushButton(const QString& a_strText, QWidget* p_Parent = nullptr);
									///< \param[in] a_strText Ссылка на инициализирующую строку.
									///< \param[in] p_Parent Указатель на родителя.
protected:
	/// Перехват событий.
	bool event(QEvent* p_Event) override;
									///< \param[in] p_Event Указатель на событие.
public:
	bool bReady; ///< Признак готовности.
	QList<QPushButton*>* pvp_QPushButtonsForSwitching; ///< Указатель на список переключаемых кнопок.

private:
	QTimer oQTimer; ///< Таймер для установки статусов кнопок.
	bool bLastEnabledState; ///< Последний заданный статус включённости кнопки.
};

#endif // SWITCHANOTHERPUSHBUTTON_H
