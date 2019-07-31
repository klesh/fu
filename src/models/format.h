#ifndef FORMAT_H
#define FORMAT_H

#include <QtCore>

struct Format
{
    uint id;
    QString name;
    QString format;

    QString generate(const QString &url, const QString &desc) {
        return QString(format).arg(url).arg(desc);
    }
};

#endif // FORMAT_H
