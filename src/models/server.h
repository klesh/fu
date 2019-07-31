#ifndef SERVER_H
#define SERVER_H

#include "format.h"
#include <QtCore>

struct Server
{
    uint id;
    QString name;
    QString protocol;
    QVariantMap settings;
    bool uploadEnabled;
    uint formatId;
    QDateTime createdAt;
};

#endif // SERVER_H
