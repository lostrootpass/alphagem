#ifndef FEED_H_
#define FEED_H_

#include <QString>
#include <QVector>
#include <ctime>

struct Episode
{
	QString title;
	QString description;
	QString mediaUrl;
	QString imageUrl;
	QString guid;
	QString mediaFormat;

	std::time_t published;

	qint32 duration;

	bool listened;
};

struct Feed
{
	QString feedUrl;
	QString title;
	QString link;
	QString description;
	QString imageUrl;

	std::time_t lastUpdated;
	
	QVector<Episode> episodes;
};

#endif