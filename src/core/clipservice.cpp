#include "../application.h"
#include "clipservice.h"

#include <QClipboard>

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

void ClipService::setClipboard(const QString &text)
{
    QApplication::clipboard()->setText(text);
}

QPixmap ClipService::thumbnailize(const QPixmap &origin)
{
    return origin.scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

const QPixmap &ClipService::unkownFileIcon()
{
    const static QPixmap unknownImg(":icons/unknown-icon.png");
    return unknownImg;
}

