#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H

#include <QtCore>

struct OutputFormat
{
    uint id;
    QString name;
    QString templateTEXT;
    bool enabled;

    QString format(const QString &output, const QString &desc) {
        return templateTEXT.arg(output).arg(desc);
    }
};

#endif // OUTPUTFORMAT_H
