#include "configdialog.h"
#include "ui_configdialog.h"

#include <QDesktopServices>

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

ConfigDialog &ConfigDialog::getInstance()
{
    static ConfigDialog instance;
    return instance;
}


void ConfigDialog::closeEvent(QCloseEvent *evt)
{
    hide();
    evt->ignore();
}

void ConfigDialog::on_btnOpenDataDir_clicked()
{
    QDesktopServices::openUrl(QUrl("file:///d:/"));
}
