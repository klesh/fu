#ifndef FTPPROTOCOL_H
#define FTPPROTOCOL_H

#include <QtCore>
#include "../models/protocol.h"

class FtpProtocol : public Protocol
{
    QList<ProtocolSettingInfo> _settingInfos;
public:
    FtpProtocol();

    // Protocol interface
public:
    const QString getName();
    const QString getTitle();
    const QList<ProtocolSettingInfo> &getSettingInfos();
};

#endif // FTPPROTOCOL_H
