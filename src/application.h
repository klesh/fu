#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QIcon>
#include <QtDebug>

class Application : public QObject
{
public:
    Application();

    QIcon &windowIcon();
    void showAboutDialog();
    void showConfigDialog();
    void showHistoryWindow();

private:
    template <typename T>
    void showWindowOrDialog(T **widget);
};

#endif // APPLICATION_H
