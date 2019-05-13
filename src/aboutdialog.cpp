#include "aboutdialog.h"
#include "ui_aboutdialog.h"
//#include "configdialog.h"
//#include "historywindow.h"

AboutDialog::AboutDialog(Application *appInst) :
    QDialog(),
    ui(new Ui::AboutDialog),
    app(appInst)
{
    ui->setupUi(this);
    ui->lblVersion->setText(tr("Version: ") + APP_VERSION);

    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowIcon(app->windowIcon());
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
