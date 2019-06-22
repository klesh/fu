#ifndef FOCUSWATCHER_H
#define FOCUSWATCHER_H

#include <QtCore>

class FocusWatcher : public QObject
{
    Q_OBJECT

public:
    explicit FocusWatcher(QObject* parent = nullptr) : QObject(parent)
    {
        if (parent)
            parent->installEventFilter(this);
    }
    virtual bool eventFilter(QObject *obj, QEvent *event) override
    {
        Q_UNUSED(obj)
        if (event->type() == QEvent::FocusIn) {
            emit focusChanged(true);
            emit focused();
        } else if (event->type() == QEvent::FocusOut) {
            emit focusChanged(false);
            emit unfocused();
        }

        return false;
    }

Q_SIGNALS:
    void focusChanged(bool in);
    void focused();
    void unfocused();
};

#endif // FOCUSWATCHER_H
