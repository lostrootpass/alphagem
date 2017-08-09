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

void FeedParser::parseFromString(QByteArray fileData)
{

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

		QString currElement = "";
		while (!xml.atEnd())
		{
			xml.readNext();

			if (xml.isStartElement())
			{
				if (currElement == "channel" && xml.name() == "title")
				{
					feed.title = xml.readElementText();
				}
				else if (currElement == "item" && xml.name() == "title")
				{
					feed.episodes.push_back(xml.readElementText());
				}

				currElement = xml.name().toString();
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
