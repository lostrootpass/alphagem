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
	QString shareLink;

	QVector<QString> categories;

	std::time_t published;

	qint32 duration;

	bool isExplicit;
	bool listened;
};

struct Feed
{
	QString feedUrl;
	QString title;
	QString link;
	QString description;
	QString imageUrl;

	QString creator;
	QString ownerName;
	QString ownerEmail;

	std::time_t lastUpdated;

	QVector<Episode> episodes;
	QVector<QString> categories;

	bool isExplicit;
};

#endif