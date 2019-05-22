#ifndef ERROR_H
#define ERROR_H

#include <QtCore>

class Error
{
    QString _text;
    void *_nested;

public:
    Error(const QString &srcFile, const int &lineNo, const QString &message, void *nested = nullptr);
    QString text();
    void *nested();
};

#define throw_error(arg) throw Error(__FILE__, __LINE__, arg)

#endif // ERROR_H
