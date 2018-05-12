#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QXmlStreamReader>

struct Feed;
struct Episode;

class QTimer;

class FeedParser : public QObject
{
	Q_OBJECT

public:
	FeedParser(QObject *parent);
	~FeedParser();

	void queueFeedDownload(const QString& url);

signals:
	void downloadFailed(QString& error);
	void feedRetrieved(Feed* name);
	void newEpisodeAdded(Feed* f, Episode* e);
	void updateProgress(int percent);
	void queueParsed();

private:
	QNetworkAccessManager _netMgr;
	QNetworkReply* _reply;
	QVector<QString> _parseQueue;

	QTimer* _timer;
	qint64 _bytesAtLastCheck;
	qint64 _total;

	void _parseOwnerData(QXmlStreamReader* xml, Feed* feed);
	void _parseChannelData(QXmlStreamReader* xml, Feed* feed);
	void _parseImageData(QXmlStreamReader*xml, Feed* feed);
	bool _parseItemData(QXmlStreamReader* xml, Episode* episode, QString untilGuid);


	void _parseFromRemoteFile(QString& url);
	void _parseNext();

	void _checkTimeout();

private slots:
	void _downloadFinished(QNetworkReply* reply);
	void _progressUpdated(qint64 received, qint64 total);
};
