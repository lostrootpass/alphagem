#include "FeedParser.h"
#include "Feed.h"

#include <QXmlStreamReader>

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
	while (!(xml->name() == "channel" && xml->isEndElement()))
	{
		xml->readNext();

		if (xml->isStartElement())
		{
			QStringRef n = xml->qualifiedName();

			if (n == "item")
			{
				_parseItemData(xml, feed);
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

void FeedParser::_parseItemData(QXmlStreamReader* xml, Feed* feed)
{
	Episode e;

	while (!(xml->name() == "item" && xml->isEndElement()))
	{
		xml->readNext();

		if (xml->isStartElement())
		{
			QStringRef n = xml->qualifiedName();
			if (n == "title")
			{
				e.title = xml->readElementText();
			}
			else if (n == "description")
			{
				e.description = xml->readElementText();
			}
			else if (n == "pubDate")
			{
				e.published = QDate::fromString(xml->readElementText());
			}
			else if (n == "itunes:duration")
			{
				e.duration = xml->readElementText().toInt();
			}
			else if (n == "enclosure")
			{
				e.mediaUrl = xml->attributes().value("url").toString();
			}
			else if (n == "itunes:image")
			{
				e.imageUrl = xml->attributes().value("href").toString();
			}
		}
	}

	feed->episodes.push_back(e);
}

void FeedParser::_downloadFinished(QNetworkReply* reply)
{
	if (reply->error())
	{
		emit downloadFailed(reply->errorString());
	}
	else
	{
		QXmlStreamReader xml(reply->readAll());
		Feed feed;

		while (!xml.atEnd())
		{
			xml.readNext();

			if (xml.isStartElement())
			{
				if (xml.name() == "channel")
				{
					_parseChannelData(&xml, &feed);
				}
			}
		}

		emit feedRetrieved(&feed);
	}
	
	reply->disconnect(this);
	reply->deleteLater();
}

void FeedParser::_progressUpdated(qint64 received, qint64 total)
{
	if(total > 0)
		emit updateProgress((received / total) * 100);
}
