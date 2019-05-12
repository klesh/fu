#ifndef FORMDIALOG_H
#define FORMDIALOG_H

#include <QDialog>

namespace Ui {
class FormDialog;
}

class FormDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FormDialog(QWidget *parent = nullptr);
    ~FormDialog();

private:
    Ui::FormDialog *ui;
};

#endif // FORMDIALOG_H
