#include "src/aboutdialog.h"

#include <QApplication>
#include <QTranslator>

#include "historywindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // setup i18n
    QTranslator qTranslator(&app);
    qTranslator.load(QLocale::system(), "fu", ".", app.applicationDirPath().append("/i18n"));
    app.installTranslator(&qTranslator);

    // create about dialog
    AboutDialog about;

    HistoryWindow::getInstance().show();
    return app.exec();
}
