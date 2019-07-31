#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include "../models/clip.h"
#include <QLabel>

class ThumbnailLabel : public QLabel
{
    Q_OBJECT
    QThread *_loading = nullptr;
    Clip *_clip = nullptr ;
    void loadClip();

public:
    ThumbnailLabel(QWidget *parent);
    ThumbnailLabel(QWidget *parent, Clip *clip);
    void setClip(Clip *clip);
};

#endif // THUMBNAILLABEL_H
