#include "thumbnaillabel.h"
#include "../application.h"
#include "../core/clipservice.h"

void ThumbnailLabel::loadClip()
{
    if ((*_clip).isImage) {
        setText(tr("Loading"));
        _loading = QThread::create([&](void) {
            this->setPixmap((*_clip).thumbnailPixmap());
            this->setText("");
        });
        connect(_loading, SIGNAL(finished()), _loading, SLOT(deleteLater()));
        _loading->start();
    } else if ((*_clip).isFile) {
        setPixmap(ClipService::unkownFileIcon());
    } else {
        setText("N/A");
    }
}

ThumbnailLabel::ThumbnailLabel(QWidget *parent)
    : QLabel(parent)
{

}

ThumbnailLabel::ThumbnailLabel(QWidget *parent, Clip *clip)
    : QLabel(parent), _clip(clip)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(THUMB_WIDTH, THUMB_HEIGHT);
    setMaximumSize(THUMB_WIDTH, THUMB_HEIGHT);

    setAlignment(Qt::AlignCenter);
    setStyleSheet("background: white");

    loadClip();
}

void ThumbnailLabel::setClip(Clip *clip)
{
    _clip = clip;
    loadClip();
}
