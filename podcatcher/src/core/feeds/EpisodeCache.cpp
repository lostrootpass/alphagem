#include "EpisodeCache.h"

#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QStandardPaths>

#include "core/Core.h"
#include "core/Settings.h"
#include "core/Version.h"

#include "core/feeds/FeedCache.h"

QString sha1(QString s)
{
	QByteArray utf8 = s.toUtf8();
	QByteArray hash = QCryptographicHash::hash(utf8, QCryptographicHash::Sha1);
	return QString(hash.toHex());
}

DownloadInfo::~DownloadInfo()
{
	if (handle != nullptr)
	{
		handle->close();
		delete handle;
	}

	if (reply != nullptr)
	{
		reply->abort();
		reply->deleteLater();
	}
}

void DownloadInfo::onReadyRead()
{
	qint64 dataSize = reply->bytesAvailable();

	if (dataSize > 0)
	{
		if (handle)
			handle->write(reply->readAll());
	}
}


EpisodeCache::EpisodeCache(Core* core, QObject *parent)
	: QObject(parent), _core(core)
{
	_mgr = new QNetworkAccessManager(this);

	connect(_mgr, &QNetworkAccessManager::finished,
		this, &EpisodeCache::_downloadFinished);
}

EpisodeCache::~EpisodeCache()
{
}

bool EpisodeCache::isDownloaded(const Episode* e) const
{
	QDir dir(_core->settings()->saveDirectory());
	if (dir.exists())
	{
		QStringList filter(sha1(e->guid) + ".*");
		QStringList list = dir.entryList(filter);

		return (list.size());
	}

	return false;
}

void EpisodeCache::pauseCurrent()
{
	if (_downloads.size())
	{
		_downloads[0]->paused = true;

		_downloads[0]->handle->close();
		_downloads[0]->reply->disconnect(this);
		_downloads[0]->reply->abort();
		_downloads[0]->reply->deleteLater();
		_downloads[0]->reply = nullptr;

		emit downloadPaused();
	}
}

void EpisodeCache::resumeCurrent()
{
	downloadNext();
}

void EpisodeCache::updateAutoDownloadQueue(Feed* feed)
{
	const FeedSettings& settings = _core->settings()->feed(feed);

	if (!settings.autoDownloadNextEpisodes)
		return;

	int locallyStored = countStatus(feed, DownloadStatus::DownloadComplete);
	int inProgress = countStatus(feed, DownloadStatus::DownloadInQueue);
	inProgress += countStatus(feed, DownloadStatus::DownloadInProgress);
	inProgress += countStatus(feed, DownloadStatus::DownloadPaused);

	int remainingQuota = settings.maxSimultaneousDownloads;

	if (settings.enableLocalStorageLimit)
	{
		remainingQuota = settings.localEpisodeStorageLimit;
		remainingQuota -= (locallyStored + inProgress);

		if (remainingQuota > settings.maxSimultaneousDownloads)
			remainingQuota = settings.maxSimultaneousDownloads;
	}

	while (remainingQuota > 0)
	{
		Episode* e = _nextEpisode(feed);
		if (!e)
			return;

		enqueueDownload(e);
		--remainingQuota;
	}
}

qint64 EpisodeCache::getPartialDownloadSize(const Episode* e)
{
	QFile handle(getTmpDownloadFilename(e));

	if (handle.exists())
		return handle.size();

	return -1;
}

QString EpisodeCache::getCachedFilename(const Episode* e)
{
	QDir dir(_core->settings()->saveDirectory());
	if (dir.exists())
	{
		QStringList filter(sha1(e->guid) + ".*");
		QStringList list = dir.entryList(filter);

		if (list.size())
			return dir.absoluteFilePath(list[0]);
	}

	return QString();
}

DownloadStatus EpisodeCache::downloadStatus(const Episode& e) const
{
	for (const DownloadInfo* i : _downloads)
	{
		if (i->episode->guid == e.guid)
		{
			if ((i->handle->isOpen() || i == _downloads[0]) && !i->paused)
				return DownloadStatus::DownloadInProgress;
			else if (i->paused)
				return DownloadStatus::DownloadPaused;
			else
				return DownloadStatus::DownloadInQueue;
		}
	}

	if (isDownloaded(&e))
	{
		return DownloadStatus::DownloadComplete;
	}

	return DownloadStatus::DownloadNotInQueue;
}

void EpisodeCache::downloadNext()
{
	if (!_downloads.size()) return;

	DownloadInfo* info = _downloads.first();

	if (info->paused)
		info->paused = false;

	QNetworkRequest req(QUrl(info->episode->mediaUrl));
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	if (info->handle->exists())
	{
		qint64 size = info->handle->size();
		QByteArray rangeHeader = "bytes=" + QByteArray::number(size) + "-";
		req.setRawHeader("Range", rangeHeader);
	}

	info->reply = _mgr->get(req);

	connect(info->reply, &QNetworkReply::downloadProgress,
		this, &EpisodeCache::_downloadProgressUpdated);

	connect(info->reply, &QNetworkReply::readyRead,
		info, &DownloadInfo::onReadyRead);

	info->handle->open(QIODevice::WriteOnly | QIODevice::Append);
}

void EpisodeCache::enqueueDownload(Episode* e)
{
	for (const DownloadInfo* i : _downloads)
	{
		if (i->episode == e)
			return;
	}

	DownloadInfo* info = new DownloadInfo;

	info->episode = e;
	info->handle = new QFile(getTmpDownloadFilename(e));

	_downloads.push_back(info);

	emit downloadQueueUpdated();

	if (_downloads.size() == 1)
		downloadNext();
}

QUrl EpisodeCache::getEpisodeUrl(const Episode* e)
{
	if (!e) return QUrl();

	QUrl url(e->mediaUrl);

	QString cachedName = getCachedFilename(e);
	if (cachedName != "")
		url = QUrl(cachedName);

	return url;
}

QString EpisodeCache::getTmpDownloadFilename(const Episode* e)
{
	QString ext = "";
	
	if (e->mediaFormat == "audio/mpeg")
		ext = ".mp3";

	QDir dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	if (!dir.cd("podcasts/download"))
	{
		dir.mkpath("podcasts/download");
		dir.cd("podcasts/download");
	}

	//Hash the guid as not all guids use filename-friendly formats (e.g. URLs)
	return QString("%1%2").arg(dir.absoluteFilePath(sha1(e->guid))).arg(ext);
}

void EpisodeCache::cancelDownload(Episode* e)
{
	DownloadInfo* toRemove = nullptr;

	for (DownloadInfo* d : _downloads)
	{
		if (d->episode == e)
		{
			toRemove = d;
			break;
		}
	}

	if (toRemove)
	{
		_downloads.removeOne(toRemove);

		toRemove->handle->close();
		toRemove->handle->remove();
		toRemove->handle = nullptr;

		delete toRemove;
		
		emit downloadQueueUpdated();
	}
}

int EpisodeCache::countStatus(Feed* feed, DownloadStatus status) const
{
	int count = 0;

	for (const Episode* e : feed->episodes)
	{
		if (downloadStatus(*e) == status)
			++count;
	}

	return count;
}

void EpisodeCache::deleteLocalFile(const Episode* e)
{
	QString cachedName = getCachedFilename(e);

	if (cachedName == "")
		return;

	QFile file(cachedName);
	file.remove();

	emit cacheStatusUpdated(e);
}

void EpisodeCache::_downloadFinished(QNetworkReply* reply)
{
	if (!_downloads.size())
		return;

	DownloadInfo* info = _downloads.first();

	if (info->paused || info->reply != reply)
		return;

	Episode* e = info->episode;

	if (reply->error())
	{
		emit downloadFailed(*e, reply->errorString());
		emit downloadQueueUpdated();
	}
	else
	{
		QString baseUrl = reply->url().toString(QUrl::RemoveQuery);
		QString ext = QFileInfo(baseUrl).suffix();

		if (ext == "")
		{
			if(e->mediaFormat == "audio/mpeg")
				ext = "mp3";
		}

		QDir d = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
		if (!d.cd("podcasts"))
		{
			d.mkdir("podcasts");
			d.cd("podcasts");
		}

		QString outName = QString("%2.%3").arg(sha1(e->guid)).arg(ext);
		
		info->handle->close();
		info->handle->rename(d.absoluteFilePath(outName));

		emit downloadComplete(*e);
		emit downloadQueueUpdated();

		updateAutoDownloadQueue(_core->feedCache()->feedForEpisode(e));
	}

	info->reply->disconnect(this);
	delete info;

	_downloads.pop_front();

	downloadNext();
}

void EpisodeCache::_downloadProgressUpdated(qint64 done, qint64)
{
	if(_downloads.size())
		emit downloadProgressUpdated(*_downloads.first()->episode, done);
}

Episode* EpisodeCache::_nextEpisode(Feed* feed)
{
	const FeedSettings& settings = _core->settings()->feed(feed);

	if (settings.episodeOrder == EpisodeOrder::OldestFirst)
	{
		QVector<Episode*>::iterator it = feed->episodes.begin();
		QVector<Episode*>::iterator end = feed->episodes.end();

		return _nextEpisodeForDownload(it, end);

	}
	else
	{
		QVector<Episode*>::reverse_iterator it = feed->episodes.rbegin();
		QVector<Episode*>::reverse_iterator end = feed->episodes.rend();

		return _nextEpisodeForDownload(it, end);
	}
}
