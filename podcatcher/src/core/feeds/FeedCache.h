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

	Feed* feedForEpisode(const Episode* e);
	Feed* feedForUrl(const QString& url);
	Episode* getEpisode(const QString& guid);
	void loadFromDisk();
	void refresh(Feed* feed);
	void refresh(int index);
	void refreshAll();
	void removeFeed(int index);
	void saveToDisk();

	inline QVector<Episode*>& episodes(int index) { return _feeds[index]->episodes; }
	inline QVector<Feed*>& feeds() { return _feeds; }

signals:
	void feedListUpdated();

public slots:
	void onAboutToQuit();
	void onFeedAdded(const QString& url);
	void onFeedRetrieved(Feed* feed);
	void onOPMLExported(const QString& fileName);
	void onOPMLImported(const QString& fileName);


private:
	QVector<Feed*> _feeds;

	FeedParser* _feedParser;
};
