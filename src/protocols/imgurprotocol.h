#ifndef IMGURPROTOCOL_H
#define IMGURPROTOCOL_H

#include <QtCore>
#include "./base.h"

class ImgurUploader: public Uploader
{
    QCurl _curl;
public:
    ImgurUploader(const QVariantMap settings);
    void upload(QIODevice *stream, UploadJob &job) override;
};

class ImgurProtocol : public Protocol
{
    Q_OBJECT
    QList<ProtocolSettingInfo> _settingInfos;
public:
    ImgurProtocol();

    // Protocol interface
    const QString getName();
    const QString getTitle();
    const QList<ProtocolSettingInfo> &getSettingInfos();
    Uploader *createUploader(const QVariantMap &settings);
};

#endif // IMGURPROTOCOL_H
