#ifndef LOCALSTORAGEPROTOCOL_H
#define LOCALSTORAGEPROTOCOL_H

#include <QtCore>
#include "../models/protocol.h"


class LocalStorageProtocol : public Protocol
{
    QList<ProtocolSettingInfo> _settingInfos;
public:
    LocalStorageProtocol();

    // Protocol interface
public:
    const QString getName();
    const QString getTitle();
    const QList<ProtocolSettingInfo> &getSettingInfos();
};

#endif // LOCALSTORAGEPROTOCOL_H