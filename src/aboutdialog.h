#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "application.h"

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(Application *appInst);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
    Application *app;
};

#endif // ABOUTDIALOG_H
