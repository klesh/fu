#ifndef UPGRADEDIALOG_H
#define UPGRADEDIALOG_H

#include <QDialog>
#include "store/migrator.h"

namespace Ui {
class UpgradeDialog;
}

class UpgradeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpgradeDialog(Migrator *m);
    ~UpgradeDialog();

public slots:
    void progressUpdate(int step, double percent);
    void startUpgrading();

private:
    Ui::UpgradeDialog *ui;
    Migrator *migrator;
};

#endif // UPGRADEDIALOG_H
