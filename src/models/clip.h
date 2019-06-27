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
    QByteArray rawPngThumb;
    QString name;
    QString description;
    QDateTime createdAt;
    QStringList tags;
};

#endif // CLIP_H
