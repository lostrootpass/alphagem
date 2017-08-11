#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>

struct Feed;
struct Episode;

class FeedParser : public QObject
{
	Q_OBJECT

public:
	FeedParser(QObject *parent);
	~FeedParser();

	void parseFromRemoteFile(QString& url);

signals:
	void downloadFailed(QString& error);
	void updateProgress(int percent);
	void feedRetrieved(Feed* name);

private:
	QNetworkAccessManager _netMgr;
	QNetworkReply* _reply;

	void _parseChannelData(QXmlStreamReader* xml, Feed* feed);
	void _parseImageData(QXmlStreamReader*xml, Feed* feed);
	bool _parseItemData(QXmlStreamReader* xml, Episode* episode, QString untilGuid);

private slots:
	void _downloadFinished(QNetworkReply* reply);
	void _progressUpdated(qint64 received, qint64 total);
};
