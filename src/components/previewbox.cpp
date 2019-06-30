#include "../application.h"
#include "previewbox.h"
#include "flowlayout.h"
#include "tagbutton.h"
#include "thumbnaillabel.h"

#include <QDebug>
#include <QMenu>

const static QString BOX_STYLE_DEFAULT = "#box{border-style: solid;border-width: 1px;border-color: #eee}";
const static QString BOX_STYLE_SELECTED = "QFrame {background: transparent}\n#box{border-style: solid;border-width: 1px;border-color: #eee; background: lightblue}";

PreviewBox::PreviewBox(QWidget *parent, const Clip &clip)
    : QFrame(parent), clip(clip)
{

    // setup components
    QVBoxLayout *boxLayout = new QVBoxLayout(this);
    setCursor(Qt::PointingHandCursor);
    setLayout(boxLayout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(180, 255);
    setMaximumSize(180, 255);
    setObjectName("box");
    setStyleSheet(BOX_STYLE_DEFAULT);

    previewImg = new ThumbnailLabel(this);
    boxLayout->addWidget(previewImg);

    name = new QLabel(this);
    name->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    name->setMaximumHeight(25);
    name->setText(tr("Uploaded to: ") + "haha");
    name->setAlignment(Qt::AlignHCenter);
    boxLayout->addWidget(name);

    tagsFrame = nullptr;

    // initialize properties
    selected = false;

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));


    QPixmap thumbnail;
    thumbnail.loadFromData(clip.rawPngThumb, "PNG");
    setImage(thumbnail);
    setTags(clip.tags);
    setName(clip.name);
}

/*
PreviewBox::~PreviewBox()
{
    QLayoutItem *item;
    while ((item = layout()->takeAt(0)))
        delete item;
    delete layout();
}
*/

void PreviewBox::setImage(const QPixmap &thumbnail)
{
    previewImg->setPixmap(thumbnail);
}

void PreviewBox::setName(const QString &n)
{
    name->setText(n);
}

void PreviewBox::setTags(const QStringList &tags)
{
    if (tagsFrame) {
        layout()->removeWidget(tagsFrame);
        delete tagsFrame;
    }
    tagsFrame = new QFrame(this);
    tagsFrame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tagsFrame->setMaximumHeight(50);
    FlowLayout *tagsLayout = new FlowLayout(tagsFrame);
    tagsLayout->setMargin(0);
    tagsFrame->setLayout(tagsLayout);
    for (int i = 0; i < tags.size(); i++) {
        TagButton *tag = new TagButton(tags.at(i));
        tagsLayout->addWidget(tag);
    }
    layout()->addWidget(tagsFrame);
}

bool PreviewBox::isSelected()
{
    return selected;
}

void PreviewBox::mousePressEvent(QMouseEvent *evt)
{
    if (evt->button() == Qt::LeftButton) {
        setSelected(!selected);
        evt->accept();
    } else {
        QFrame::mousePressEvent(evt);
    }
}

void PreviewBox::setSelected(bool s)
{
    selected = s;
    setStyleSheet(selected ? BOX_STYLE_SELECTED : BOX_STYLE_DEFAULT);
}

void PreviewBox::showContextMenu(const QPoint &pos)
{
    setSelected(true);

    QMenu contextMenu(this);

    auto outputFormats = APP->outputFormatService()->getAll();

    QList<QObject*> pointers;
    for (auto &upload : APP->uploadService()->getAllByClipId(clip.id)) {
        auto server = APP->serverService()->findById(upload.serverId);
        auto serverMenu = new QMenu(server.name);
        pointers.append(serverMenu);

        for (auto &outputFormat : outputFormats) {
            auto outputAs = new QAction(tr("Copy as %1").arg(outputFormat.name));
            serverMenu->addAction(outputAs);
            connect(outputAs, &QAction::triggered, [=]() {
                APP->clipService()->setClipboard(OutputFormatService::format(outputFormat, clip, upload));
            });
            pointers.append(outputAs);
        }

        contextMenu.addMenu(serverMenu);
    }

    contextMenu.addSeparator();

    QAction editAction(tr("&Edit"), this);
    connect(&editAction, &QAction::triggered, [this]() {
        (new UploadDialog(this))->show();
    });
    contextMenu.addAction(&editAction);

    QAction deleteAction(tr("&Delete this"), this);
    connect(&deleteAction, &QAction::triggered, [this]() {
        APP->clipService()->remove(this->clip.id);
    });
    contextMenu.addAction(&deleteAction);
    contextMenu.exec(mapToGlobal(pos));

    for (auto &pointer : pointers)
        delete pointer;
}
