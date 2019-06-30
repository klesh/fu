#include "../application.h"
#include "clipservice.h"

#include <QClipboard>

Clip convertResultToClip(QSqlQuery &result) {
    Clip clip;
    auto rec = result.record();
    clip.id = result.value(rec.indexOf("id")).toUInt();
    clip.name = result.value(rec.indexOf("name")).toString();
    clip.isImage = result.value(rec.indexOf("isImage")).toBool();
    clip.isFile = result.value(rec.indexOf("isFile")).toBool();
    clip.rawPngThumb = result.value(rec.indexOf("preview")).toByteArray();
    clip.description = result.value(rec.indexOf("description")).toString();
    clip.createdAt = result.value(rec.indexOf("createdAt")).toDateTime();
    return clip;
}

ClipService::ClipService(SqlStore &store)
    : _store(store)
{

}

QList<Clip> ClipService::getAllFromClipboard()
{
    QList<Clip> list;

    const auto clipboard = QApplication::clipboard();
    const auto mimeData = clipboard->mimeData();
    const static QMimeDatabase mimeDb;

    if (mimeData->hasImage()) {
        Clip clip;
        clip.isFile = false;
        auto image = qvariant_cast<QPixmap>(mimeData->imageData());
        clip.data = image;
        clip.isImage = true;
        clip.name = QString("%1.png").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
        list.append(clip);
    } else if (mimeData->hasUrls()) {
        for (auto &url : mimeData->urls()) {
            if (!url.isLocalFile())
                continue;

            Clip clip;
            clip.isFile = true;
            auto mimeType = mimeDb.mimeTypeForUrl(url);
            if (mimeType.name().startsWith("image/")) {
                clip.isImage = true;
            } else {
                clip.isImage = false;
            }
            clip.name = url.fileName();
            clip.data = url;
            list.append(clip);
        }
    }

    return list;
}

void ClipService::massAppend(QList<Clip> &clips, const QList<QString> tags, const QString &desc)
{
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
        query.bindValue(":createdAt",  datetimeToISO(QDateTime::currentDateTime()));

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
}

void ClipService::clean()
{
    _store.exec("DELETE FROM clips");
}

void ClipService::remove(uint clipId)
{
    _store.exec(QString("DELETE FROM clips where id=%1").arg(clipId));
}

void ClipService::setClipboard(const QString &text)
{
    QApplication::clipboard()->setText(text);
}

QList<Clip> ClipService::search(QMap<QString, QVariant> &filter)
{
    QList<Clip> clips;

    QStringList sql, where;
    sql.append("SELECT clips.* FROM clips");


    if (filter.contains("dateFrom")) {
        where.append(QString("DATETIME(clips.createdAt) > DATETIME('%1')").arg(dateToISO(filter["dateFrom"].toDate())));
    }
    if (filter.contains("dateTo")) {
        where.append(QString("DATETIME(clips.createdAt) <= DATETIME('%1')").arg(dateToISO(filter["dateTo"].toDate().addDays(1))));
    }
    if (filter.contains("serverIds")) {
        auto serverIds = qvariant_cast<QList<uint>>(filter["serverIds"]);
        sql.append("LEFT JOIN uploads ON clips.id=uploads.clipId");
        where.append(QString("uploads.serverId IN (%1)").arg(join<uint>(serverIds)));
    }
    if (filter.contains("tags")) {
        auto tags = qvariant_cast<QStringList>(filter["tags"]);
        auto tagIds = APP->tagService()->mapToIds(tags);
        sql.append("LEFT JOIN clips_tags ON clips.id=clips_tags.clipId");
        where.append(QString("clips_tags.tagId IN (%1)").arg(join<uint>(tagIds)));
    }

    if (!where.empty()) {
        sql.append("WHERE");
        sql.append(where.join(" AND "));
    }

    sql.append("ORDER BY id DESC");
    auto sqlText = sql.join(" ");

    auto result = _store.exec(sqlText);
    while (result.next()) {
        clips.append(convertResultToClip(result));
    }

    for (auto &clip : clips) {
        auto tagsResult = _store.exec(QString("SELECT tags.name FROM tags LEFT JOIN clips_tags ON (tags.id=clips_tags.tagId) WHERE clips_tags.clipId=%1").arg(clip.id));
        while (tagsResult.next()) {
            clip.tags.append(tagsResult.value(0).toString());
        }
    }

    return clips;
}

QList<QPair<QDate, QList<Clip>>> ClipService::searchAndGroup(QMap<QString, QVariant> &filter)
{
    auto clips = search(filter);
    auto datedClips = groupByCreationDate(clips);
    return datedClips;
}

QPixmap ClipService::thumbnailize(const QPixmap &origin)
{
    return origin.scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

const QPixmap &ClipService::unkownFileIcon()
{
    const static QPixmap unknownImg(":icons/file.svg");
    return unknownImg;
}

QList<QPair<QDate, QList<Clip>>> ClipService::groupByCreationDate(QList<Clip> &clips)
{
    QList<QPair<QDate, QList<Clip>>> datedClips;
    for (auto &clip : clips) {
        QDate date = clip.createdAt.date();
        if (datedClips.empty() || datedClips.last().first != date) {
        //if (!datedClips.contains(date)) {
            QPair<QDate, QList<Clip>> pair;
            QList<Clip> clips;
            pair.first = date;
            pair.second = clips;
            datedClips.append(pair);
        }
        datedClips.last().second.append(clip);
    }
    return datedClips;
}
