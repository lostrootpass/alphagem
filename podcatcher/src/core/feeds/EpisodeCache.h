#pragma once

#include <QNetworkReply>
#include <QObject>
#include <QUrl>

#include "Feed.h"

class QFile;
class QNetworkAccessManager;
class QNetworkReply;

class EpisodeCache : public QObject
{
	Q_OBJECT

public:
	EpisodeCache(QObject *parent);
	~EpisodeCache();

	static bool isDownloaded(const Episode* e);
	static QUrl getEpisodeUrl(const Episode* e);

	void downloadEpisode(const Episode& e);
	bool downloadInProgress() { return (_reply != nullptr); }

signals:
	void downloadComplete(const Episode& e);
	void downloadFailed(const Episode& e, QString error);
	void downloadProgressUpdated(const Episode& e, qint64 bytesDownloaded);

private slots:
	void _downloadFinished(QNetworkReply* reply);
	void _downloadProgressUpdated(qint64 done, qint64 total);
	void _onReadyRead();

private:
	QNetworkAccessManager* _mgr;
	QFile* _handle;
	QNetworkReply* _reply;

	const Episode* _currentDownload;

	QString _getTmpDownloadFilename();
};
