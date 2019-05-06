#ifndef QTAG_H
#define QTAG_H

#include <QPushButton>

class QTag : public QPushButton
{
    Q_OBJECT

public:
    QTag(QWidget *parent = nullptr);
    QTag(const QString text, QWidget *parent = nullptr);
};

#endif // QTAG_H
