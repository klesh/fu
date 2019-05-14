#include "upgradedialog.h"
#include "ui_upgradedialog.h"

UpgradeDialog::UpgradeDialog(Migrator *m) :
    QDialog(),
    ui(new Ui::UpgradeDialog),
    migrator(m)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(startUpgrading()));
}

UpgradeDialog::~UpgradeDialog()
{
    delete ui;
}

void UpgradeDialog::startUpgrading()
{
    connect(migrator, SIGNAL(progressChanged(int,  double)), this, SLOT(progressUpdate(int, double)));
    connect(migrator, SIGNAL(finished()), this, SLOT(accept()));
    migrator->start();
    ui->buttonBox->setEnabled(false);
}

void UpgradeDialog::progressUpdate(int step, double percent)
{
    ui->lblStep->setText(QString("%1 : %2/%3").arg(tr("Step")).arg(step).arg(migrator->totalPendingMigration()));
    ui->prgPercentage->setValue(static_cast<int>(percent*100));
}
