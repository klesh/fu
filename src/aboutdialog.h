#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;

    void createTrayIcon();

    void closeEvent(QCloseEvent *evt);
};

#endif // ABOUTDIALOG_H
