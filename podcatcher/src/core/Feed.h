#ifndef FEED_H_
#define FEED_H_

#include <QString>
#include <QVector>
#include <QDate>

struct Episode
{
	QString title;
	QString description;
	QString mediaUrl;
	QString imageUrl;
	QString guid;

	QDate published;

	qint32 duration;
};

struct Feed
{
	QString title;
	QString link;
	QString description;
	QString imageUrl;

	QDate lastUpdated;
	
	QVector<Episode> episodes;
};

#endif