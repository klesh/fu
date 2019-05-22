#ifndef ERRORMESSAGE_H
#define ERRORMESSAGE_H

#include <QMessageBox>

class ErrorMessage : public QMessageBox
{
    Q_OBJECT

public:
    ErrorMessage(QWidget *parent = nullptr);

    static int showFatal(const QString &message, QWidget *parent = nullptr);
};

#endif // ERRORMESSAGE_H
