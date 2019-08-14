#include "sftpprotocol.h"
#include <qcurl.h>

SftpProtocol::SftpProtocol()
{
    _settingInfos.append({"host", tr("Host"), tr("FTP server host name"), Text, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"port", tr("Port"), tr(""), Integer, true, "", 22});
    _settingInfos.append({"user", tr("Username"), tr(""), Text, true, "", ""});
    _settingInfos.append({"pass", tr("Password"), tr(""), Text, true, "", ""});
    _settingInfos.append({"keyPath", tr("Private Key"), tr(""), File, false, "", ""});
    _settingInfos.append({"keyPass", tr("Key Password"), tr(""), Text, false, "", ""});
    _settingInfos.append({"pubkeyPath", tr("Public Key"), tr(""), File, false, "", ""});
    _settingInfos.append({"path", tr("Remote Path"), tr(""), Text, true, "", ""});
    _settingInfos.append({"outputUrl", tr("Output Url"), tr(""), Text, true, "", "http://localhost/%1"});
}

const QString SftpProtocol::getName()
{
    return "sftp";
}

const QString SftpProtocol::getTitle()
{
    return tr("SFTP");
}

const QList<ProtocolSettingInfo> &SftpProtocol::getSettingInfos()
{
    return _settingInfos;
}

Uploader *SftpProtocol::createUploader(const QVariantMap &settings)
{
    return new SftpUploader(settings);
}

SftpUploader::SftpUploader(const QVariantMap settings)
    : _sftpUrl(createUrlFromSettings(settings))
{
    _keyPath = settings["keyPath"].toString();
    _keyPass = settings["keyPass"].toString();
    _pubkeyPath = settings["pubkeyPath"].toString();
    _outputUrl = settings["outputUrl"].toString();
}

void SftpUploader::upload(QDataStream *stream, UploadJob &job)
{
    QCurl curl(_sftpUrl);

    if (!_keyPath.isEmpty())
        curl.setSshKeyFiles(_keyPath, _pubkeyPath, _keyPass);

    if (job.overwrite == false && curl.exists(job.name) == 1) {
        job.status = Duplicated;
        return;
    }

    auto res = curl.put(job.name, *stream->device());

    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        job.status = Success;
        job.url = _outputUrl.arg(job.name);
    }
}
