#include "uploaddialog.h"
#include "ui_uploaddialog.h"
#include "application.h"
#include "components/flowlayout.h"
#include "components/thumbnaillabel.h"
#include "core/clipservice.h"

#include <QClipboard>
#include <QMimeData>
#include <QFormLayout>
#include <QPushButton>

const static QString UPLOAD_STYLE_SHEET = "\
QPushButton { \
    height: 20px; \
    border-top-left-radius: 8px; \
    border-bottom-left-radius: 8px; \
    border-style:  solid; \
    border-width:1px; \
    border-color: #999; \
    background: white; \
    padding: 3px 10px; \
}\n\
QPushButton:hover:!checked { \
    background: #eee; \
}\n\
QPushButton:hover:checked { \
    color: white; \
}\n\
QPushButton:checked { \
    background: #28a745; \
}\n\
" ;
const static QString OUTPUT_STYLE_SHEET = "\
QComboBox  { \
    height: 20px; \
    border-top-right-radius: 8px; \
    border-bottom-right-radius: 8px; \
    border-style:  solid; \
    border-width:1px; \
    border-color: #999; \
    background: white; \
    padding: 3px 10px 3px 10px; \
    border-left: none; \
}\n\
QComboBox::drop-down { \
    border-radius: 3px; \
} \
" ;

UploadDialog::UploadDialog(QWidget *parent, uint clipId) :
    QDialog(parent),
    ui(new Ui::UploadDialog)
{
    ui->setupUi(this);

    _previewLayout = new FlowLayout(ui->sclPreview);
    auto uploadToLayout = new QVBoxLayout(ui->sclUploadTo);
    if (clipId) {
        editMode(clipId);
    } else {
        uploadMode();
    }

    uploadToLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    connect(APP->uploadService(), SIGNAL(uploadStateChanged()), this, SLOT(syncState()));
}


UploadDialog::~UploadDialog()
{
    delete ui;
}

void UploadDialog::editMode(uint clipId)
{
    auto clip = APP->clipService()->findById(clipId);
    _clips.append(clip);

    for (auto &upload : APP->uploadService()->getAllByClipId(clipId)) {
        Server server;
        server.id = 0;
        server.name = upload.serverName;
        createUploadToRow(server);
    }

    auto thumbnail = new ThumbnailLabel(ui->sclPreview);
    thumbnail->setPixmap(clip.thumbnailPixmap());
    _previewLayout->addWidget(thumbnail);
    auto name = new QLabel(ui->sclPreview);
    QFontMetrics metrix(name->font());
    int width = thumbnail->width() - 5;
    QString clippedText = metrix.elidedText(clip.name, Qt::ElideRight, width);
    name->setText(clippedText);
    _previewLayout->addWidget(name);

    ui->tgeTags->setTags(clip.tags);
    ui->txtDescription->setText(clip.description);
}

void UploadDialog::uploadMode()
{
    auto formats = APP->formatService()->getAll();
    for (auto &server : APP->serverService()->getAll()) {
        createUploadToRow(server, formats);
    }

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(reload()));
    reload();
}

void UploadDialog::createUploadToRow(const Server &server, const QList<Format> &formats)
{
    auto rowLayout = new QHBoxLayout();

    auto uploadWidget = new QPushButton(server.name, ui->sclUploadTo);

    uploadWidget->setCheckable(true);
    rowLayout->addWidget(uploadWidget);

    if (server.id) {
        auto outputWidget = new QComboBox(ui->sclUploadTo);
        outputWidget->addItem(tr("Don't output"), 0);
        int i = 1;
        for (auto &format : formats) {
            outputWidget->addItem(format.name, format.id);
            if (format.id == server.formatId) {
                outputWidget->setCurrentIndex(i);
            }
            i++;
        }
        uploadWidget->setChecked(server.uploadEnabled);
        connect(uploadWidget, &QPushButton::toggled, [this, server, outputWidget](bool checked) {
            outputWidget->setEnabled(checked);
            APP->serverService()->setUploadEnabled(server.id, checked);
            syncState();
        });

        outputWidget->setEnabled(server.uploadEnabled);
        outputWidget->setStyleSheet(OUTPUT_STYLE_SHEET);
        outputWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        rowLayout->addWidget(outputWidget);
        connect(outputWidget, &QComboBox::currentTextChanged, [server, outputWidget](const QString&) {
            APP->serverService()->setOutputFormatId(server.id, outputWidget->currentData().toUInt());
        });
    } else { // edit mode
        uploadWidget->setChecked(true);
        uploadWidget->setEnabled(false);
    }

    uploadWidget->setStyleSheet(UPLOAD_STYLE_SHEET);
    uploadWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->sclUploadTo->layout()->addItem(rowLayout);
}

void UploadDialog::accept()
{
    // edit mode
    if (_clips.size() == 1 && _clips.first().id) {
        auto clip = _clips.first();
        clip.tags = ui->tgeTags->tags();
        clip.description = ui->txtDescription->text();
        APP->clipService()->update(clip);
        QDialog::accept();
        return;
    }

    // pick up file name
    for (auto &clip : _clips) {
        clip.name = ui->sclPreview->findChild<QLineEdit*>(clip.name)->text();
    }

    APP->uploadService()->upload(_clips, ui->tgeTags->tags(), ui->txtDescription->text());
    QDialog::accept();
}

void UploadDialog::reload()
{
    QLayoutItem* child = nullptr;
    while ((child = _previewLayout->takeAt(0))) {
        delete child;
    }
    QWidget *ctrl = nullptr;
    while ((ctrl = ui->sclPreview->findChild<QWidget*>())) {
        delete ctrl;
    }

    _clips =  APP->clipService()->getAllFromClipboard();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    for (auto &clip : _clips) {
        auto thumbnail = new ThumbnailLabel(ui->sclPreview, clip);

        _previewLayout->addWidget(thumbnail);
        auto name = new QLineEdit(ui->sclPreview);
        name->setText(clip.name);
        name->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        name->setAlignment(Qt::AlignCenter);
        name->setMinimumWidth(THUMB_WIDTH);
        name->setObjectName(clip.name);
        connect(name, &QLineEdit::textChanged, [&](const QString &) {
            this->syncState();
        });
        _previewLayout->addWidget(name);
    }

    syncState();
}

void UploadDialog::syncState()
{
    bool isAnyServerSelected = false;
    for (auto &button : ui->sclUploadTo->findChildren<QPushButton*>()) {
        if (button->isChecked()) {
            isAnyServerSelected = true;
            break;
        }
    }
    bool isAnyClipNameEmpty = false;
    for (auto &lineEdit : ui->sclPreview->findChildren<QLineEdit*>()) {
        if (lineEdit->text().isEmpty()) {
            isAnyClipNameEmpty = true;
            break;
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isAnyServerSelected
                                                            && !_clips.empty()
                                                            && !isAnyClipNameEmpty
                                                            && !APP->uploadService()->isUploading());
}
