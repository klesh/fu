#include "application.h"
#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDesktopServices>
#include <QFileInfo>

ConfigDialog::ConfigDialog() :
    QDialog(),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

void ConfigDialog::on_btnOpenDataDir_clicked()
{
    auto app = qobject_cast<Application*>qApp;
    QFileInfo fileInfo(app->getDbPath());
    QDesktopServices::openUrl(QUrl("file:///" + fileInfo.dir().absolutePath()));
}
