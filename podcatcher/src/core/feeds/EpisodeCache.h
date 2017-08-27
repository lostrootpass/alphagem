#pragma once

#include <QNetworkReply>
#include <QObject>
#include <QUrl>

#include "Feed.h"

class QFile;
class QNetworkAccessManager;
class QNetworkReply;

class Core;

enum class DownloadStatus
{
	/* Download has completed, file is stored locally */
	DownloadComplete,

	/* Download of this particular file is in progress right now */
	DownloadInProgress,

	/* The file is in the queue waiting to be downloaded */
	DownloadInQueue,

	/* The file is not cached locally, downloading, or pending */
	DownloadNotInQueue
};

struct DownloadInfo : public QObject
{
	Q_OBJECT

public:
	~DownloadInfo();

	QNetworkReply* reply = nullptr;
	QFile* handle = nullptr;
	Episode* episode;

public slots:
	void onReadyRead();
};

class EpisodeCache : public QObject
{
	Q_OBJECT

public:
	EpisodeCache(Core* core, QObject *parent = nullptr);
	~EpisodeCache();

	void cancelDownload(Episode* e);

	void deleteLocalFile(const Episode* e);

	bool downloadInProgress() { return (_downloads.size()); }
	const QList<DownloadInfo*>& downloadList() { return _downloads; }
	DownloadStatus downloadStatus(const Episode& e) const;
	void downloadNext();

	void enqueueDownload(Episode* e);

	QString getCachedFilename(const Episode* e);
	QUrl getEpisodeUrl(const Episode* e);
	qint64 getPartialDownloadSize(const Episode* e);
	QString getTmpDownloadFilename(const Episode* e);

	bool isDownloaded(const Episode* e) const;

signals:
	void cacheStatusUpdated(const Episode* e);
	void downloadComplete(Episode& e);
	void downloadFailed(const Episode& e, QString error);
	void downloadProgressUpdated(const Episode& e, qint64 bytesDownloaded);
	void downloadQueueUpdated();

private slots:
	void _downloadFinished(QNetworkReply* reply);
	void _downloadProgressUpdated(qint64 done, qint64 total);
	
private:
	QList<DownloadInfo*> _downloads;
	QNetworkAccessManager* _mgr;

	Core* _core;
};
