#include "sftpprotocol.h"
#include <qcurl.h>

SftpProtocol::SftpProtocol()
{
    _settingInfos.append({"proxy", tr("Proxy"), tr("socks5://localhost:1080"), Text, false, "", ""});
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
    : _curl(createQCurl(settings, "sftp"))
{
    _curl.setPrivateKeyPath(settings["keyPath"].toString());
    _curl.setPublicKeyPath(settings["pubkeyPath"].toString());
    _curl.setKeyPassword(settings["keyPass"].toString());
    _outputUrl = settings["outputUrl"].toString();
}

void SftpUploader::upload(QIODevice *stream, UploadJob &job)
{
    if (job.overwrite == false) {
        int exists;
        auto re = _curl.exists(exists, job.name);
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

    auto res = _curl.put(job.name, *stream);

    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        job.status = Success;
        job.url = _outputUrl.arg(job.name);
    }
}
