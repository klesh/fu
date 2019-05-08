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

    static QMap<QDate, QList<Record>> groupByCreationDate(QList<Record> records) {
        QMap<QDate, QList<Record>> dateRecordsMap;
        for (const Record &record : records) {
            QDate date = record.createdAt.date();
            if (!dateRecordsMap.contains(date)) {
                QList<Record> dateRecords;
                dateRecordsMap.insert(date, dateRecords);
            }
            dateRecordsMap[date].append(record);
        }
        return dateRecordsMap;
    }
};

#endif // RECORD_H
