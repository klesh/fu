#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtCore>

enum SettingDataType {
    Text,
    Integer
};

struct SettingInfo
{
    QString label;
    QString description;
    SettingDataType type;
};

class Protocol : QObject
{
    Q_OBJECT
public:
    virtual ~Protocol() {};
    virtual const QString getName() = 0;
    virtual const QList<SettingInfo> &getSettingInfos() = 0;
};

#endif // PROTOCOL_H
