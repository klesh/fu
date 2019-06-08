#ifndef APPLICATION_H
#define APPLICATION_H

#include "components/errormessage.h"
#include "upgradedialog.h"
#include "aboutdialog.h"
#include "configdialog.h"
#include "historywindow.h"
#include "store/sqlstore.h"
#include "core/error.h"
#include "core/tagservice.h"
#include "core/serverservice.h"

#include <QtGlobal>
#include <QApplication>
#include <QIcon>

class Application : public QApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv);
    ~Application() override;

    int showUpgradeWindow();
    void createTrayIcon();
    void showAboutDialog();
    void showConfigDialog();
    void showHistoryWindow();
    bool prepare(const QString& dbPath);

    const QString &getDbPath();

    TagService *tagService();
    ServerService *serverService();

private:
    template <typename T>
    void showWindowOrDialog(T **widget);
    QIcon _windowIcon;
    QString _dbPath;
    SqlStore *_store = nullptr;
    TagService *_tagService = nullptr;
    ServerService *_serverService = nullptr;

    UpgradeDialog *upgradeDialog = nullptr;
    AboutDialog *aboutDialog = nullptr;
    ConfigDialog *configDialog = nullptr;
    HistoryWindow *historyWindow = nullptr;
};

#define APP (qobject_cast<Application*>qApp)

#endif // APPLICATION_H


