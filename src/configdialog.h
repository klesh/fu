#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QAbstractItemDelegate>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog();
    ~ConfigDialog();

private slots:
    void on_btnOpenDataDir_clicked();
    void on_currentTab_changed(int id);
    void on_lstTags_endEdit(QWidget*, QAbstractItemDelegate::EndEditHint);
    void on_btnAddTag_clicked();
    void on_btnDelTag_clicked();

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
