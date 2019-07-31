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

    QByteArray

    QPixmap _pixmap;
    QPixmap _thumbnailPixmap;

    QPixmap pixmap() {
        if (!isImage || !_pixmap.isNull())
            return _pixmap;

        if (isFile) {
            _pixmap.load(data.toUrl().toLocalFile());
        } else {
            _pixmap = qvariant_cast<QPixmap>(data);
        }
        return _pixmap;
    }

    QPixmap thumbnailPixmap() {
        if (!isImage || !_thumbnailPixmap.isNull())
            return _thumbnailPixmap;
        _thumbnailPixmap = pixmap().scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return _thumbnailPixmap;
    }

    QByteArray getThumbnailBytes() {
        QByteArray bytes;
        if (!isImage || !_thumbnailBytes.isNull())
            return bytes;
        auto thumbnail = thumbnailPixmap();
        if (thumbnail.isNull() != true) {
            QBuffer buffer(&bytes);
            thumbnail.save(&buffer, "jpg", 75);
        }
        return bytes;
    }

    void setThumbnailBytes(QByteArray bytes) {
        _thumbnailBytes = bytes;
        _thumbnailPixmap.loadFromData(bytes, "jpg");
    }

    QImage getThumbnailImage() {
        QImage image;
        if (!isImage || !_thumbnailImage.isNull())
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
