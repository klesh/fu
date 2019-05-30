#ifndef SERVER_H
#define SERVER_H

#include <QtCore>

struct Server
{
    uint id;
    QString name;
    QString protocol;
    QMap<QString, QVariant> settings;
    bool isActived;
    bool isOutputEnabled;
    QDateTime createAt;
};

#endif // SERVER_H
