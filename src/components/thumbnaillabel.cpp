#include "thumbnaillabel.h"
#include "../application.h"
#include "../core/clipservice.h"

ThumbnailLabel::ThumbnailLabel(QWidget *parent)
    : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(THUMB_WIDTH, THUMB_HEIGHT);
    setMaximumSize(THUMB_WIDTH, THUMB_HEIGHT);

    setAlignment(Qt::AlignCenter);
    setStyleSheet("background: white");

    setText("N/A");
}

ThumbnailLabel::ThumbnailLabel(QWidget *parent, Clip &clip)
    : ThumbnailLabel(parent)
{
    loadByClip(clip);
}

void ThumbnailLabel::loadByClip(Clip &clip)
{
    if (clip.isImage) {
        setText(tr("Loading"));
        _loading = QThread::create([&](void) {
            auto pixmap = clip.thumbnailPixmap();
            if (pixmap.isNull()) {
                this->setText("Failed");
            } else {
                this->setPixmap(pixmap);
                this->setText("");
            }
        });
        connect(_loading, SIGNAL(finished()), _loading, SLOT(deleteLater()));
        _loading->start();
    } else if (clip.isFile) {
        setPixmap(ClipService::unkownFileIcon());
    }
}
