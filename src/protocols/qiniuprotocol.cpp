#include "qiniuprotocol.h"

QiniuProtocol::QiniuProtocol()
{
    _settingInfos.append({"proxy", tr("Proxy"), tr("socks5://localhost:1080"), Text, false, "", ""});
    _settingInfos.append({"endpoint",
                          tr("Region endpoint"),
                          tr("https://developer.qiniu.com/kodo/manual/1671/region-endpoint"),
                          Text,
                          true,
                          tr("Please enter your Region Endpoint url"),
                          "http://upload.qiniup.com/"
                         });
    _settingInfos.append({"accessKey",
                          tr("AccessKey"),
                          tr("Obtain from https://portal.qiniu.com/user/key"),
                          Text,
                          true,
                          tr("Please enter your qiniu AccessKey"),
                          ""
                         });
    _settingInfos.append({"secretKey",
                          tr("SecretKey"),
                          tr("Obtain from https://portal.qiniu.com/user/key"),
                          Text,
                          true,
                          tr("Please enter your qiniu SecretKey"),
                          ""
                         });
    _settingInfos.append({"bucket",
                          tr("Bucket"),
                          tr(""),
                          Text,
                          true,
                          tr("Please enter your qiniu Bucket name"),
                          ""
                         });
    _settingInfos.append({"domain",
                          tr("Domain"),
                          tr("Bucket domain"),
                          Text,
                          true,
                          tr("Please enter your qiniu Bucket domain"),
                          ""
                         });
    _settingInfos.append({"prefix",
                          tr("Prefix Path"),
                          tr(""),
                          Text,
                          false,
                          tr(""),
                          ""
                         });
}

const QString QiniuProtocol::getName()
{
    return "qiniu";
}

const QString QiniuProtocol::getTitle()
{
    return tr("qiniu.com");
}

const QList<ProtocolSettingInfo> &QiniuProtocol::getSettingInfos()
{
    return _settingInfos;
}

Uploader *QiniuProtocol::createUploader(const QVariantMap &settings)
{
    return new QiniuUploader(settings);
}

QiniuUploader::QiniuUploader(const QVariantMap settings)
    : _curl(QCurl(QUrl(settings["endpoint"].toString()))), _settings(settings)
{
}

void QiniuUploader::upload(QIODevice *stream, UploadJob &job)
{
    // obtain upload token
    const QString putPolicy = QString("{\"scope\":\"%1\",\"deadline\":%2,\"insertOnly\":%3}")
            .arg(_settings["bucket"].toString())
            .arg(QDateTime::currentDateTime().toTime_t()+3600)
            .arg(job.overwrite ? 1 : 0);
    const QString encodedPutPolicy = putPolicy.toUtf8().toBase64(QByteArray::Base64UrlEncoding);

    const QString secretKey = _settings["secretKey"].toString();
    QMessageAuthenticationCode hmac(QCryptographicHash::Sha1, secretKey.toUtf8());
    hmac.addData(encodedPutPolicy.toUtf8());
    const QString encodedSign = QString(hmac.result().toBase64(QByteArray::Base64UrlEncoding));


    const QString accessKey = _settings["accessKey"].toString();
    const QString uploadToken = QString("%1:%2:%3")
            .arg(accessKey)
            .arg(encodedSign)
            .arg(encodedPutPolicy);

    // construct multipart form
    QCurlMultipart parts;
    QString path = job.name;
    auto prefix = _settings["prefix"].toString();
    if (prefix.isEmpty() == false) {
        if (prefix.endsWith('/') == false) prefix.append('/');
        path = prefix + path;
    }
    if (path.startsWith('/')) path.remove(0, 1);
    parts.append({"key", path});
    parts.append({"token", uploadToken});
    parts.append({"file", QVariant::fromValue(stream)});

    auto res = _curl.post(parts);
    if (res.statusCode() == 200) {
        job.status = Success;
        job.url = QString("http://%1/%2").arg(_settings["domain"].toString()).arg(path);
    } else {
        job.status = Error;
        auto json = res.responseJson();
        auto root = json.object();
        job.msg = root["error"].toString();
    }
}
