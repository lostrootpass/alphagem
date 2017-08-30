#include "FeedParser.h"

#include <QDebug>
#include <QXmlStreamReader>

#include "Feed.h"
#include "FeedCache.h"

#include <sstream>
#include <iomanip>

std::time_t toTime(QString s)
{
	struct tm tm = { 0 };
	std::istringstream str(s.toStdString());
	str >> std::get_time(&tm, "%a, %d %b %Y %H:%M:%S %Z%z");
	return mktime(&tm);
}

FeedParser::FeedParser(QObject *parent)
	: QObject(parent), _reply(nullptr)
{
	connect(&_netMgr, &QNetworkAccessManager::finished, 
		this, &FeedParser::_downloadFinished);
}

FeedParser::~FeedParser()
{
}

void FeedParser::queueFeedDownload(const QString& url)
{
	_parseQueue.push_back(url);
	_parseNext();
}

void FeedParser::_parseOwnerData(QXmlStreamReader* xml, Feed* feed)
{
	while (!(xml->qualifiedName() == "itunes:owner" && xml->isEndElement()))
	{
		xml->readNext();

		if (xml->isStartElement())
		{
			QStringRef n = xml->name();

			if (n == "name")
			{
				feed->ownerName = xml->readElementText();
			}
			else if (n == "email")
			{
				feed->ownerEmail = xml->readElementText();
			}
		}
	}
}

void FeedParser::_parseChannelData(QXmlStreamReader* xml, Feed* feed)
{
	QVector<Episode*> newEpisodes;
	QString untilGuid = "";
	if (feed->episodes.size())
		untilGuid = feed->episodes.back()->guid;

	while (!(xml->name() == "channel" && xml->isEndElement()))
	{
		xml->readNext();

		if (xml->isStartElement())
		{
			QStringRef n = xml->qualifiedName();

			if (n == "item")
			{
				Episode* e = new Episode();
				e->listened = false;

				if (_parseItemData(xml, e, untilGuid))
				{
					emit newEpisodeAdded(feed, e);
				}
				else
				{
					break;
				}

				newEpisodes.push_front(e);
			}
			else if (n == "image")
			{
				_parseImageData(xml, feed);
			}
			else if (n == "itunes:image")
			{
				feed->imageUrl = xml->attributes().value("href").toString();
			}
			else if (n == "title")
			{
				feed->title = xml->readElementText();
			}
			else if (n == "link")
			{
				feed->link = xml->readElementText();
			}
			else if (n == "description" || n == "itunes:summary")
			{
				feed->description = xml->readElementText();
			}
			else if (n == "lastBuildDate")
			{
				feed->lastUpdated = xml->readElementText();
			}
			else if (n == "itunes:explicit")
			{
				feed->isExplicit = (xml->readElementText() == "yes");
			}
			else if (n == "category")
			{
				QString cat = xml->readElementText();
				if (!feed->categories.contains(cat))
					feed->categories.push_back(cat);
			}
			else if (n == "itunes:category")
			{
				QString cat = xml->attributes().value("text").toString();
				if (!feed->categories.contains(cat))
					feed->categories.push_back(cat);
			}
			else if (n == "itunes:owner")
			{
				_parseOwnerData(xml, feed);
			}
			else if (n == "itunes:author")
			{
				feed->creator = xml->readElementText();
			}
			else if (n == "media:credit")
			{
				if (xml->attributes().value("role") == "author")
				{
					feed->creator = xml->readElementText();
				}
			}
		}
	}

	qDebug() << newEpisodes.size() << " eps retrieved for " << feed->title;
	feed->episodes.append(newEpisodes);
}

void FeedParser::_parseImageData(QXmlStreamReader*xml, Feed* feed)
{
	while (!(xml->name() == "image" && xml->isEndElement()))
	{
		xml->readNext();

		if (xml->isStartElement())
		{
			if (xml->name() == "url")
			{
				feed->imageUrl = xml->readElementText();
			}
		}
	}
}

//Parse an item element and turn it in to an episode.
//Returns: true if the episode was added, false if episode guid was untilGuid
bool FeedParser::_parseItemData(QXmlStreamReader* xml, Episode* episode,
	QString untilGuid)
{
	while (!(xml->name() == "item" && xml->isEndElement()))
	{
		xml->readNext();

		if (xml->isStartElement())
		{
			QStringRef n = xml->qualifiedName();
			if (n == "title")
			{
				episode->title = xml->readElementText();
			}
			else if (n == "description" || n == "itunes:summary")
			{
				episode->description = xml->readElementText();
			}
			else if (n == "pubDate")
			{
				episode->published = xml->readElementText();
			}
			else if (n == "itunes:duration")
			{
				int el1, el2, el3;
				int numElements = sscanf_s(xml->readElementText().toUtf8(),
					"%d:%d:%d", &el1, &el2, &el3);

				switch (numElements)
				{
				case 1:
					//SS
					episode->duration = (el1);
					break;
				case 2:
					//MM:SS
					episode->duration = (el1 * 60) + (el2);
					break;

				case 3:
					//HH:MM:SS
					episode->duration = (el1 * 60 * 60) + (el2 * 60) + (el3);
					break;
				default:
					//This should never happen, but you never know.
					break;
				}
			}
			else if (n == "enclosure")
			{
				episode->mediaUrl = xml->attributes().value("url").toString();
				episode->mediaFormat = xml->attributes().value("type").toString();
			}
			else if (n == "itunes:image")
			{
				episode->imageUrl = xml->attributes().value("href").toString();
			}
			else if (n == "guid")
			{
				episode->guid = xml->readElementText();
			}
			else if (n == "itunes:explicit")
			{
				episode->isExplicit = (xml->readElementText() == "yes");
			}
			else if (n == "category")
			{
				QString cat = xml->readElementText();
				if(!episode->categories.contains(cat))
					episode->categories.push_back(cat);
			}
			else if (n == "itunes:category")
			{
				QString cat = xml->attributes().value("text").toString();
				if (!episode->categories.contains(cat))
					episode->categories.push_back(cat);
			}
			else if (n == "link")
			{
				episode->shareLink = xml->readElementText();
			}
			else if (n == "itunes:author")
			{
				episode->author = xml->readElementText();
			}
			else if (n == "media:credit")
			{
				if (xml->attributes().value("role") == "author")
					episode->author = xml->readElementText();
			}
		}
	}

	if (episode->guid == untilGuid && untilGuid != "")
		return false;

	return true;
}

void FeedParser::_parseFromRemoteFile(QString& url)
{
	_reply = _netMgr.get(QNetworkRequest(QUrl(url)));

	connect(_reply, &QNetworkReply::downloadProgress,
		this, &FeedParser::_progressUpdated);
}

void FeedParser::_parseNext()
{
	if (_reply)
		return;

	if (!_parseQueue.size())
	{
		emit queueParsed();
		return;
	}
	
	QString next = _parseQueue.takeFirst();
	_parseFromRemoteFile(next);
}

void FeedParser::_downloadFinished(QNetworkReply* reply)
{
	if (reply->error())
	{
		QString error = reply->errorString();
		emit downloadFailed(error);
	}
	else
	{
		QXmlStreamReader xml(reply->readAll());

		QString feedUrl = reply->url().toString();
		FeedCache* cache = (FeedCache*)parent();
		Feed* feed = cache->feedForUrl(feedUrl);

		feed->lastRefreshTimestamp = QDateTime::currentSecsSinceEpoch();

		while (!xml.atEnd())
		{
			xml.readNext();

			if (xml.isStartElement())
			{
				if (xml.name() == "channel")
				{
					_parseChannelData(&xml, feed);
				}
			}
		}

		emit feedRetrieved(feed);
	}
	
	reply->disconnect(this);
	reply->deleteLater();

	_reply = nullptr;
	_parseNext();
}

void FeedParser::_progressUpdated(qint64 received, qint64 total)
{
	if(total > 0)
		emit updateProgress((received / total) * 100);
}
