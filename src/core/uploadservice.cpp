#include "../application.h"
#include "uploadservice.h"
#include "uploadthread.h"

#include <QSqlQuery>
#include <QSqlRecord>
#include <QPainter>
#include <QImageWriter>
#include <QCheckBox>
#include <QPushButton>

inline void query2upload(QSqlQuery &query, Upload &upload) {
    auto rec = query.record();
    upload.id = query.value(rec.indexOf("id")).toUInt();
    upload.clipId = query.value(rec.indexOf("clipId")).toUInt();
    upload.serverId = query.value(rec.indexOf("serverId")).toUInt();
    upload.url = query.value(rec.indexOf("output")).toString();
    upload.createdAt = query.value(rec.indexOf("createdAt")).toDateTime();
    auto serverNameIdx = rec.indexOf("serverName");
    if (!~serverNameIdx) upload.serverName = query.value(serverNameIdx).toString();
}


QList<Upload> UploadService::getAllByClipId(uint clipId)
{
    QSqlQuery query(QString("SELECT uploads.*, servers.name AS serverName "
                            "FROM uploads LEFT JOIN servers ON (uploads.serverId = servers.id) "
                            "WHERE uploads.clipId=%1").arg(clipId));

    assert(query.exec());
    QList<Upload> uploads;
    while (query.next()) {
        Upload upload;
        query2upload(query, upload);
        uploads.append(upload);
    }
    return uploads;
}

void UploadService::upload(QList<Clip> &clips, const QStringList &tags, const QString &desc)
{
    assert (_isUploading == false);
    _isUploading = true;
    APP->clipService()->massAppend(clips, tags, desc);
    upload(clips);
}

void UploadService::upload(QList<Clip> &clips)
{
    auto servers = APP->serverService()->getAllUploadEnabled();
    // prepare image preprocess data
    auto imageCompressionEanbled = APP->settingService()->imageCompressionEnabled();
    auto imageWartermarkEnabled = APP->settingService()->imageWatermarkEnabled();
    QPixmap watermark;
    if (imageWartermarkEnabled)
        watermark.load(APP->settingService()->imageWatermarkPath());
    auto imageWatermarkPos = APP->settingService()->imageWatermarkPosition();

    // build up job list
    for (auto &clip : clips) {
        clip.freePixmap(); // no need pixmap anymore, free it up
        QByteArray imageBytes;

        if (clip.isImage) {
            // detect format and initialize quality
            QByteArray fmt = "jpg";
            int quality = -1;
            auto pixmap = clip.pixmap();
            if (clip.isFile) {
                QFileInfo fi(clip.localPath());
                QByteArray suf = fi.completeSuffix().toLower().toLatin1();
                if (QImageWriter::supportedImageFormats().contains(suf)) {
                    fmt = suf;
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

            // save as bytes
            QBuffer buffer(&imageBytes);
            buffer.open(QIODevice::WriteOnly);
            pixmap.save(&buffer, fmt, quality);
        }

        for (auto &server : servers) {
            UploadJob job;
            job.server = server;
            job.clip = clip;
            if (!imageBytes.isEmpty())
                job.data = imageBytes;
            else
                job.path = clip.localPath();
            job.name = clip.name;
            _jobs.append(job);
        }
    }

    // setup thread pool
    poolSeats = 5;
    populatePool();
}

void UploadService::populatePool()
{
    // make sure only one running at a time
    mutex.lock();
    if (isPopulating) {
        needAnotherPopulation = true;
        mutex.unlock();
        return;
    }
    needAnotherPopulation = false;
    isPopulating = true;
    mutex.unlock();

    for (int i = 0; i < _jobs.size() && poolSeats; i++) {
        auto &job = _jobs[i];
        if (job.status == Duplicated) {
            // deal with file name duplicated on remote server
            handleDuplication(job);
        }
        if (job.status == Pending) {
            // kick start uploading for a job
            job.status = Assigned;
            auto thread = new UploadThread(job);
            connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
            poolSeats--;
        } else if (job.status == Success && job.saved == false) {
            // upload record to database
            QSqlQuery query;
            query.prepare("INSERT INTO uploads (clipId, serverId, output, createdAt) "
                          "VALUES (:clipId, :serverId, :output, :createdAt)");
            query.bindValue(":clipId", job.clip.id);
            query.bindValue(":serverId", job.server.id);
            query.bindValue(":output", job.url);
            query.bindValue(":createdAt", datetimeToISO());
            assert(query.exec());
            job.saved = true;
        } else if (job.status == Error && job.notified == false) {
            job.notified = true;
            APP->sendNotification(job.msg, "fu", QSystemTrayIcon::Warning);
        }
    }

    mutex.lock();
    isPopulating = false;
    mutex.unlock();
    // run another population if any other threads were completed during process
    if (needAnotherPopulation) {
        populatePool();
    } else if (poolSeats == 5) {
        uploadFinished();
    }
}

void UploadService::handleDuplication(UploadJob &job)
{
    DuplicationHandlingMethod method;
    if (job.counter) { // already selected AutoRename for this job before
        method = AutoRename;
    } else if (_applyToAll != None) { // `apply to all` was checked
        method = _applyToAll;
    } else { // pop up a Dialog for user to choose
        auto msgBox = new QMessageBox(QMessageBox::Question,
                                      tr("File exists"),
                                      tr("File %1 already exists on Server %2. What would you like to do?")
                                      .arg(job.name).arg(job.server.name));
        auto btnAutoRename = new QPushButton(tr("Auto rename"), msgBox);
        auto btnOverwrite = new QPushButton(tr("Overwrite"), msgBox);
        auto btnSkip = new QPushButton(tr("Skip"), msgBox);
        msgBox->setCheckBox(new QCheckBox(tr("Apply to all"), msgBox));
        msgBox->addButton(btnAutoRename, QMessageBox::AcceptRole);
        msgBox->addButton(btnOverwrite, QMessageBox::DestructiveRole);
        msgBox->addButton(btnSkip, QMessageBox::RejectRole);

        msgBox->exec();
        if (msgBox->clickedButton() == btnAutoRename)
            method = AutoRename;
        else if (msgBox->clickedButton() == btnOverwrite)
            method = Overwrite;
        else
            method = Skip;
        if (msgBox->checkBox()->checkState() == Qt::Checked)
            _applyToAll = method;
        delete msgBox;
    }

    if (method == AutoRename) {
        QFileInfo fi(job.clip.name);
        job.name = QString("%1-%2.%3")
                .arg(fi.baseName()).arg(++job.counter).arg(fi.completeSuffix());
        job.status = Pending;
    } else if (method == Overwrite) {
        job.overwrite = true;
        job.status = Pending;
    } else if (method == Skip) {
        job.status = Skipped;
    }
}

void UploadService::threadFinished()
{
    poolSeats++;
    populatePool();
}

void UploadService::uploadFinished()
{
    // produce output for Clipboard
    QMap<uint, Format> formats; // cache
    QStringList outputs;
    int success = 0, error = 0, skip = 0;
    for (auto &job : _jobs) {
        if (job.status == Success) {
            success++;
            auto formatId = job.server.formatId;
            if (formatId) {
                if (!formats.contains(formatId))
                    formats[formatId] = APP->formatService()->findById(formatId);
                auto &format = formats[formatId];
                outputs.append(format.generate(job.url, job.clip.description));
            }
        } else if (job.status == Error) {
            error++;
        } else if (job.status == Skipped) {
            skip++;
        }

    }
    _jobs.clear();

    if (outputs.size())
        APP->clipService()->setClipboard(outputs.join("\n"));

    QString notification = tr("File upload completed.\n%1 succeeded, %2 failed and %3 skipped")
            .arg(success).arg(error).arg(skip);
    APP->sendNotification(notification);
    _isUploading = false;
}

