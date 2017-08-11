#include "FeedParser.h"

#include <QXmlStreamReader>

#include "Feed.h"
#include "FeedCache.h"

FeedParser::FeedParser(QObject *parent)
	: QObject(parent), _reply(nullptr)
{
	connect(&_netMgr, &QNetworkAccessManager::finished, 
		this, &FeedParser::_downloadFinished);
}

FeedParser::~FeedParser()
{
}

void FeedParser::parseFromRemoteFile(QString& url)
{
	_reply = _netMgr.get(QNetworkRequest(QUrl(url)));

	connect(_reply, &QNetworkReply::downloadProgress, this, &FeedParser::_progressUpdated);
}

void FeedParser::_parseChannelData(QXmlStreamReader* xml, Feed* feed)
{
	QVector<Episode> newEpisodes;
	QString untilGuid = "";
	if (feed->episodes.size())
		untilGuid = feed->episodes.front().guid;

	while (!(xml->name() == "channel" && xml->isEndElement()))
	{
		xml->readNext();

		if (xml->isStartElement())
		{
			QStringRef n = xml->qualifiedName();

			if (n == "item")
			{
				Episode e;

				if (!_parseItemData(xml, &e, untilGuid))
				{
					break;
				}

				newEpisodes.push_front(e);
			}
			else if (n == "image")
			{
				_parseImageData(xml, feed);
			}
			else if (n == "title")
			{
				feed->title = xml->readElementText();
			}
			else if (n == "link")
			{
				feed->link = xml->readElementText();
			}
			else if (n == "description")
			{
				feed->description = xml->readElementText();
			}
			else if (n == "lastBuildDate")
			{
				feed->lastUpdated = QDate::fromString(xml->readElementText());
			}
		}
	}

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
bool FeedParser::_parseItemData(QXmlStreamReader* xml, Episode* episode, QString untilGuid)
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
			else if (n == "description")
			{
				episode->description = xml->readElementText();
			}
			else if (n == "pubDate")
			{
				episode->published = QDate::fromString(xml->readElementText());
			}
			else if (n == "itunes:duration")
			{
				episode->duration = xml->readElementText().toInt();
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
		}
	}

	if (episode->guid == untilGuid && untilGuid != "")
		return false;

	return true;
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
}

void FeedParser::_progressUpdated(qint64 received, qint64 total)
{
	if(total > 0)
		emit updateProgress((received / total) * 100);
}
