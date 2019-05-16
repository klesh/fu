#include "application.h"
#include "upgradedialog.h"
#include "aboutdialog.h"
#include "configdialog.h"
#include "historywindow.h"

#include <QSystemTrayIcon>
#include <QMenu>
#include <QSqlDatabase>

static UpgradeDialog *upgradeDialog = nullptr;
static AboutDialog *aboutDialog = nullptr;
static ConfigDialog *configDialog = nullptr;
static HistoryWindow *historyWindow = nullptr;
static QSqlDatabase db;

Application::Application(const QString &dbPath)
    : windowIcon(":icons/icon-32.png"), dbPath(dbPath)
{
    qDebug() << "setup database : " << dbPath;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
    db.open();
}

template<typename T>
void Application::showWindowOrDialog(T **wd)
{
    // if target window/dialog is already running, bring it to top instead of creating a new instance.
    if (!*wd) {
        qDebug() << "creating window/dialog";
        *wd = new T();
        connect(*wd, &QWidget::destroyed, [=](void){
            qDebug() << "set window/dialog to null";
            *wd = nullptr;
        });
        (*wd)->setAttribute(Qt::WA_DeleteOnClose);
        (*wd)->setWindowFlags((*wd)->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        (*wd)->setWindowIcon(windowIcon);
        (*wd)->show();
        qDebug() << "showing window/dialog";
    } else {
        (*wd)->raise();
        (*wd)->activateWindow();
        qDebug() << "activate existing window/dialog";
    }
}

int Application::showUpgradeWindow()
{
    // run migrations if needed
    Migrator migrator;
    if (migrator.totalPendingMigration() > 0) {
        showWindowOrDialog<UpgradeDialog>(&upgradeDialog);
        upgradeDialog->setMigrator(&migrator);
        return upgradeDialog->exec();
    }
    return QDialog::Accepted;
}

void Application::createTrayIcon()
{
    qDebug() << "create context menu";
    QMenu *trayMenu = new QMenu();

    QAction *historyAction = new QAction(tr("&History"));
    trayMenu->addAction(historyAction);
    connect(historyAction, &QAction::triggered, this, &Application::showHistoryWindow);

    QAction *configAction = new QAction(tr("&Config"));
    trayMenu->addAction(configAction);
    connect(configAction, &QAction::triggered, this, &Application::showConfigDialog);

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
    trayIcon->setIcon(windowIcon);
    trayIcon->show();

    connect(qApp, &QApplication::aboutToQuit, [=](void){
        qDebug() << "cleanup tray icon";
        delete trayIcon;
        delete trayMenu;
    });
}

void Application::showAboutDialog()
{
    showWindowOrDialog<AboutDialog>(&aboutDialog);
}

void Application::showConfigDialog()
{
    showWindowOrDialog<ConfigDialog>(&configDialog);
}

void Application::showHistoryWindow()
{
    showWindowOrDialog<HistoryWindow>(&historyWindow);
}
