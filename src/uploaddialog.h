#ifndef UPLOADDIALOG_H
#define UPLOADDIALOG_H

#include "models/clip.h"
#include "models/outputformat.h"
#include "models/server.h"
#include "components/thumbnaillabel.h"

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
    UploadDialog(QWidget *parent = nullptr, uint clipId = 0);
    ~UploadDialog() override;

private:
    Ui::UploadDialog *ui;
    QList<Clip> _clips;
    QLayout *_previewLayout;
    void editMode(uint clipId);
    void uploadMode();
    void createUploadToRow(const Server &server, const QList<OutputFormat> &outputFormats = QList<OutputFormat>());

protected:
    void accept() override;

public slots:
    void reload();
    void syncState();
};

#endif // UPLOADDIALOG_H
