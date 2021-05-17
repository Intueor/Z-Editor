#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication
{
	Q_OBJECT
public:
	Application(int& iArgc, char** pp_Argv, int = ApplicationFlags);
	bool notify(QObject* p_QObject, QEvent* p_QEvent) override;
};

#endif // APPLICATION_H
