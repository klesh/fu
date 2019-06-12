#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include <QDialog>
#include <QKeyEvent>

/*
#include <QResizeEvent>
#include <QtDebug>
*/

namespace Ui {
class UploadDialog;
}

class UploadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UploadDialog(QWidget *parent = nullptr);
    ~UploadDialog();

private:
    Ui::UploadDialog *ui;

    /*
protected:
    void resizeEvent(QResizeEvent *e) override { qDebug() << e->size(); }
    */
};

#endif // UPLOADDIALOG_H
