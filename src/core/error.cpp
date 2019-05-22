#include "error.h"


Error::Error(const QString &srcFile, const int &lineNo, const QString &message, void *nested)
{
    _text = QString("FILE: %1\n"
                    "LINE: %2\n\n"
                    "MESSAGE: \n%3").arg(srcFile).arg(lineNo).arg(message);
    _nested = nested;
}

QString Error::text()
{
    return _text;
}

void *Error::nested()
{
    return _nested;
}
