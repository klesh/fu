#ifndef FTPPROTOCOL_H
#define FTPPROTOCOL_H

#include <QtCore>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "./base.h"


class FtpUploader : public Uploader
{
    QNetworkAccessManager _network;
    const QUrl _ftpUrl;
    QString _outputUrl;
public:
    FtpUploader(const QVariantMap settings);

    void upload(QDataStream *stream, UploadJob &job) override;
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
    Uploader *createUploader(const QVariantMap &settings);
};

#endif // FTPPROTOCOL_H
