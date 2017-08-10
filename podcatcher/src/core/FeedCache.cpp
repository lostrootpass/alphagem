#include "FeedCache.h"

#include <QDataStream>
#include <QDir>
#include <QStandardPaths>

#include "FeedParser.h"

QDataStream& operator<<(QDataStream& stream, const Episode& episode)
{
	stream << episode.title;
	stream << episode.description;
	stream << episode.mediaUrl;
	stream << episode.imageUrl;
	stream << episode.guid;
	stream << episode.published;
	stream << episode.duration;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, Episode& episode)
{
	stream >> episode.title;
	stream >> episode.description;
	stream >> episode.mediaUrl;
	stream >> episode.imageUrl;
	stream >> episode.guid;
	stream >> episode.published;
	stream >> episode.duration;

	return stream;
}

QDataStream& operator<<(QDataStream& stream, const Feed& feed)
{
	stream << feed.title << feed.description << feed.imageUrl << feed.lastUpdated;
	stream << feed.episodes;
	return stream;
}

QDataStream& operator >> (QDataStream& stream, Feed& feed)
{
	stream >> feed.title;
	stream >> feed.description;
	stream >> feed.imageUrl;
	stream >> feed.lastUpdated;
	stream >> feed.episodes;

	return stream;
}

FeedCache::FeedCache(QObject *parent)
	: QObject(parent), _feedParser(nullptr)
{
}

FeedCache::~FeedCache()
{
}

void FeedCache::onFeedAdded(QString& url)
{
	if (!_feedParser)
	{
		_feedParser = new FeedParser(this);

		connect(_feedParser, &FeedParser::feedRetrieved, this, &FeedCache::onFeedRetrieved);
	}

	_feedParser->parseFromRemoteFile(url);
}

void FeedCache::onFeedRetrieved(Feed* feed)
{
	_feeds.push_back(*feed);

	saveToDisk();

	emit feedListUpdated();
}

void FeedCache::loadFromDisk()
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	QString filePath = dir.filePath("feeds.pod");

	QFile file(filePath);
	if(!file.exists())
		return;

	file.open(QIODevice::ReadOnly);
	QDataStream inStream(&file);
	inStream >> _feeds;

	emit feedListUpdated();
}

void FeedCache::saveToDisk()
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	if (!dir.exists())
		dir.mkdir(".");

	QString filePath = dir.filePath("feeds.pod");

	QFile file(filePath);
	file.open(QIODevice::WriteOnly);

	QDataStream outStream(&file);
	outStream << _feeds;
}
