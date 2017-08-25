#ifndef TIME_UTIL_H_
#define TIME_UTIL_H_

#include <QDateTime>
#include <QString>

/* 
Converts a feed timestamp to a QDateTime object including UTC offset.
*/
QDateTime parseDateTime(const QString& datetime);

/*
Returns a new QString with `original` formatted using `format`
*/
QString formatTime(const QString& original, const QString& format);

/*
Returns a string describing the temporal relationship between two QDateTimes
e.g. "1w 3d ago" or "5h 11m ago"
*/
QString relativeTimeBetween(const QDateTime& now, const QDateTime& then);

/*
Returns a short, common timestamp of form Day DD Mon, with year if necessary.
*/
QString commonTimestamp(const QString& datetime);

#endif