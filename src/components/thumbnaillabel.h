#ifndef THUMBNAILLABEL_H
#define THUMBNAILLABEL_H

#include <QLabel>

class ThumbnailLabel : public QLabel
{
    Q_OBJECT
    QThread *_loading = nullptr;

public:
    ThumbnailLabel(QWidget *parent = nullptr);
    ThumbnailLabel(QWidget *parent, const QPixmap &origin);
    ThumbnailLabel(QWidget *parent, const QString &originPath);
    ~ThumbnailLabel();
    void setThumbnailByOrigin(const QPixmap &origin);
    void setThumbnailByOriginPath(const QString &originPath);
    QByteArray createRawPng();
    void wait();
};

#endif // THUMBNAILLABEL_H
