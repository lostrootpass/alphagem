#pragma once

#include <QNetworkReply>
#include <QObject>
#include <QUrl>

#include "Feed.h"

class QFile;
class QNetworkAccessManager;
class QNetworkReply;

struct DownloadInfo : public QObject
{
	Q_OBJECT

public:
	~DownloadInfo();

	QNetworkReply* reply = nullptr;
	QFile* handle = nullptr;
	const Episode* episode;

public slots:
	void onReadyRead();
};

class EpisodeCache : public QObject
{
	Q_OBJECT

public:
	EpisodeCache(QObject *parent);
	~EpisodeCache();

	static bool isDownloaded(const Episode* e);
	static qint64 getPartialDownloadSize(const Episode* e);
	static QUrl getEpisodeUrl(const Episode* e);
	static QString getTmpDownloadFilename(const Episode* e);

	bool downloadInProgress() { return (_downloads.size()); }
	void downloadNext();

	void enqueueDownload(const Episode& e);

signals:
	void downloadComplete(const EpisodeCache* cache, const Episode& e);
	void downloadFailed(const Episode& e, QString error);
	void downloadProgressUpdated(const Episode& e, qint64 bytesDownloaded);

private slots:
	void _downloadFinished(QNetworkReply* reply);
	void _downloadProgressUpdated(qint64 done, qint64 total);
	
private:
	QList<DownloadInfo*> _downloads;
	QNetworkAccessManager* _mgr;
};
