#ifndef PREVIEWBOX_H
#define PREVIEWBOX_H

#include <QFrame>
#include <QLabel>
#include <QStringList>

class PreviewBox : public QFrame
{
    Q_OBJECT

public:
    PreviewBox(QWidget *parent = nullptr);

    void setImage(const QString &url);
    void setUploadedTo(const QString &serverName);
    void setTags(const QStringList &tags);

protected:
    void mousePressEvent(QMouseEvent *evt) override;
    QLabel *previewImg;
    QLabel *uploadedTo;
    QFrame *tagsFrame;
};

#endif // PREVIEWBOX_H
