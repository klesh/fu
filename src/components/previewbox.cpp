#include "previewbox.h"
#include "flowlayout.h"
#include "tagbutton.h"
#include "thumbnaillabel.h"

#include <QDebug>

const static QString BOX_STYLE_DEFAULT = "#box{border-style: solid;border-width: 1px;border-color: #eee}";
const static QString BOX_STYLE_SELECTED = "* {background: transparent}\n#box{border-style: solid;border-width: 1px;border-color: #eee; background: lightblue}";

PreviewBox::PreviewBox(QWidget *parent)
    : QFrame(parent)
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

    uploadedTo = new QLabel(this);
    uploadedTo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    uploadedTo->setMaximumHeight(25);
    uploadedTo->setText(tr("Uploaded to: ") + "haha");
    uploadedTo->setAlignment(Qt::AlignHCenter);
    boxLayout->addWidget(uploadedTo);

    tagsFrame = nullptr;

    // initialize properties
    selected = false;
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

void PreviewBox::setUploadedTo(const QString &serverName)
{
    uploadedTo->setText(serverName);
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
    if (evt->button() == Qt::LeftButton)
        selected = !selected ;
    else if (evt->button() == Qt::RightButton)
        selected = true;

    setStyleSheet(selected ? BOX_STYLE_SELECTED : BOX_STYLE_DEFAULT);
    evt->accept();
}
