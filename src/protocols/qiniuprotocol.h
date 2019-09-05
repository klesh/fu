#ifndef QINIUPROTOCOL_H
#define QINIUPROTOCOL_H

#include <QtCore>
#include "./base.h"

class QiniuUploader: public Uploader
{
    QCurl _curl;
    const QVariantMap _settings;

public:
    QiniuUploader(const QVariantMap settings);
    void upload(QIODevice *stream, UploadJob &job) override;
};

class QiniuProtocol : public Protocol
{
    Q_OBJECT
    QList<ProtocolSettingInfo> _settingInfos;
public:
    QiniuProtocol();

    // Protocol interface
    const QString getName();
    const QString getTitle();
    const QList<ProtocolSettingInfo> &getSettingInfos();
    Uploader *createUploader(const QVariantMap &settings);
};

#endif // QINIUPROTOCOL_H
