#include "../application.h"
#include "uploadservice.h"
#include "uploadthread.h"

#include <QPainter>
#include <QImageWriter>
#include <QCheckBox>
#include <QPushButton>

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
    auto result = _store.exec(QString("SELECT uploads.*, servers.name AS serverName FROM uploads LEFT JOIN servers ON (uploads.serverId = servers.id) WHERE uploads.clipId=%1").arg(clipId));
    while (result.next()) {
        auto upload = convertResultToUpload(result);
        upload.serverName = result.value(result.record().indexOf("serverName")).toString();
        uploads.append(upload);
    }
    return uploads;
}

void UploadService::handleDuplication(UploadJob &job)
{
    DuplicationHandlingMethod method;
    if (job.counter) { // already select AutoRename for this job
        method = AutoRename;
    } else if (_applyToAll != None) { // apply to all was checked
        method = _applyToAll;
    } else { // pop up a Dialog for user to choose
        auto msgBox = new QMessageBox(QMessageBox::Question,
                                      tr("File exists"),
                                      tr("File %1 already exists on Server %2. What would you like to do?")
                                      .arg(job.server.name).arg(job.clip.name));
        msgBox->setCheckBox(new QCheckBox(tr("Apply to all"), msgBox));
        msgBox->addButton(new QPushButton(tr("Auto rename"), msgBox), QMessageBox::AcceptRole);
        msgBox->addButton(new QPushButton(tr("Overwrite"), msgBox), QMessageBox::DestructiveRole);
        msgBox->addButton(new QPushButton(tr("Skip"), msgBox), QMessageBox::RejectRole);

        int role = msgBox->exec();
        if (role == QMessageBox::AcceptRole)
            method = AutoRename;
        else if (role == QMessageBox::DestructiveRole)
            method = Overwrite;
        else {
            method = Skip;
        }
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

void UploadService::populatePool()
{
    mutex.lock();
    if (isPopulating) {
        needAnotherPopulation = true;
        mutex.unlock();
        return;
    }
    needAnotherPopulation = false;
    isPopulating = true;
    mutex.unlock();

    qDebug() << "populate @" << QThread::currentThreadId();
    for (int i = 0; i < _jobs.size() && poolSeats; i++) {
        auto &job = _jobs[i];
        if (job.status == Duplicated) {
            handleDuplication(job);
        }
        if (job.status == Pending) {
            job.status = Assigned;
            auto thread = new UploadThread(job);
            connect(thread, SIGNAL(finished()), this, SLOT(threadFinished()));
            connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
            poolSeats--;
        }
    }
    mutex.lock();
    isPopulating = false;
    mutex.unlock();
    if (needAnotherPopulation) {
        populatePool();
    } else if (poolSeats == 5) {
        qDebug() << poolSeats << "ALL DONE............";
    }
}

void UploadService::threadFinished()
{
    poolSeats++;
    populatePool();
}

void UploadService::uploadFinished()
{
    APP->clipService()->setClipboard(_formattedOutput);
}

void UploadService::upload(QList<Clip> &clips)
{
    // load all servers
    auto servers = APP->serverService()->getAllUploadEnabled();

    // prepare clips
    auto imageCompressionEanbled = APP->settingService()->imageCompressionEnabled();
    auto imageWartermarkEnabled = APP->settingService()->imageWatermarkEnabled();
    QPixmap watermark;
    if (imageWartermarkEnabled)
        watermark.load(APP->settingService()->imageWatermarkPath());
    auto imageWatermarkPos = APP->settingService()->imageWatermarkPosition();

    for (auto &clip : clips) {
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
            job.data = imageBytes;
            job.name = clip.name;
            _jobs.append(job);
        }
    }

    poolSeats = 5;
    populatePool();


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
