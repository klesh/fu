#ifndef TAGSEDIT_H
#define TAGSEDIT_H

#include "focuswatcher.h"

#include <QFrame>
#include <QComboBox>
#include <QScrollArea>


class TagsEdit : public QFrame
{
    Q_OBJECT

public:
    TagsEdit(QWidget *parnet = nullptr);
    ~TagsEdit() override;
    bool isTagSelected(const QString &);
    QStringList tags();

protected:
    FocusWatcher *focusWatcher;
    QScrollArea *sclSelected;
    QLayout *laySelected;
    QComboBox *cbxPicker;
    void keyPressEvent(QKeyEvent *) override;

public slots:
    void selectTag(const QString &);
    void deselectTag(const QString &);
    void finishUp();
};

#endif // TAGSEDIT_H
