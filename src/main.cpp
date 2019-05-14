#include "src/aboutdialog.h"

#include <QApplication>
#include <QTranslator>

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

    // run migrations if needed
    Migrator *migrator = new Migrator();
    if (migrator->totalPendingMigration() > 0) {
        UpgradeDialog *upgradeDialog = new UpgradeDialog(migrator);
        if (upgradeDialog->exec() == QDialog::Rejected) {
            qDebug() << "rejected";
            return 0;
        }
        delete migrator;
    }

    qDebug() << "upgraded";

    // initialize app
    Application app;

    return qtApp.exec();
}
