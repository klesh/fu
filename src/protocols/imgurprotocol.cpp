#include "imgurprotocol.h"

ImgurProtocol::ImgurProtocol()
{
    _settingInfos.append({"proxy", tr("Proxy"), tr("socks5://localhost:1080"), Text, false, "", ""});
    _settingInfos.append({"clientId",
                          tr("Client ID"),
                          tr("Obtain from https://imgur.com/account/settings/apps"),
                          Text,
                          true,
                          tr("Please enter your imgur.com Client ID"),
                          ""
                         });
}

const QString ImgurProtocol::getName()
{
    return "imgur";
}

const QString ImgurProtocol::getTitle()
{
    return "imgur.com";
}

const QList<ProtocolSettingInfo> &ImgurProtocol::getSettingInfos()
{
    return _settingInfos;
}

Uploader *ImgurProtocol::createUploader(const QVariantMap &settings)
{
    return new ImgurUploader(settings);
}

ImgurUploader::ImgurUploader(const QVariantMap settings)
    : _curl(QCurl(QUrl("https://api.imgur.com/3/image")))
{
    _curl.setProxyUrl(settings["proxy"].toString());
    _curl.setHeaders({{"Authorization", QString("Client-ID %1").arg(settings["clientId"].toString())}});
    _curl.setVerbose(true);
}

void ImgurUploader::upload(QIODevice *stream, UploadJob &job)
{
    qDebug() << "at end" << stream->atEnd() << stream->size();
    QCurlMultipart form;
    form.append({"image", QVariant::fromValue(stream)});
    form.append({"title", job.name});
    form.append({"description", job.clip.description});
    auto res = _curl.post(form);
    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        auto json = res.responseJson();
        auto root = json.object();
        auto data = root["data"].toObject();
        if (root["success"].toBool()) {
            job.status = Success;
            job.url = data["link"].toString();
        } else {
            job.status = Error;
            job.msg = data["error"].toString();
        }
    }
}
