#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "configdialog.h"
#include "historywindow.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->lblVersion->setText(tr("Version: ") + APP_VERSION);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    createTrayIcon();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::createTrayIcon()
{
    // create context menu
    trayMenu = new QMenu(this);

    QAction *historyAction = new QAction(tr("&History"));
    trayMenu->addAction(historyAction);
    connect(historyAction, &QAction::triggered, &HistoryWindow::getInstance(), &HistoryWindow::show);

    QAction *configAction = new QAction(tr("&Config"));
    trayMenu->addAction(configAction);
    connect(configAction, &QAction::triggered, &ConfigDialog::getInstance(), &ConfigDialog::show);

    QAction *aboutAction = new QAction(tr("&About"));
    trayMenu->addAction(aboutAction);
    connect(aboutAction, &QAction::triggered, this, &AboutDialog::show);

    trayMenu->addSeparator();

    QAction *quitAction = new QAction(tr("&Exit"));
    trayMenu->addAction(quitAction);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));


    // setup tray icon
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->setIcon(QIcon(":/icons/icon.png"));
    trayIcon->show();
}

void AboutDialog::closeEvent(QCloseEvent *evt)
{
    hide();
    evt->ignore();
}
