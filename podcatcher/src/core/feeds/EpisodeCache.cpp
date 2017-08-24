#include "EpisodeCache.h"

#include <QDataStream>
#include <QDir>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QStandardPaths>

#include "core/Core.h"
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


EpisodeCache::EpisodeCache(QObject *parent)
	: QObject(parent)
{
	_mgr = new QNetworkAccessManager(this);

	connect(_mgr, &QNetworkAccessManager::finished,
		this, &EpisodeCache::_downloadFinished);
}

EpisodeCache::~EpisodeCache()
{
}

bool EpisodeCache::isDownloaded(const Episode* e)
{
	QDir dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	if (dir.cd("podcasts"))
	{
		QStringList filter(sha1(e->guid) + ".*");
		QStringList list = dir.entryList(filter);

		return (list.size());
	}

	return false;
}

qint64 EpisodeCache::getPartialDownloadSize(const Episode* e)
{
	QFile handle(getTmpDownloadFilename(e));

	if (handle.exists())
		return handle.size();

	return -1;
}

DownloadStatus EpisodeCache::downloadStatus(const Episode& e) const
{
	for (const DownloadInfo* i : _downloads)
	{
		if (i->episode->guid == e.guid)
		{
			if (i->handle->isOpen())
				return DownloadStatus::DownloadInProgress;
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

	if (_downloads.size() == 1)
		downloadNext();
}

QUrl EpisodeCache::getEpisodeUrl(const Episode* e)
{
	if (!e) return QUrl();

	QUrl url(e->mediaUrl);

	QDir dir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	if (dir.cd("podcasts"))
	{
		QStringList filter(sha1(e->guid) + ".*");
		QStringList list = dir.entryList(filter);

		if (list.size())
			url = QUrl(dir.absoluteFilePath(list[0]));
	}

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

void EpisodeCache::_downloadFinished(QNetworkReply* reply)
{
	DownloadInfo* info = _downloads.first();
	const Episode* e = info->episode;

	if (reply->error())
	{
		emit downloadFailed(*e, reply->errorString());
	}
	else
	{
		QString baseUrl = reply->url().toString(QUrl::RemoveQuery);
		QString ext = QFileInfo(baseUrl).completeSuffix();

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
	}

	info->reply->disconnect(this);
	delete info;

	_downloads.pop_front();

	downloadNext();
}

void EpisodeCache::_downloadProgressUpdated(qint64 done, qint64)
{
	emit downloadProgressUpdated(*_downloads.first()->episode, done);
}
