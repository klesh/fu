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
    void reloadTab(int id);
    void bakOpenDataDir();
    void tagsEndEdit(QWidget*);
    void tagsAddItem();
    void tagsDelItems();

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
