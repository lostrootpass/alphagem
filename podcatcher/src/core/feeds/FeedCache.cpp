#include "FeedCache.h"

#include <QDataStream>
#include <QDir>
#include <QStandardPaths>

#include "FeedParser.h"

static const qint64 VERSION = 0x00000001;

QDataStream& operator<<(QDataStream& stream, const Episode& episode)
{
	stream << episode.title;
	stream << episode.description;
	stream << episode.mediaUrl;
	stream << episode.imageUrl;
	stream << episode.guid;
	stream << episode.mediaFormat;
	stream << episode.shareLink;
	stream << episode.categories;
	stream << episode.published;
	stream << episode.duration;
	stream << episode.isExplicit;
	stream << episode.listened;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, Episode& episode)
{
	stream >> episode.title;
	stream >> episode.description;
	stream >> episode.mediaUrl;
	stream >> episode.imageUrl;
	stream >> episode.guid;
	stream >> episode.mediaFormat;
	stream >> episode.shareLink;
	stream >> episode.categories;
	stream >> episode.published;
	stream >> episode.duration;
	stream >> episode.isExplicit;
	stream >> episode.listened;

	return stream;
}

QDataStream& operator<<(QDataStream& stream, const Feed& feed)
{
	stream << feed.feedUrl << feed.title << feed.description << feed.imageUrl;
	stream << feed.creator << feed.ownerName << feed.ownerEmail;
	stream << feed.lastUpdated << feed.episodes << feed.categories;
	stream << feed.isExplicit;
	return stream;
}

QDataStream& operator >> (QDataStream& stream, Feed& feed)
{
	stream >> feed.feedUrl;
	stream >> feed.title;
	stream >> feed.description;
	stream >> feed.imageUrl;
	stream >> feed.creator;
	stream >> feed.ownerName;
	stream >> feed.ownerEmail;
	stream >> feed.lastUpdated;
	stream >> feed.episodes;
	stream >> feed.categories;
	stream >> feed.isExplicit;

	return stream;
}

FeedCache::FeedCache(QObject *parent)
	: QObject(parent), _feedParser(nullptr)
{
}

FeedCache::~FeedCache()
{
}

Feed* FeedCache::feedForUrl(const QString& url)
{
	for (Feed& f : _feeds)
	{
		if (f.feedUrl == url)
			return &f;
	}

	Feed f;
	f.feedUrl = QString(url);
	_feeds.push_back(f);
	return &_feeds.back();
}

void FeedCache::onAboutToQuit()
{
	saveToDisk();
}

void FeedCache::onFeedAdded(QString& url)
{
	if (!_feedParser)
	{
		_feedParser = new FeedParser(this);

		connect(_feedParser, &FeedParser::feedRetrieved,
			this, &FeedCache::onFeedRetrieved);
	}

	_feedParser->parseFromRemoteFile(url);
}

void FeedCache::onFeedRetrieved(Feed*)
{
	saveToDisk();

	emit feedListUpdated();
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

	emit feedListUpdated();
}

void FeedCache::refresh(int index)
{
	if (index < 0 || index >= _feeds.size()) return;

	onFeedAdded(_feeds[index].feedUrl);
}

void FeedCache::removeFeed(int index)
{
	if (index < 0 || index >= _feeds.size()) return;

	//For now, don't remove associated data, just the feed.
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

	QString filePath = dir.filePath("feeds.pod");

	QFile file(filePath);
	file.open(QIODevice::WriteOnly);

	QDataStream outStream(&file);
	outStream << VERSION;
	outStream << _feeds;
}
