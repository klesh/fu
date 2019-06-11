#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H

#include <QtCore>

struct OutputFormat
{
    uint id;
    QString name;
    QString templateTEXT;
    bool enabled;
};

#endif // OUTPUTFORMAT_H
