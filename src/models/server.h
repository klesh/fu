#ifndef SERVER_H
#define SERVER_H

#include <QtCore>

struct Server
{
    uint id;
    QString name;
    QString protocol;
    QVariantMap settings;
    bool uploadEnabled;
    uint outputFormatId;
    QDateTime createAt;
};

#endif // SERVER_H
