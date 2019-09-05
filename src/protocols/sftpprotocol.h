#ifndef SFTPPROTOCOL_H
#define SFTPPROTOCOL_H

#include <QtCore>
#include "./base.h"

class SftpUploader : public Uploader
{
    QCurl _curl;
    QVariantMap _settings;

public:
    SftpUploader(const QVariantMap settings);
    void upload(QIODevice *stream, UploadJob &job) override;
};

class SftpProtocol: public Protocol
{
    Q_OBJECT
    QList<ProtocolSettingInfo> _settingInfos;

public:
    SftpProtocol();

    // Protocol interface
    const QString getName();
    const QString getTitle();
    const QList<ProtocolSettingInfo> &getSettingInfos();
    Uploader *createUploader(const QVariantMap &settings);
};

#endif // SFTPPROTOCOL_H
