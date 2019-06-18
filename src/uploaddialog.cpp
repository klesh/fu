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
:hover:!checked { \
    background: #eee; \
}\n\
:hover:checked { \
    color: white; \
}\n\
:checked { \
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

UploadDialog::UploadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UploadDialog)
{
    ui->setupUi(this);

    auto uploadToLayout = new QVBoxLayout(ui->sclUploadTo);
    auto outputFormats = APP->outputFormatService()->getAll();

    for (auto &server : APP->serverService()->getAll()) {
        auto uploadWidget = new QPushButton(server.name, ui->sclUploadTo);
        auto outputWidget = new QComboBox(ui->sclUploadTo);
        outputWidget->addItem(tr("Don't output"), 0);
        int i = 1;
        for (auto &outputFormat : outputFormats) {
            outputWidget->addItem(outputFormat.name, outputFormat.id);
            if (outputFormat.id == server.outputFormatId) {
                qDebug() << "output format index " << i;
                outputWidget->setCurrentIndex(i);
            }
            i++;
        }

        uploadWidget->setCheckable(true);
        uploadWidget->setChecked(server.uploadEnabled);
        outputWidget->setEnabled(server.uploadEnabled);
        connect(uploadWidget, &QPushButton::toggled, [=](bool checked) {
            outputWidget->setEnabled(checked);
            APP->serverService()->setUploadEnabled(server.id, checked);
            refresh();
        });
        connect(outputWidget, &QComboBox::currentTextChanged, [=](const QString&) {
            APP->serverService()->setOutputFormatId(server.id, outputWidget->currentData().toUInt());
        });

        uploadWidget->setStyleSheet(UPLOAD_STYLE_SHEET);
        uploadWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        outputWidget->setStyleSheet(OUTPUT_STYLE_SHEET);
        outputWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        auto rowLayout = new QHBoxLayout();
        rowLayout->addWidget(uploadWidget);
        rowLayout->addWidget(outputWidget);
        uploadToLayout->addItem(rowLayout);
    }
    uploadToLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));


    _previewLayout = new FlowLayout(ui->sclPreview);

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(reload()));
    reload();
}

UploadDialog::~UploadDialog()
{
    delete ui;
}

void UploadDialog::accept()
{
    qDebug() << _clips.size();
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
        _previewLayout->addWidget(new ThumbnailLabel(ui->sclPreview, clip.icon));
        if (!clip.name.isEmpty()) {
            auto lblName = new QLabel(ui->sclPreview);
            lblName->setText(clip.name);
            lblName->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            lblName->setAlignment(Qt::AlignCenter);
            lblName->setMinimumWidth(THUMB_WIDTH);
            QFontMetrics metrix(lblName->font());
            int width = THUMB_WIDTH - 5;
            QString clippedText = metrix.elidedText(clip.name, Qt::ElideRight, width);
            lblName->setText(clippedText);
            _previewLayout->addWidget(lblName);
        }
    }

    refresh();
}

void UploadDialog::refresh()
{
    bool isAnyServerSelected = false;
    for (auto &button : ui->sclUploadTo->findChildren<QPushButton*>()) {
        if (button->isChecked()) {
            isAnyServerSelected = true;
            break;
        }
    }

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isAnyServerSelected && !_clips.empty());
}
