#include "../application.h"
#include "uploadservice.h"

#include <QPainter>
#include <QImageWriter>

UploadService::UploadService(SqlStore &store)
    : _store(store)
{

}

void UploadService::upload(QList<Clip> &clips, const QStringList &tags, const QString &desc)
{
    // ensure tags are saved
    QList<uint> tagIds;
    for (auto &tag : tags) {
        tagIds.append(APP->tagService()->findOrAppend(tag));
    }

    auto servers = APP->serverService()->getAllUploadEnabled();
    QPixmap watermark;
    if (APP->settingService()->imageWatermarkEnabled()) {
        watermark.load(APP->settingService()->imageWatermarkPath());
    }
    auto imageWatermarkPos = APP->settingService()->imageWatermarkPosition();
    auto imageCompressionEanbled = APP->settingService()->imageCompressionEnabled();

    // ensure clips are saved
    try {
        for (auto &clip : clips) {
            auto query = _store.prepare("INSERT INTO clips (name, isImage, isFile, preview, description, createdAt)"
                                        " VALUES (:name, :isImage, :isFile, :preview, :description, :createdAt)");
            query.bindValue(":name", clip.name);
            query.bindValue(":isImage", clip.isImage);
            query.bindValue(":isFile", clip.isFile);
            if (clip.isImage) {
                QByteArray bytes;
                QBuffer buffer(&bytes);
                buffer.open(QIODevice::WriteOnly);

                clip.preview.save(&buffer, "PNG");
                query.bindValue(":preview", bytes);
            } else {
                query.bindValue(":preview", NULL);
            }
            query.bindValue(":description", desc);
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

            // only when clip is a image, and preprocess are needed
            QByteArray imageBytes;
            if (clip.isImage && (imageCompressionEanbled || !watermark.isNull())) {
                // prefer png
                char format[10] = "PNG";
                int quality = -1;
                auto pixmap = clip.isFile ? QPixmap(clip.data.toUrl().toLocalFile()) : qvariant_cast<QPixmap>(clip.data);
                if (!clip.name.isEmpty()) {
                    auto ext = clip.name.mid(clip.name.lastIndexOf('.') + 1).toLower().toLatin1();
                    if (!ext.isEmpty() && QImageWriter::supportedImageFormats().contains(ext)) {
                        strcpy(format, ext.data());
                    }
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

                QBuffer buffer(&imageBytes);
                buffer.open(QIODevice::WriteOnly);
                pixmap.save(&buffer, format, quality);
            }

            // upload to servers, shall be rewrite as multi-threaded
            for (auto &server : servers) {
                QDataStream *stream;
                QFile *file = nullptr;
                if (!imageBytes.isEmpty()) { // share same processed image
                    stream = new QDataStream(&imageBytes, QIODevice::ReadOnly);
                } else { // create independent file stream
                    auto filePath = clip.data.toUrl().toLocalFile();
                    file = new QFile(filePath);
                    if (!file->open(QIODevice::ReadOnly)) {
                        APP->sendNotification(tr("Failed to open file %1").arg(filePath), tr("Error"), QSystemTrayIcon::MessageIcon::Warning);
                        continue;
                    }
                    stream = new QDataStream(file);
                }
                server.uploader->upload(stream, clip.name);
                delete stream;
                if (file) {
                    file->close();
                    delete file;
                }
            }

        }
    } catch (...) {

    }

    for (auto &server : servers) {
        delete server.uploader;
    }
}
