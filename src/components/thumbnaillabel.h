#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include "../models/clip.h"
#include <QLabel>

class ThumbnailLabel : public QLabel
{
    Q_OBJECT
    QThread *_loading;

public:
    ThumbnailLabel(QWidget *parent = nullptr);
    ThumbnailLabel(QWidget *parent, Clip &clip);
    void loadByClip(Clip &clip);
};

#endif // THUMBNAILLABEL_H
