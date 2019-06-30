#include "../application.h"
#include "uploadservice.h"
#include "uploadthread.h"

#include <QPainter>
#include <QImageWriter>

Upload convertResultToUpload(QSqlQuery &result) {
    Upload upload;
    auto rec = result.record();
    upload.id = result.value(rec.indexOf("id")).toUInt();
    upload.clipId = result.value(rec.indexOf("clipId")).toUInt();
    upload.serverId = result.value(rec.indexOf("serverId")).toUInt();
    upload.rawOutput = result.value(rec.indexOf("output")).toString();
    upload.createdAt = result.value(rec.indexOf("createdAt")).toDateTime();
    return upload;
}


UploadService::UploadService(SqlStore &store)
    : _store(store)
{

}

void UploadService::upload(QList<Clip> &clips, const QStringList &tags, const QString &desc)
{
    assert (_isUploading == false);

    _isUploading = true;

    // save clips and tags
    APP->clipService()->massAppend(clips, tags, desc);

    // start a new thread in charge of whole upload process
    auto thread = QThread::create([=](void) {
        upload(clips);
        _isUploading = false;
    });

    // callback in main thread in order to update clipboard properly.
    connect(thread, SIGNAL(finished()), this, SLOT(uploadFinished()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

QList<Upload> UploadService::getAllByClipId(uint clipId)
{
    QList<Upload> uploads;
    auto result = _store.exec(QString("SELECT * FROM uploads WHERE clipId=%1").arg(clipId));
    while (result.next()) {
        auto server = convertResultToUpload(result);
        uploads.append(server);
    }
    return uploads;
}

void UploadService::uploadFinished()
{
    APP->clipService()->setClipboard(_formattedOutput);
}

void UploadService::upload(const QList<Clip> &clips)
{
    // upload all clips to all enabled servers;
    auto servers = APP->serverService()->getAllUploadEnabled();
    QPixmap watermark;
    if (APP->settingService()->imageWatermarkEnabled()) {
        watermark.load(APP->settingService()->imageWatermarkPath());
    }
    auto imageWatermarkPos = APP->settingService()->imageWatermarkPosition();
    auto imageCompressionEanbled = APP->settingService()->imageCompressionEnabled();


    QList<UploadThread*> uploadThreads;
    QList<QByteArray*> imageBytesPtrs;

    // ensure clips are saved
    try {
        for (auto &clip : clips) {
            // only when clip is a image, and preprocess are needed
            QByteArray *imageBytes = nullptr;
            if (clip.isImage && (imageCompressionEanbled || !watermark.isNull())) {
                // prefer png
                char format[10] = "PNG";
                int quality = -1;
                auto pixmap = clip.isFile ? QPixmap(clip.data.toUrl().toLocalFile()) : qvariant_cast<QPixmap>(clip.data);
                auto ext = clip.name.mid(clip.name.lastIndexOf('.') + 1).toLower().toLatin1();
                if (!ext.isEmpty() && QImageWriter::supportedImageFormats().contains(ext)) {
                    strcpy(format, ext.data());
                }

                // composite watermark
                int x = 0, y = 0;
                if (!watermark.isNull() && pixmap.height() > watermark.height() && pixmap.width() > watermark.width()) {
                    if (imageWatermarkPos.contains("Middle"))
                        y = (pixmap.height() - watermark.height()) / 2;
                    else if (imageWatermarkPos.contains("Bottom"))
                        y = pixmap.height() - watermark.height();

                    if (imageWatermarkPos.contains("Center"))
                        x = (pixmap.width() - watermark.width()) / 2;
                    else if (imageWatermarkPos.contains("Right"))
                        x = pixmap.width() - watermark.width();

                    QPainter painter(&pixmap);
                    painter.drawPixmap(x, y, watermark.width(), watermark.height(), watermark);
                }

                // perform compression
                if (imageCompressionEanbled) {
                    quality = 70;
                }

                imageBytes = new QByteArray();
                QBuffer buffer(imageBytes);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer, format, quality);
                imageBytesPtrs.append(imageBytes);
            }

            // upload to servers, shall be rewrite as multi-threaded
            for (auto &server : servers) {
                auto uploadThread = new UploadThread(server, clip, *imageBytes);
                uploadThread->start();
                uploadThreads.append(uploadThread);
            }

        }
    } catch (...) {

    }


    int suceed = 0, fail = 0;
    QStringList formattedOutputs;
    for (auto &uploadThread : uploadThreads) {
        uploadThread->wait();
        if (uploadThread->isSuccess()) {
            suceed++;
        } else {
            fail++;
        }

        Server server = uploadThread->server();
        Clip clip = uploadThread->clip();

        Upload upload;
        upload.clipId = clip.id;
        upload.serverId = server.id;
        upload.rawOutput = uploadThread->output();
        upload.createdAt = QDateTime::currentDateTime();
        auto query = _store.prepare("INSERT INTO uploads (clipId, serverId, output, createdAt) VALUES (:clipId, :serverId, :output, :createdAt)");
        query.bindValue(":clipId", upload.clipId);
        query.bindValue(":serverId", upload.serverId);
        query.bindValue(":output", upload.rawOutput);
        query.bindValue(":createdAt", datetimeToISO(upload.createdAt));
        query.exec();

        if (uploadThread->server().outputFormatId) {

            formattedOutputs.append(OutputFormatService::format(server.outputFormat, clip, upload));
        }

        delete  uploadThread;
    }

    for (auto &server : servers) {
        delete server.uploader;
    }
    for (auto &imageBytes : imageBytesPtrs) {
        delete imageBytes;
    }
    _formattedOutput = formattedOutputs.join("\n");
    APP->sendNotification(tr("File upload completed.\n%1 suceeded, %2 failed").arg(suceed).arg(fail));
}
