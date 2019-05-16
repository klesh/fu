#include "src/aboutdialog.h"

#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QStandardPaths>

#include "application.h"
#include "store/migrator.h"
#include "upgradedialog.h"

int main(int argc, char *argv[])
{
    QApplication qtApp(argc, argv);
    qtApp.setQuitOnLastWindowClosed(false);

    // setup i18n
    QTranslator qTranslator(&qtApp);
    qTranslator.load(QLocale::system(), "fu", ".", qtApp.applicationDirPath().append("/i18n"));
    qtApp.installTranslator(&qTranslator);

    // initialize app
    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dataDir.exists())
        dataDir.mkdir(".");

    QString dbPath = dataDir.filePath("data.db");
    Application app(dbPath);

    if (app.showUpgradeWindow() == QDialog::Rejected)
        return -1;

    app.createTrayIcon();

    return qtApp.exec();
}
