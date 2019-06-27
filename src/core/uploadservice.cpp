#include "../application.h"
#include "uploadservice.h"
#include "uploadthread.h"

#include <QPainter>
#include <QImageWriter>


UploadService::UploadService(SqlStore &store)
    : _store(store)
{

}

void UploadService::upload(QList<Clip> &clips, const QStringList &tags, const QString &desc)
{
    assert (_isUploading == false);

    _isUploading = true;


    // save clips and tags
    QList<uint> tagIds;
    for (auto &tag : tags) {
        tagIds.append(APP->tagService()->findOrAppend(tag));
    }

    for (auto &clip : clips) {
        clip.description = desc;
        auto query = _store.prepare("INSERT INTO clips (name, isImage, isFile, preview, description, createdAt)"
                                    " VALUES (:name, :isImage, :isFile, :preview, :description, :createdAt)");
        query.bindValue(":name", clip.name);
        query.bindValue(":isImage", clip.isImage);
        query.bindValue(":isFile", clip.isFile);
        query.bindValue(":preview", clip.rawPngThumb);
        query.bindValue(":description", clip.description);
        query.bindValue(":createdAt",  QDateTime::currentDateTime().toString(Qt::ISODate));

        auto result = _store.exec();
        clip.id = result.lastInsertId().toUInt();

        // save relationship between clip and tags
        for (auto &tagId: tagIds) {
            query = _store.prepare("INSERT INTO clips_tags (clipId, tagId) VALUES (:clipId, :tagId)");
            query.bindValue(":clipId", clip.id);
            query.bindValue(":tagId", tagId);
            _store.exec();
        }

    }

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

        auto query = _store.prepare("INSERT INTO uploads (clipId, serverId, output, createdAt) VALUES (:clipId, :serverId, :output, :createdAt)");
        query.bindValue(":clipId", uploadThread->clip().id);
        query.bindValue(":serverId", uploadThread->server().id);
        query.bindValue(":output", uploadThread->output());
        query.bindValue(":createdAt",  QDateTime::currentDateTime().toString(Qt::ISODate));
        query.exec();

        if (uploadThread->server().outputFormatId) {
            formattedOutputs.append(uploadThread->server().outputFormat.templateTEXT.arg(uploadThread->output()).arg(uploadThread->clip().description));
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
