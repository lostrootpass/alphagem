#pragma once

#include <QObject>
#include <QVector>

#include "Feed.h"

class FeedParser;

class FeedCache : public QObject
{
	Q_OBJECT

public:
	FeedCache(QObject *parent);
	~FeedCache();

	void loadFromDisk();
	void saveToDisk();

	QVector<Feed>& feeds() { return _feeds; }

signals:
	void feedListUpdated();

public slots:
	void onFeedAdded(QString& url);
	void onFeedRetrieved(Feed* feed);


private:
	QVector<Feed> _feeds;

	FeedParser* _feedParser;
};
