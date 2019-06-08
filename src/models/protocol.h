#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtCore>

enum ProtocolSettingDataType {
    Text,
    Integer,
    Directory
};

struct ProtocolSettingInfo
{
    QString name;
    QString label;
    QString description;
    ProtocolSettingDataType type;
    bool required;
    QString hint;
    QVariant defaultValue;
};

class Protocol : QObject
{
    Q_OBJECT
public:
    virtual ~Protocol() {}
    virtual const QString getName() = 0;
    virtual const QString getTitle() = 0;
    virtual const QList<ProtocolSettingInfo> &getSettingInfos() = 0;
};

#endif // PROTOCOL_H
