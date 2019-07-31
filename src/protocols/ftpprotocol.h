#ifndef FTPPROTOCOL_H
#define FTPPROTOCOL_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "../models/protocol.h"


class FtpUploader : public Uploader
{
    QNetworkAccessManager network;
    const QUrl url;
public:
    FtpUploader(const QVariantMap settings);
    ~FtpUploader() override;
    QString upload(QDataStream *stream, const QString name, bool overwrite = false) override;
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
