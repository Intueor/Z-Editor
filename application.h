#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

//== КЛАССЫ.
/// Класс приложения.
class Application : public QApplication
{
	Q_OBJECT
public:
	/// Конструктор.
	Application(int& iArgc, char** pp_Argv, int = ApplicationFlags);
												///< \param[in] iArgc Ссылка на целоцисленный аргумент.
												///< \param[in] pp_Argv Указатель на указатель на буфер со строкой аргументов.
	/// Переопределение обработки всех событий из-за болкировки текстового ввода в поле пользовательского редактора внутри элемента.
	bool notify(QObject* p_QObject, QEvent* p_QEvent) override;
												///< \param[in] p_QObject Указатель на объект-адресат.
												///< \param[in] p_QEvent Указатель на событие.
												///< \return Результат работы обработчика.
};

#endif // APPLICATION_H
