#ifndef CLIP_H
#define CLIP_H

#include <QtCore>
#include <QPixmap>

#define THUMB_WIDTH 160
#define THUMB_HEIGHT 160

struct Clip
{
    uint id;
    QVariant data;
    bool isImage;
    bool isFile;
    QString name;
    QString description;
    QDateTime createdAt;
    QStringList tags;
    quint64 phash;

    // these two are likely to be used multiple time during lifecyle, cache it
    QPixmap _pixmap;
    QPixmap _thumbnailPixmap;

    QPixmap pixmap() {
        if (!isImage || !_pixmap.isNull())
            return _pixmap;

        if (isFile) {
            _pixmap.load(data.toUrl().toLocalFile());
        } else {
            _pixmap = QPixmap::fromImage(qvariant_cast<QImage>(data));
        }
        return _pixmap;
    }

    void freePixmap() {
        if (!_pixmap.isNull())
            _pixmap = QPixmap();
    }

    QPixmap thumbnailPixmap() {
        if (!isImage || !_thumbnailPixmap.isNull())
            return _thumbnailPixmap;
        _thumbnailPixmap = pixmap().scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return _thumbnailPixmap;
    }

    QByteArray getThumbnailBytes() { // only used in database operation
        QByteArray bytes;
        if (!isImage)
            return bytes;
        auto thumbnail = thumbnailPixmap();
        if (thumbnail.isNull() != true) {
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            thumbnail.save(&buffer, "jpg", 75);
        }
        return bytes;
    }

    void setThumbnailBytes(QByteArray bytes) {
        _thumbnailPixmap.loadFromData(bytes, "jpg");
    }

    QImage getThumbnailImage() {
        QImage image;
        if (!isImage)
            return image;
        image = thumbnailPixmap().toImage();
        return image;
    }

    QString localPath() {
        if (isFile)
            return data.toUrl().toLocalFile();
        return "";
    }
};

#endif // CLIP_H
