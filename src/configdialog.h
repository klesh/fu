#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QCloseEvent>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    static ConfigDialog &getInstance();
    ConfigDialog(ConfigDialog const&) = delete;
    void operator=(ConfigDialog const&) = delete;

private slots:
    void on_btnOpenDataDir_clicked();

private:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();
    void closeEvent(QCloseEvent *evt);

    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
