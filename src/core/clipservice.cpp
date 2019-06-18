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
    const static auto fileIcon = QApplication::style()->standardIcon(QStyle::SP_FileIcon).pixmap(QSize(THUMB_WIDTH, THUMB_HEIGHT));

    if (mimeData->hasImage()) {
        Clip clip;
        auto image = qvariant_cast<QPixmap>(mimeData->imageData());
        clip.icon = image.scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio);
        clip.data = image;
        clip.isPixmap = true;
        list.append(clip);
    } else if (mimeData->hasUrls()) {
        for (auto &url : mimeData->urls()) {
            if (!url.isLocalFile())
                continue;

            Clip clip;
            auto mimeType = mimeDb.mimeTypeForUrl(url);
            if (mimeType.name().startsWith("image/")) {
                clip.icon = QPixmap(url.path().mid(1)).scaled(THUMB_WIDTH, THUMB_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            } else {
                clip.icon = fileIcon;
            }
            clip.name = url.fileName();
            clip.data = url;
            clip.isPixmap = false;
            list.append(clip);
        }
    }

    return list;
}
