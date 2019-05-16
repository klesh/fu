#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QIcon>
#include <QtDebug>

class Application : public QObject
{
public:
    Application(const QString &dbPath);

    int showUpgradeWindow();
    void createTrayIcon();
    void showAboutDialog();
    void showConfigDialog();
    void showHistoryWindow();

    const QIcon windowIcon;

private:
    template <typename T>
    void showWindowOrDialog(T **widget);
    const QString &dbPath;
};

#endif // APPLICATION_H


