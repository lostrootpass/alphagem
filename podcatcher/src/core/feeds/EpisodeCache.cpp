#include "EpisodeCache.h"

#include <QDir>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QStandardPaths>

EpisodeCache::EpisodeCache(QObject *parent)
	: QObject(parent), _currentDownload(nullptr), _handle(nullptr), _reply(nullptr)
{
	_mgr = new QNetworkAccessManager(this);

	connect(_mgr, &QNetworkAccessManager::finished,
		this, &EpisodeCache::_downloadFinished);
}

EpisodeCache::~EpisodeCache()
{
	if (_handle)
	{
		_handle->close();
		delete _handle;
	}
}

bool EpisodeCache::isDownloaded(const Episode* e)
{
	QDir podcastDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	if (podcastDir.cd("podcasts"))
	{
		QStringList filter(e->guid + ".*");
		QStringList list = podcastDir.entryList(filter);

		return (list.size());
	}

	return false;
}

void EpisodeCache::downloadEpisode(const Episode& e)
{
	_currentDownload = &e;
	_handle = new QFile(_getTmpDownloadFilename());

	QNetworkRequest req(QUrl(e.mediaUrl));
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	if (_handle->exists())
	{
		QByteArray rangeHeader = "bytes=" + QByteArray::number(_handle->size()) + "-";
		req.setRawHeader("Range", rangeHeader);
	}

	_reply = _mgr->get(req);

	connect(_reply, &QNetworkReply::downloadProgress,
		this, &EpisodeCache::_downloadProgressUpdated);

	connect(_reply, &QNetworkReply::readyRead,
		this, &EpisodeCache::_onReadyRead);

	_handle->open(QIODevice::WriteOnly | QIODevice::Append);
}

QUrl EpisodeCache::getEpisodeUrl(const Episode* e)
{
	if (!e) return QUrl();

	QUrl url(e->mediaUrl);

	QDir podcastDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	if (podcastDir.cd("podcasts"))
	{
		QStringList filter(e->guid + ".*");
		QStringList list = podcastDir.entryList(filter);

		if (list.size())
			url = QUrl(podcastDir.absoluteFilePath(list[0]));
	}

	return url;
}

void EpisodeCache::_downloadFinished(QNetworkReply* reply)
{
	if (reply->error())
	{
		emit downloadFailed(*_currentDownload, reply->errorString());
	}
	else
	{
		QString ext = QFileInfo(reply->url().toString(QUrl::RemoveQuery)).completeSuffix();

		if (ext == "")
		{
			if(_currentDownload->mediaFormat == "audio/mpeg")
				ext = "mp3";
		}

		QDir podcastDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
		if (!podcastDir.cd("podcasts"))
		{
			podcastDir.mkdir("podcasts");
			podcastDir.cd("podcasts");
		}

		QString outName = QString("%2.%3").arg(_currentDownload->guid).arg(ext);
		
		_handle->close();
		_handle->rename(podcastDir.absoluteFilePath(outName));
		delete _handle;
		_handle = nullptr;

		emit downloadComplete(*_currentDownload);
	}

	reply->disconnect(this);
	reply->deleteLater();
	_reply = nullptr;

	_currentDownload = nullptr;
}

void EpisodeCache::_downloadProgressUpdated(qint64 done, qint64)
{
	emit downloadProgressUpdated(*_currentDownload, done);
}

void EpisodeCache::_onReadyRead()
{
	qint64 dataSize = _reply->bytesAvailable();

	if (dataSize > 0)
	{
		if(_handle)
			_handle->write(_reply->readAll());
	}
}

QString EpisodeCache::_getTmpDownloadFilename()
{
	QString ext = "";
	if (_currentDownload->mediaFormat == "audio/mpeg")
		ext = ".mp3";

	QDir podcastDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	if (!podcastDir.cd("podcasts/download"))
	{
		podcastDir.mkpath("podcasts/download");
		podcastDir.cd("podcasts/download");
	}

	return QString("%1%2").arg(podcastDir.absoluteFilePath(_currentDownload->guid)).arg(ext);
}
