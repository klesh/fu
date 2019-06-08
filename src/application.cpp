#include "application.h"

#include <QSystemTrayIcon>
#include <QMenu>


Application::Application(int &argc, char **argv)
    : QApplication(argc, argv), _windowIcon(":icons/icon-32.png")
{
    setWindowIcon(_windowIcon);

}

Application::~Application()
{
    if (_store)
        delete _store;
    if (_tagService)
        delete _tagService;
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
    Migrator migrator(*_store);
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
    trayIcon->setIcon(_windowIcon);
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

bool Application::prepare(const QString &dbPath)
{
    if (_store)
        throw_error("Application:prepare being invoked multiple time");

    QFileInfo dbFileInfo(dbPath);
    if (!dbFileInfo.exists() && !dbFileInfo.dir().exists()) {
        dbFileInfo.dir().mkdir(".");
    }

    // setup database
    _dbPath = dbPath;
    qDebug() << "database path : " << dbPath;
    _store = new SqlStore(dbPath);
    _tagService = new TagService(*_store);
    _serverService = new ServerService(*_store);

    // upgrade checking
    if (showUpgradeWindow() == QDialog::Rejected)
        return false;

    createTrayIcon();
    return true;
}

const QString &Application::getDbPath()
{
    return _dbPath;
}

TagService *Application::tagService()
{
    return _tagService;
}

ServerService *Application::serverService()
{
    return _serverService;
}
