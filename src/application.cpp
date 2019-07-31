#include "application.h"

#include <QMenu>


Application::Application(int &argc, char **argv)
    : QApplication(argc, argv), _windowIcon(":icons/icon-32.png"), _uploadingIcon(":icons/icon-uploading-32.png")
{
    setWindowIcon(_windowIcon);

}

Application::~Application()
{
}

template<typename T>
void Application::showWindowOrDialog(T **wd)
{
    // if target window/dialog is already running, bring it to top instead of creating a new instance.
    if (!*wd) {
        *wd = new T();
        connect(*wd, &QWidget::destroyed, [=](void){
            *wd = nullptr;
        });
        (*wd)->setAttribute(Qt::WA_DeleteOnClose);
        (*wd)->setWindowFlags((*wd)->windowFlags() & ~Qt::WindowContextHelpButtonHint);
        (*wd)->show();
    }
    (*wd)->raise();
    (*wd)->activateWindow();
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

    auto historyAction = new QAction(tr("&History"));
    trayMenu->addAction(historyAction);
    connect(historyAction, &QAction::triggered, this, &Application::showHistoryWindow);

    auto configAction = new QAction(tr("&Config"));
    trayMenu->addAction(configAction);
    connect(configAction, &QAction::triggered, this, &Application::showConfigDialog);

    auto aboutAction = new QAction(tr("&About"));
    trayMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &Application::showAboutDialog);

    trayMenu->addSeparator();

    auto quitAction = new QAction(tr("&Exit"));
    trayMenu->addAction(quitAction);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));


    qDebug() << "setup tray icon";
    _trayIcon = new QSystemTrayIcon();
    _trayIcon->setContextMenu(trayMenu);
    _trayIcon->setIcon(_windowIcon);
    _trayIcon->show();

    connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    connect(qApp, &QApplication::aboutToQuit, [&](void){
        qDebug() << "cleanup tray icon";
        delete _trayIcon;
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

void Application::showUploadDialog()
{
    showWindowOrDialog<UploadDialog>(&uploadDialog);
}

bool Application::prepare(const QString &dbPath)
{
    // create folder
    QFileInfo dbFileInfo(dbPath);
    if (!dbFileInfo.exists() && !dbFileInfo.dir().exists()) {
        dbFileInfo.dir().mkdir(".");
    }

    // setup database
    _dbPath = dbPath;
    qDebug() << "database path : " << dbPath;
    _sqlDb = QSqlDatabase::addDatabase("QSQLITE");
    _sqlDb.setDatabaseName(_dbPath);
    _sqlDb.open();
    _tagService = new TagService();
    _serverService = new ServerService();
    _settingService = new SettingService();
    _formatService = new FormatService();
    _clipService = new ClipService();
    _uploadService = new UploadService();

    // upgrade checking
    if (showUpgradeWindow() == QDialog::Rejected)
        return false;

    createTrayIcon();

    // timer for uploading icon animation
    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [&](void) {
        if (_uploadService->isUploading()) {
            _isUploadingIcon = !_isUploadingIcon;
            _trayIcon->setIcon(_isUploadingIcon ? _uploadingIcon : _windowIcon);
        } else if (_isUploadingIcon) {
            _isUploadingIcon = false;
            _trayIcon->setIcon(_windowIcon);
        }
    });
    timer->start(500);

    return true;
}

void Application::sendNotification(const QString &message, const QString &title, QSystemTrayIcon::MessageIcon icon, int timeout)
{
    _trayIcon->showMessage(title, message, icon, timeout);
}

const QString &Application::getDbPath()
{
    return _dbPath;
}

void Application::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::ActivationReason::Trigger) {
        qDebug() << "tray icon activated " << reason;
        showUploadDialog();
    }
}
