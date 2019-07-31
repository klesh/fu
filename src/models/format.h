#ifndef OUTPUTFORMAT_H
#define OUTPUTFORMAT_H

#include <QtCore>

struct Format
{
    uint id;
    QString name;
    QString format;

    QString genrate(QString &url, QString &desc) {
        return QString(format).arg(url).arg(desc);
    }
};

#endif // OUTPUTFORMAT_H
