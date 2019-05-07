#include "previewbox.h"
#include "flowlayout.h"
#include "tagbutton.h"

const static QString BOX_STYLE_DEFAULT = "#box{border-style: solid;border-width: 1px;border-color: #eee}";
const static QString BOX_STYLE_SELECTED = "#box{border-style: solid;border-width: 1px;border-color: #eee}";

PreviewBox::PreviewBox(QWidget *parent)
    : QFrame(parent)
{
    QVBoxLayout *boxLayout = new QVBoxLayout(this);
    setCursor(Qt::PointingHandCursor);
    setLayout(boxLayout);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(180, 255);
    setMaximumSize(180, 255);
    setObjectName("box");
    setStyleSheet(BOX_STYLE_DEFAULT);

    previewImg = new QLabel(this);
    previewImg->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    previewImg->setMinimumSize(160, 160);
    previewImg->setMaximumSize(160, 160);
    boxLayout->addWidget(previewImg);

    uploadedTo = new QLabel(this);
    uploadedTo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    uploadedTo->setMaximumHeight(25);
    uploadedTo->setText(tr("Uploaded to: ") + "haha");
    uploadedTo->setAlignment(Qt::AlignHCenter);
    boxLayout->addWidget(uploadedTo);

    tagsFrame = nullptr;

}

void PreviewBox::setImage(const QString &url)
{
    previewImg->setPixmap(QPixmap(url).scaled(160, 160, Qt::KeepAspectRatio));
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

void PreviewBox::mousePressEvent(QMouseEvent *evt)
{
}
