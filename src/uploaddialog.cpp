#include "uploaddialog.h"
#include "ui_uploaddialog.h"
#include "application.h"
#include "components/flowlayout.h"
#include "components/thumbnaillabel.h"

#include <QClipboard>
#include <QMimeData>

UploadDialog::UploadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadDialog)
{
    ui->setupUi(this);

    const static QPixmap thumbnail = QPixmap("D:/Nextcloud/kleshwong/wallpapers/6fVBDMW-dark-minimalist-wallpaper.jpg").scaled(160, 160, Qt::KeepAspectRatio);

    auto previewLayout = new FlowLayout(ui->sclPreview);
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, thumbnail));
    /*
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    if (mimeData->hasImage()) {
        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    } else if (mimeData->hasHtml()) {
        setText(mimeData->html());
        setTextFormat(Qt::RichText);
    } else if (mimeData->hasText()) {
        setText(mimeData->text());
        setTextFormat(Qt::PlainText);
    } else {
        setText(tr("Cannot display data"));
    }
    */
}

UploadDialog::~UploadDialog()
{
    delete ui;
}
