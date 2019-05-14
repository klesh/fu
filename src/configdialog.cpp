#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDesktopServices>

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
    QDesktopServices::openUrl(QUrl("file:///d:/"));
}
