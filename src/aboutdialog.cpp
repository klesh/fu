#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog() :
    QDialog(),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->lblVersion->setText(tr("Version: ") + APP_VERSION);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
