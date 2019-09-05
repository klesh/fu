#include "localstorageprotocol.h"

LocalStorageProtocol::LocalStorageProtocol()
{
    _settingInfos.append({"root", tr("Root path"), tr("Root path on local machine, will not be part of output url"), Directory, true, "", ""});
    _settingInfos.append({"folder", tr("Folder"), tr("%1 for Year, %2 for Month, %3 for Day Of Month, part of output url"), Text, false, "", "%1-%2-%3"});
    _settingInfos.append({"outputUrl", tr("Output Url"), tr("%1 for {Folder}/{File name}"), Text, false, "", "http://localhost/%1"});
}

const QString LocalStorageProtocol::getName()
{
    return "local";
}

const QString LocalStorageProtocol::getTitle()
{
    return tr("Local Storage");
}

const QList<ProtocolSettingInfo> &LocalStorageProtocol::getSettingInfos()
{
    return _settingInfos;
}

Uploader *LocalStorageProtocol::createUploader(const QVariantMap &settings)
{
    return new LocalStorageUploader(settings);
}


// uploader
LocalStorageUploader::LocalStorageUploader(QVariantMap settings)
    : _settings(settings)
{
}

void LocalStorageUploader::upload(QIODevice *stream, UploadJob &job)
{
    QString path = formatPath(_settings["folder"].toString(), job.name);
    QString fullpath = joinPath(_settings["root"].toString(), path);

    QFileInfo fi(fullpath);

    if (fi.exists() && !job.overwrite) {
        job.status = Duplicated;
        return;
    }
    if (!fi.dir().exists() && !fi.dir().mkpath(".")) {
        job.status = Error;
        job.msg = tr("Failed to create folder");
        return;
    }

    QFile outputFile(fullpath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        job.status = Error;
        job.msg = tr("Failed to open file for writing.");
        return;
    }
    const size_t BUFFER_SIZE = 1024 * 256;
    char *buffer = new char[BUFFER_SIZE];
    while (!stream->atEnd()) {
        auto len = stream->read(buffer, BUFFER_SIZE);
        outputFile.write(buffer, len);
    }
    outputFile.close();

    delete[] buffer;
    job.status = Success;
    job.url = _settings["outputUrl"].toString().arg(path);
}
