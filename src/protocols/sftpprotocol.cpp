#include "sftpprotocol.h"
#include <qcurl.h>

SftpProtocol::SftpProtocol()
{
    _settingInfos.append({"host", tr("Host"), tr("FTP server host name"), Text, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"port", tr("Port"), tr(""), Integer, true, tr("Please enter port number"), 22});
    _settingInfos.append({"user", tr("Username"), tr(""), Text, true, tr("Please enter login user name"), ""});
    _settingInfos.append({"pass", tr("Password"), tr(""), Text, true, tr("Please enter login password"), ""});
    _settingInfos.append({"keyPath", tr("Private Key"), tr(""), File, false, "", ""});
    _settingInfos.append({"keyPass", tr("Key Password"), tr(""), Text, false, "", ""});
    _settingInfos.append({"pubkeyPath", tr("Public Key"), tr(""), File, false, "", ""});
    _settingInfos.append({"path", tr("Remote Path"), tr(""), Text, false, "", ""});
    _settingInfos.append({"outputUrl", tr("Output Url"), tr(""), Text, false, "", "http://localhost/%1"});
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
    : _sftpUrl(createUrlFromSettings(settings, "sftp"))
{
    _keyPath = settings["keyPath"].toString();
    _keyPass = settings["keyPass"].toString();
    _pubkeyPath = settings["pubkeyPath"].toString();
    _outputUrl = settings["outputUrl"].toString();
}

void SftpUploader::upload(QDataStream *stream, UploadJob &job)
{
    QCurl curl(_sftpUrl);

//    if (!_keyPath.isEmpty())
//        curl.setSshKeyFiles(_keyPath, _pubkeyPath, _keyPass);

    if (job.overwrite == false) {
        int exists;
        auto re = curl.exists(exists, job.name);
        if (exists == 1) {
            job.status = Duplicated;
            return;
        }
        if (exists == -1) {
            job.status = Error;
            job.msg = re.message();
            return;
        }
    }

    curl.setVerbose(true);
    auto res = curl.put(job.name, *stream->device());

    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        job.status = Success;
        job.url = _outputUrl.arg(job.name);
    }
}
