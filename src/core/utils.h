#ifndef UTILS_H
#define UTILS_H

#include <QtCore>

inline QString datetimeToISO(const QDateTime &dt = QDateTime::currentDateTime()) {
    QDateTime dt2(dt);
    dt2.setOffsetFromUtc(dt.offsetFromUtc());
    return dt2.toString(Qt::ISODate);
}

inline QDateTime dateToDateTime(const QDate &date = QDate::currentDate()) {
    QTime time(0,0,0);
    QDateTime datetime(date, time);
    return datetime;
}

inline QString dateToISO(const QDate &date = QDate::currentDate()) {
    return datetimeToISO(dateToDateTime(date));
}

template <typename T>
inline QString join(const QList<T> list, const QString &sep = ", ") {
    QStringList tmp;
    for (auto &item : list) {
        qDebug() << item;
        tmp.append(QString("%1").arg(item));
    }
    return tmp.join(sep);
}

#endif // UTILS_H
