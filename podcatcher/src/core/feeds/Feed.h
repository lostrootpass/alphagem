#ifndef FEED_H_
#define FEED_H_

#include <QObject>
#include <QString>
#include <QVector>
#include <ctime>

#include "FeedSettings.h"

struct Episode : public QObject
{
	Q_OBJECT;
public:
	QString title;
	QString description;
	QString encodedContent;
	QString mediaUrl;
	QString imageUrl;
	QString guid;
	QString mediaFormat;
	QString shareLink;
	QString author;
	QString published;

	QVector<QString> categories;

	qint32 duration;

	bool isExplicit;
	bool listened;

	inline void setListened(bool l = true)
	{
		listened = l;
		emit updated();
	}

signals:
	void updated();
};

struct Feed
{
	Feed() : useGlobalSettings(true) {}
	~Feed()
	{
		for (Episode* e : episodes)
		{
			delete e;
		}
	}

	FeedSettings settings;

	QString feedUrl;
	QString title;
	QString link;
	QString description;
	QString imageUrl;

	QString creator;
	QString ownerName;
	QString ownerEmail;

	QString lastUpdated;

	QVector<Episode*> episodes;
	QVector<QString> categories;

	qint64 lastRefreshTimestamp;

	bool isExplicit;
	bool useGlobalSettings;

	int countUnplayed()
	{
		//Could probably cache this, but need to handle updates from the eps.
		int c = 0;

		for (const Episode* e : episodes)
		{
			if (!e->listened)
				++c;
		}

		return c;
	}
};

#endif