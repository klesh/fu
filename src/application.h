#ifndef APPLICATION_H
#define APPLICATION_H

#include <functional>
#include <QApplication>
#include <QIcon>
#include <QtDebug>

class Application : public QObject
{
public:
    Application();

    QIcon &windowIcon();
    void showAboutDialog();
private:
    template <typename T>
    void ensureExistence(T **widget, std::function<T*(void)> createWD);
};

#endif // APPLICATION_H
