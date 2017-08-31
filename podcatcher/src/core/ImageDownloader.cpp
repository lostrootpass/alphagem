#include "ImageDownloader.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QStandardPaths>


ImageDownload::~ImageDownload()
{
	if (handle)
	{
		handle->close();
		delete handle;
	}

	if (reply)
	{
		reply->abort();
		reply->deleteLater();
	}
}

void ImageDownload::onReadyRead()
{
	handle->write(reply->readAll());
}

ImageDownloader::ImageDownloader(QObject *parent)
	: QObject(parent)
{
	connect(&_mgr, &QNetworkAccessManager::finished,
		this, &ImageDownloader::onImageDownloaded);
}

ImageDownloader::~ImageDownloader()
{
}

void ImageDownloader::getImage(QUrl url, QLabel* label)
{
	QFile cached(_getCachedLocation(url));

	if (cached.exists())
	{
		QString n = cached.fileName();
		QPixmap px;
		px.load(n, nullptr, Qt::ImageConversionFlag::DiffuseAlphaDither);
		
		if(label) 
			_setPixmap(px, label);

		return;
	}

	for (const ImageDownload* d : _downloads)
	{
		if (d->requestedUrl == url.toString())
			return;
	}

	ImageDownload* download = new ImageDownload(url.toString());
	_downloads.push_back(download);

	download->label = label;

	QNetworkRequest req(url);
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	download->handle = new QFile(_getDownloadLocation(url));

	if(download->handle->exists())
	{
		QByteArray rangeHeader = 
			"bytes=" + QByteArray::number(download->handle->size()) + "-";
		req.setRawHeader("Range", rangeHeader);
	}

	download->reply = _mgr.get(req);

	connect(download->reply, &QNetworkReply::readyRead,
		download, &ImageDownload::onReadyRead);

	download->handle->open(QIODevice::ReadWrite | QIODevice::Append);
}

void ImageDownloader::getImage(QUrl url, QPixmap* px)
{
	if (_pixmapCache.contains(url))
	{
		*px = _pixmapCache[url];
	}
	else
	{
		_loadPixmap(url, px);
		_pixmapCache.insert(url, *px);
	}
}

bool ImageDownloader::isCached(QUrl url)
{
	QFile cached(_getCachedLocation(url));

	return cached.exists();
}

void ImageDownloader::_loadPixmap(QUrl url, QPixmap* px)
{
	QFile cached(_getCachedLocation(url));

	if (cached.exists())
	{
		QString n = cached.fileName();
		px->load(n, nullptr, Qt::ImageConversionFlag::DiffuseAlphaDither);

		return;
	}
}

QString ImageDownloader::_getCachedLocation(QUrl url)
{
	QString path = 
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	if (!dir.cd("images"))
	{
		dir.mkpath("images");
		dir.cd("images");
	}

	return dir.filePath(_getDownloadName(url));
}

QString ImageDownloader::_getDownloadLocation(QUrl url)
{
	QString path = 
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	if (!dir.cd("images/download"))
	{
		dir.mkpath("images/download");
		dir.cd("images/download");
	}

	return dir.filePath(_getDownloadName(url));
}

QString ImageDownloader::_getDownloadName(QUrl url)
{
	QByteArray utf8 = url.toString().toUtf8();
	QByteArray hash = QCryptographicHash::hash(utf8, QCryptographicHash::Sha1);
	return QString(hash.toHex());
}

void ImageDownloader::_setPixmap(QPixmap& pixmap, QLabel* label)
{
	label->setPixmap(pixmap.scaled(label->size(), Qt::KeepAspectRatio,
		Qt::SmoothTransformation));
}

void ImageDownloader::onImageDownloaded(QNetworkReply* reply)
{
	int i;
	for (i = 0; i < _downloads.size(); ++i)
	{
		if (_downloads[i]->reply == reply)
		{
			break;
		}
	}

	ImageDownload* download = _downloads[i];
	
	reply->disconnect(this);

	if (reply->error())
	{
		reply->deleteLater();
		_downloads.remove(i);

		return;
	}

	QPixmap px;
	download->handle->seek(0);
	px.loadFromData(download->handle->readAll());
	if (download->label)
	{
		_setPixmap(px, download->label);
	}
	else
	{
		emit imageDownloaded(&px, download->requestedUrl);
	}

	download->handle->close();

	QString dl = _getDownloadLocation(download->requestedUrl);
	QString cl = _getCachedLocation(download->requestedUrl);
	QFile::rename(dl, cl);
	
	reply->deleteLater();
	delete _downloads[i];
	_downloads.remove(i);
}
