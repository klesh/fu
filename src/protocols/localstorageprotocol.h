#ifndef LOCALSTORAGEPROTOCOL_H
#define LOCALSTORAGEPROTOCOL_H

#include <QtCore>
#include "../models/protocol.h"


class LocalStorageProtocol : public Protocol
{
public:
    LocalStorageProtocol();

    // Protocol interface
public:
    const QString getName();
    const QList<SettingInfo> &getSettingInfos();
};

#endif // LOCALSTORAGEPROTOCOL_H
