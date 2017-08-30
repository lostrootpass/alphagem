#include "FeedCache.h"

#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>

#include "FeedParser.h"
#include "OPMLParser.h"

#include "core/Settings.h"
#include "core/TimeUtil.h"
#include "core/Version.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/FeedSettings.h"

QDataStream& operator<<(QDataStream& stream, const Episode* episode)
{
	stream << episode->title;
	stream << episode->description;
	stream << episode->mediaUrl;
	stream << episode->imageUrl;
	stream << episode->guid;
	stream << episode->mediaFormat;
	stream << episode->shareLink;
	stream << episode->author;
	stream << episode->categories;
	stream << episode->published;
	stream << episode->duration;
	stream << episode->isExplicit;
	stream << episode->listened;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, Episode*& episode)
{
	episode = new Episode();

	stream >> episode->title;
	stream >> episode->description;
	stream >> episode->mediaUrl;
	stream >> episode->imageUrl;
	stream >> episode->guid;
	stream >> episode->mediaFormat;
	stream >> episode->shareLink;
	stream >> episode->author;
	stream >> episode->categories;
	stream >> episode->published;
	stream >> episode->duration;
	stream >> episode->isExplicit;
	stream >> episode->listened;

	return stream;
}

QDataStream& operator<<(QDataStream& stream, const Feed* feed)
{
	stream << feed->feedUrl << feed->title << feed->link;
	stream << feed->description << feed->imageUrl;
	stream << feed->creator << feed->ownerName << feed->ownerEmail;
	stream << feed->lastUpdated << feed->episodes << feed->categories;
	stream << feed->lastRefreshTimestamp << feed->isExplicit;
	stream << feed->useGlobalSettings;

	return stream;
}

QDataStream& operator >> (QDataStream& stream, Feed*& feed)
{
	feed = new Feed();
	
	stream >> feed->feedUrl;
	stream >> feed->title;
	stream >> feed->link;
	stream >> feed->description;
	stream >> feed->imageUrl;
	stream >> feed->creator;
	stream >> feed->ownerName;
	stream >> feed->ownerEmail;
	stream >> feed->lastUpdated;
	stream >> feed->episodes;
	stream >> feed->categories;
	stream >> feed->lastRefreshTimestamp;
	stream >> feed->isExplicit;
	stream >> feed->useGlobalSettings;

	return stream;
}

FeedCache::FeedCache(Core* core)
	: QObject(nullptr), _core(core), _feedParser(nullptr)
{
	connect(_core->settings(), &Settings::feedSettingsChanged,
		this, &FeedCache::onFeedSettingsChanged);
}

FeedCache::~FeedCache()
{
	delete _feedParser;

	for (Feed* f : _feeds)
	{
		delete f;
	}
}

Feed* FeedCache::feedForEpisode(const Episode* e)
{
	for (Feed* f : _feeds)
	{
		for (const Episode* ep : f->episodes)
		{
			if (ep == e)
				return f;
		}
	}

	return nullptr;
}

Feed* FeedCache::feedForUrl(const QString& url)
{
	for (Feed* f : _feeds)
	{
		if (f->feedUrl == url)
			return f;
	}

	Feed* f = new Feed();
	f->feedUrl = QString(url);
	_feeds.push_back(f);
	return f;
}

Episode* FeedCache::getEpisode(const QString& guid)
{
	for (const Feed* f : _feeds)
	{
		for (Episode* e : f->episodes)
		{
			if (e->guid == guid)
				return e;
		}
	}

	return nullptr;
}

void FeedCache::onAboutToQuit()
{
	saveToDisk();
}

void FeedCache::onFeedAdded(const QString& url)
{
	if (!_feedParser)
	{
		_feedParser = new FeedParser(this);

		connect(_feedParser, &FeedParser::feedRetrieved,
			this, &FeedCache::onFeedRetrieved);
		connect(_feedParser, &FeedParser::newEpisodeAdded,
			this, &FeedCache::onNewEpisodeAdded);
		connect(_feedParser, &FeedParser::queueParsed,
			this, &FeedCache::onQueueParsed);
	}

	_feedParser->queueFeedDownload(url);
}

void FeedCache::onFeedRetrieved(Feed* feed)
{
	_clearOldEpisodes(feed);

	saveToDisk();

	emit feedListUpdated();

	_core->episodeCache()->updateAutoDownloadQueue(feed);
}

void FeedCache::onOPMLExported(const QString& fileName)
{
	OPMLParser opml(this);
	opml.save(fileName, _feeds);
}

void FeedCache::onOPMLImported(const QString& fileName)
{
	OPMLParser opml(this);
	opml.parse(fileName);

	const QVector<QString>& feedURLs = opml.urls();

	for (const QString& url : feedURLs)
	{
		onFeedAdded(url);
	}
}

void FeedCache::onQueueParsed()
{
	emit queueParsed();
}

void FeedCache::_clearOldEpisodes(Feed* feed)
{
	const FeedSettings& settings = _core->settings()->feed(feed);

	if (!settings.enableThreshold)
		return;

	qint64 secs = settings.ignoreThreshold * 24 * 60 * 60;
	qint64 epoch = QDateTime::currentSecsSinceEpoch();
	qint64 threshold = (epoch - secs);

	QDateTime epTime;
	qint64 epStamp;
	Episode* e;
	QVector<Episode*>::iterator i = feed->episodes.begin();
	while(i != feed->episodes.end())
	{
		e = *i;
		epTime = parseDateTime(e->published);
		epTime.setOffsetFromUtc(0);
		epStamp = epTime.toSecsSinceEpoch();

		if (epStamp < threshold)
		{
			_core->removeEpisode(e);
			i = feed->episodes.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void FeedCache::onFeedSettingsChanged(Feed* feed)
{
	if(feed)
		refresh(feed);
}

void FeedCache::onNewEpisodeAdded(Feed*, Episode* e)
{
	emit newEpisodeAdded(e);
}

void FeedCache::onTimedRefresh()
{
	qDebug() << "Starting scheduled refresh...";

	qint64 epoch = QDateTime::currentSecsSinceEpoch();
	qint64 refreshPeriod = 0;

	for (Feed* f : _feeds)
	{
		const FeedSettings& settings = _core->settings()->feed(f);
		if (settings.enableRefreshPeriod)
		{
			refreshPeriod = settings.refreshPeriod * 60 * 60;
			if (epoch >= f->lastRefreshTimestamp + refreshPeriod)
			{
				refresh(f);
			}
		}
	}
}

void FeedCache::loadFromDisk()
{
	QString path =
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	QString filePath = dir.filePath("feeds.pod");

	QFile file(filePath);
	if(!file.exists())
		return;

	file.open(QIODevice::ReadOnly);
	QDataStream inStream(&file);
	qint64 fileVersion;
	inStream >> fileVersion;
	inStream >> _feeds;

	file.close();

	emit feedListUpdated();
}

void FeedCache::refresh(int index)
{
	if (index < 0 || index >= _feeds.size()) return;

	onFeedAdded(_feeds[index]->feedUrl);
	emit refreshStarted(_feeds[index]);
}

void FeedCache::refresh(Feed* feed)
{
	onFeedAdded(feed->feedUrl);
	emit refreshStarted(feed);
}

void FeedCache::refreshAll()
{
	for (const Feed* f : _feeds)
	{
		onFeedAdded(f->feedUrl);
	}

	emit refreshStarted(nullptr);
}

void FeedCache::removeFeed(int index)
{
	if (index < 0 || index >= _feeds.size()) return;

	for (Episode* e : _feeds[index]->episodes)
	{
		_core->removeEpisode(e);
	}

	_feeds.remove(index);
	saveToDisk();
	
	emit feedListUpdated();
}

void FeedCache::saveToDisk()
{
	QString path = 
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	if (!dir.exists())
		dir.mkdir(".");

	QString filePath = dir.filePath("feeds.pod.tmp");

	QFile file(filePath);
	file.open(QIODevice::WriteOnly);

	QDataStream outStream(&file);
	outStream << VERSION;
	outStream << _feeds;

	file.close();

	QFile::remove(dir.filePath("feeds.pod"));
	QFile::rename(filePath, dir.filePath("feeds.pod"));
}

void FeedCache::startRefreshTimer()
{
	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &FeedCache::onTimedRefresh);

	//Check refreshes once per 30 minutes.
	timer->start(30 * 60 * 1000);
}

void FeedCache::startupRefresh()
{
	for (const Feed* f : _feeds)
	{
		if(_core->settings()->feed(f).refreshAtStartup)
			onFeedAdded(f->feedUrl);
	}
}
