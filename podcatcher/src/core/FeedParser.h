#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

struct Feed;

class FeedParser : public QObject
{
	Q_OBJECT

public:
	FeedParser(QObject *parent);
	~FeedParser();

	void parseFromRemoteFile(QString& url);
	void parseFromString(QByteArray fileData);

signals:
	void downloadFailed(QString& error);
	void updateProgress(int percent);
	void feedRetrieved(Feed* name);

private:
	QNetworkAccessManager _netMgr;
	QNetworkReply* _reply;

private slots:
	void _downloadFinished(QNetworkReply* reply);
	void _progressUpdated(qint64 received, qint64 total);
};
