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

	Feed* feedForUrl(const QString& url);
	void loadFromDisk();
	void refresh();
	void saveToDisk();

	inline QVector<Feed>& feeds() { return _feeds; }

signals:
	void feedListUpdated();

public slots:
	void onFeedAdded(QString& url);
	void onFeedRetrieved(Feed* feed);


private:
	QVector<Feed> _feeds;

	FeedParser* _feedParser;
};
