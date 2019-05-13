#include "application.h"
#include "aboutdialog.h"

#include <QSystemTrayIcon>
#include <QMenu>

static AboutDialog *aboutDialog = nullptr;

Application::Application()
{
    //
    qDebug() << "create context menu";
    QMenu *trayMenu = new QMenu();

    QAction *historyAction = new QAction(tr("&History"));
    trayMenu->addAction(historyAction);
    //connect(historyAction, &QAction::triggered, &HistoryWindow::getInstance(), &HistoryWindow::show);

    QAction *configAction = new QAction(tr("&Config"));
    trayMenu->addAction(configAction);
    //connect(configAction, &QAction::triggered, &ConfigDialog::getInstance(), &ConfigDialog::show);

    QAction *aboutAction = new QAction(tr("&About"));
    trayMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &Application::showAboutDialog);

    trayMenu->addSeparator();

    QAction *quitAction = new QAction(tr("&Exit"));
    trayMenu->addAction(quitAction);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));


    qDebug() << "setup tray icon";
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon();
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setIcon(windowIcon());
    trayIcon->show();

    connect(qApp, &QApplication::aboutToQuit, [=](void){
        qDebug() << "cleanup tray icon";
        delete trayIcon;
        delete trayMenu;
    });
}

QIcon &Application::windowIcon()
{
    static QIcon icon(":icons/icon-32.png");
    return icon;
}

template<typename T>
void Application::ensureExistence(T **wd, std::function<T*(void)> createWD)
{
    if (!*wd) {
        qDebug() << "creating window/dialog";
        *wd = createWD();
        connect(*wd, &QWidget::destroyed, [=](void){
            qDebug() << "set window/dialog to null";
            *wd = nullptr;
        });
        (*wd)->show();
        qDebug() << "showing window/dialog";
    } else {
        (*wd)->raise();
        (*wd)->activateWindow();
        qDebug() << "activate existing window/dialog";
    }
}

void Application::showAboutDialog()
{
    ensureExistence<AboutDialog>(&aboutDialog, [this] { return new AboutDialog(this); });
}


