#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include <QAbstractItemDelegate>
#include <QListWidgetItem>

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

    void serversShowItem(QListWidgetItem*, QListWidgetItem*);
    void serversEditItem(QListWidgetItem*);
    void serversEditItemSave();
    void serversEditItemCancel();
    void serversAddItem();
    void serversDelItems();
    void serversReloadSettingsFrame(const QString&);

    void tagsEndEdit(QWidget*);
    void tagsAddItem();
    void tagsDelItems();

    void imagePickWatermarkFile();
    void imageSaveCompressionSetting(int);
    void imageSaveWatermarkSetting(int);
    void imageSaveWatermarkPositionSetting(bool);

    void bakOpenDataDir();

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
