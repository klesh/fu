#ifndef CLIP_H
#define CLIP_H

#include <QtCore>
#include <QPixmap>

struct Clip
{
    uint id;
    bool isPixmap;
    QVariant data;
    QDateTime createdAt;
    QPixmap icon;
    QString name;
};

#endif // CLIP_H
