#ifndef PREVIEWBOX_H
#define PREVIEWBOX_H

#include <QFrame>
#include <QLabel>
#include <QStringList>
#include <QMouseEvent>

class PreviewBox : public QFrame
{
    Q_OBJECT

public:
    PreviewBox(QWidget *parent = nullptr);
    //~PreviewBox();

    void setImage(const QPixmap &thumbnail);
    void setName(const QString &name);
    void setTags(const QStringList &tags);
    bool isSelected();

protected:
    void mousePressEvent(QMouseEvent *evt) override;
    QLabel *previewImg;
    QLabel *name;
    QFrame *tagsFrame;
    bool selected;
};

#endif // PREVIEWBOX_H
