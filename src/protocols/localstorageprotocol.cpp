#include "localstorageprotocol.h"

LocalStorageProtocol::LocalStorageProtocol()
{
    _settingInfos.append({"folder", tr("Storage folder"), tr("Click 'Browse' button to select a folder"), Directory, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"output", tr("Output template"), tr("%1 will be replace with your file name"), Text, false, "", "http://localhost/%1"});
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
{
    _folder = settings["folder"].toString();
    _output = settings["output"].toString();
}

void LocalStorageUploader::upload(QIODevice *stream, UploadJob &job)
{
    QDir dir(_folder);
    QString filePath = dir.filePath(job.name);
    QFile outputFile(filePath);
    if (outputFile.exists() && !job.overwrite) {
        job.status = Duplicated;
        return;
    }
    outputFile.open(QIODevice::WriteOnly);
    const size_t BUFFER_SIZE = 1024 * 256;
    char *buffer = new char[BUFFER_SIZE];
    while (!stream->atEnd()) {
        auto len = stream->read(buffer, BUFFER_SIZE);
        outputFile.write(buffer, len);
        qDebug() << "write bytes: " << len;
    }
    outputFile.close();

    delete[] buffer;
    job.status = Success;
    job.url = _output.arg(job.name);
}
