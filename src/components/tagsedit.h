#ifndef TAGSEDIT_H
#define TAGSEDIT_H

#include <QFrame>
#include <QComboBox>
#include <QScrollArea>


class TagsEdit : public QFrame
{
    Q_OBJECT

public:
    TagsEdit(QWidget *parnet = nullptr);
    //~TagsEdit();
    bool isTagSelected(const QString &);

protected:
    QScrollArea *sclSelected;
    QLayout *laySelected;
    QComboBox *cbxPicker;
    void keyPressEvent(QKeyEvent *) override;

public slots:
    void selectTag(const QString &);
    void deselectTag(const QString &);
};

#endif // TAGSEDIT_H
