
#include <QTranslator>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QStandardPaths>

#include "application.h"


int main(int argc, char *argv[])
{
    Application app(argc, argv);

    // setup i18n
    QTranslator qTranslator(&app);
    qTranslator.load(QLocale::system(), "fu", ".", app.applicationDirPath().append("/i18n"));
    app.installTranslator(&qTranslator);

    // application info
    QApplication::setApplicationName("fu");
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setQuitOnLastWindowClosed(false);

    // parse arguments
    QCommandLineParser parser;
    parser.setApplicationDescription("fu - File Uploader, a utility upload files/images to remote server and output desirable plain text to your Clipboard");
    parser.addHelpOption();

    QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QString defaultDbPath = dataDir.filePath("data.db");
    QCommandLineOption dbPathOption("p", app.tr("sqlite database file path"), "path", defaultDbPath);
    parser.addOption(dbPathOption);

    parser.process(app);

    if (!app.prepare(parser.value(dbPathOption)))
        return -1;

//    app.showHistoryWindow();
    return app.exec();
}
