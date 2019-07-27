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
    setPixmap(ClipService::unkownFileIcon());
}

ThumbnailLabel::ThumbnailLabel(QWidget *parent, const QPixmap &origin)
    : ThumbnailLabel(parent)
{
    setThumbnailByOrigin(origin);
}

ThumbnailLabel::ThumbnailLabel(QWidget *parent, const QString &originPath)
    : ThumbnailLabel(parent)
{
    setThumbnailByOriginPath(originPath);
}

ThumbnailLabel::~ThumbnailLabel()
{
    if (_loading)
        _loading->deleteLater();
}

void ThumbnailLabel::setThumbnailByOrigin(const QPixmap &origin)
{
    this->setPixmap(ClipService::thumbnailize(origin));
}

void ThumbnailLabel::setThumbnailByOriginPath(const QString &originPath)
{
    _loading = QThread::create([=](void) {
        QPixmap origin(originPath);
        this->setThumbnailByOrigin(origin);
    });
    _loading->start();
}

QImage ThumbnailLabel::image()
{
    return pixmap()->toImage();
}


void ThumbnailLabel::wait()
{
    if (_loading)
        _loading->wait();
}
