#include "src/aboutdialog.h"

#include <QApplication>
#include <QTranslator>

#include "application.h"

int main(int argc, char *argv[])
{
    QApplication qtApp(argc, argv);
    qtApp.setQuitOnLastWindowClosed(false);

    // setup i18n
    QTranslator qTranslator(&qtApp);
    qTranslator.load(QLocale::system(), "fu", ".", qtApp.applicationDirPath().append("/i18n"));
    qtApp.installTranslator(&qTranslator);


    // initialize app
    Application app;

    return qtApp.exec();
}
