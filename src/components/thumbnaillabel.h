#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QLabel>

class ThumbnailLabel : public QLabel
{
    Q_OBJECT

public:
    ThumbnailLabel(QWidget *parent = nullptr);
    ThumbnailLabel(QWidget *parent, const QPixmap &thumbnail);
};

#endif // THUMBNAILLABEL_H
