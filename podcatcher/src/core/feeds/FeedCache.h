#pragma once

#include <QObject>
#include <QVector>

#include "Feed.h"
#include "core/Core.h"

class FeedParser;

class FeedCache : public QObject
{
	Q_OBJECT

public:
	FeedCache(Core* core);
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
	void startRefreshTimer();
	void startupRefresh();

	inline QVector<Episode*>& episodes(int index) { return _feeds[index]->episodes; }
	inline QVector<Feed*>& feeds() { return _feeds; }

signals:
	void feedListUpdated();
	void newEpisodeAdded(Episode* e);
	void refreshStarted(Feed* f);

public slots:
	void onAboutToQuit();
	void onFeedAdded(const QString& url);
	void onFeedRetrieved(Feed* feed);
	void onOPMLExported(const QString& fileName);
	void onOPMLImported(const QString& fileName);


private:
	QVector<Feed*> _feeds;

	FeedParser* _feedParser;
	Core* _core;

	void _clearOldEpisodes(Feed* feed);

private slots:
	void onFeedSettingsChanged(Feed* feed);
	void onNewEpisodeAdded(Feed* f, Episode* e);
	void onTimedRefresh();
};
