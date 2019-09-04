#include "application.h"
#include "upgradedialog.h"
#include "ui_upgradedialog.h"
#include <QPushButton>

UpgradeDialog::UpgradeDialog() :
    QDialog(),
    ui(new Ui::UpgradeDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(startUpgrading()));
}

void UpgradeDialog::setMigrator(Migrator *m)
{
    migrator = m;
    progressUpdate(1, 0);
}

UpgradeDialog::~UpgradeDialog()
{
    delete ui;
}

void UpgradeDialog::startUpgrading()
{
    connect(migrator, SIGNAL(progressChanged(int,  double)), this, SLOT(progressUpdate(int, double)));
    ui->buttonBox->setEnabled(false);
    migrator->run();
    accept();
}

void UpgradeDialog::progressUpdate(int step, double percent)
{
    ui->lblStep->setText(QString("%1 : %2/%3").arg(tr("Step")).arg(step).arg(migrator->totalPendingMigration()));
    ui->prgPercentage->setValue(static_cast<int>(percent*100));
}
