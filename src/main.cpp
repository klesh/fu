
#include <QTranslator>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDir>
#include <QStandardPaths>

#include "application.h"
#include "runguard.h"


int main(int argc, char *argv[])
{
    Application app(argc, argv);

    RunGuard guard("File2URL");
    if (!guard.tryToRun()) {
        QMessageBox::warning(NULL, app.tr("Warning!"), app.tr("An instance of this application is running!") );
        exit(1);
    }

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
