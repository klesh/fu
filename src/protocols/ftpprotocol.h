#ifndef FTPPROTOCOL_H
#define FTPPROTOCOL_H

#include <QtCore>
#include "../models/protocol.h"


class FtpUploader : public Uploader
{
public:
    FtpUploader(QVariantMap settings);
    ~FtpUploader() override;
    QString upload(QDataStream *stream, const QString name) override;
};

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
    Uploader *createUploader(QVariantMap &settings);
};

#endif // FTPPROTOCOL_H
