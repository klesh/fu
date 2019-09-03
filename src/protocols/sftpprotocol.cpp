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
    _settingInfos.append({"root", tr("Root path"), tr("Root path on remote server, will not be part of output url"), Text, false, "", ""});
    _settingInfos.append({"folder", tr("Folder"), tr("%1 for Year, %2 for Month, %3 for Day Of Month, part of output url"), Text, false, "", "%1-%2-%3"});
    _settingInfos.append({"outputUrl", tr("Output Url"), tr("%1 for {Folder}/{File name}"), Text, false, "", "http://localhost/%1"});
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
    : _curl(createQCurl(settings, "sftp")), _settings(settings)
{
    _curl.setPrivateKeyPath(settings["keyPath"].toString());
    _curl.setPublicKeyPath(settings["pubkeyPath"].toString());
    _curl.setKeyPassword(settings["keyPass"].toString());
}

void SftpUploader::upload(QIODevice *stream, UploadJob &job)
{
    QString path = formatPath(_settings["folder"].toString(), job.name);
    QString fullpath = joinPath(_settings["root"].toString(), path);

    if (job.overwrite == false) {
        int exists;
        auto re = _curl.exists(exists, fullpath);
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

    auto res = _curl.put(fullpath, *stream);

    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        job.status = Success;
        job.url = _settings["outputUrl"].toString().arg(path);
    }
}
