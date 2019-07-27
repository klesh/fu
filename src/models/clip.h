#ifndef CLIP_H
#define CLIP_H

#include <QtCore>
#include <QPixmap>

struct Clip
{
    uint id;
    QVariant data;
    bool isImage;
    bool isFile;
    QImage thumbnail;
    QString name;
    QString description;
    QDateTime createdAt;
    QStringList tags;
    quint64 phash;
};

#endif // CLIP_H
