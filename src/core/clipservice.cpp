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
    const static auto unknownImg = QPixmap(":icons/unknown-icon.png");

    if (mimeData->hasImage()) {
        Clip clip;
        clip.isFile = false;
        auto image = qvariant_cast<QPixmap>(mimeData->imageData());
        clip.preview = image.scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio);
        clip.data = image;
        clip.isImage = true;
        list.append(clip);
    } else if (mimeData->hasUrls()) {
        for (auto &url : mimeData->urls()) {
            if (!url.isLocalFile())
                continue;

            Clip clip;
            clip.isFile = true;
            auto mimeType = mimeDb.mimeTypeForUrl(url);
            if (mimeType.name().startsWith("image/")) {
                clip.preview = QPixmap(url.path().mid(1)).scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                clip.isImage = true;
            } else {
                clip.preview = unknownImg;
                clip.isImage = false;
            }
            clip.name = url.fileName();
            clip.data = url;
            list.append(clip);
        }
    }

    return list;
}
