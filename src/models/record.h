#ifndef RECORD_H
#define RECORD_H

#include <QtCore>
#include <QPixmap>

class Record
{
private:
    QString uploadedTo;
    QString desc;
    QStringList tags;
    QDateTime createdAt;
    QDateTime updatedAt;
    QPixmap thumbnail;


public:
    void setUploadedTo(QString value) { uploadedTo = value; }
    QString getUploadedTo() const { return uploadedTo; }

    void setDesc(QString value) { desc = value; }
    QString getDesc() const { return desc; }

    void setTags(QStringList value) { tags = value; }
    QStringList getTags() const { return tags; }

    void setCreatedAt(QDateTime value) { createdAt = value; }
    QDateTime getCreatedAT() const { return createdAt; }

    void setUpdatedAT(QDateTime value) { updatedAt = value; }
    QDateTime getUpdatedAt() const {return updatedAt; }

    void setThumbnail(QPixmap value) { thumbnail = value; }
    QPixmap getThumbnail() const { return thumbnail; }

};

#endif // RECORD_H
