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

protected:
    QLayout *laySelected;
    QComboBox *cbxPicker;
};

#endif // TAGSEDIT_H
