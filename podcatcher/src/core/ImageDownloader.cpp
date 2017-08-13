#include "ImageDownloader.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QNetworkReply>
#include <QStandardPaths>

ImageDownloader::ImageDownloader(QObject *parent)
	: QObject(parent), _reply(nullptr), _handle(nullptr)
{
	connect(&_mgr, &QNetworkAccessManager::finished,
		this, &ImageDownloader::onImageDownloaded);
}

ImageDownloader::~ImageDownloader()
{
	if (_reply)
	{
		_reply->abort();
		_reply->deleteLater();
	}

	if (_handle)
	{
		_handle->close();
		delete _handle;
	}
}

void ImageDownloader::setImage(QUrl url, QLabel& label)
{
	_label = &label;

	if (_reply)
	{
		_reply->abort();
		_reply->deleteLater();
	}

	QFile cached(_getCachedLocation(url));

	if (cached.exists())
	{
		QPixmap px;
		cached.open(QIODevice::ReadOnly);
		px.loadFromData(cached.readAll());
		cached.close();
		_setPixmap(px);
		return;
	}

	QNetworkRequest req(url);

	_handle = new QFile(_getDownloadLocation(url));

	if(_handle->exists())
	{
		QByteArray rangeHeader = "bytes=" + QByteArray::number(_handle->size()) + "-";
		req.setRawHeader("Range", rangeHeader);
	}


	_reply = _mgr.get(req);

	connect(_reply, &QNetworkReply::readyRead,
		this, &ImageDownloader::_readyRead);

	_handle->open(QIODevice::ReadWrite | QIODevice::Append);
}

QString ImageDownloader::_getCachedLocation(QUrl url)
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
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
	QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
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
	return QString(QCryptographicHash::hash(url.toString().toUtf8(), QCryptographicHash::Sha1).toHex());
}

void ImageDownloader::_readyRead()
{
	_handle->write(_reply->readAll());
}

void ImageDownloader::_setPixmap(QPixmap& pixmap)
{
	_label->setPixmap(pixmap.scaled(_label->size(), Qt::KeepAspectRatio));
}

void ImageDownloader::onImageDownloaded(QNetworkReply* reply)
{
	reply->disconnect(this);
	_reply = nullptr;

	if (reply->error())
	{
		reply->deleteLater();
		return;
	}

	QPixmap px;
	_handle->seek(0);
	px.loadFromData(_handle->readAll());
	_setPixmap(px);

	_handle->close();
	delete _handle;

	QFile::rename(_getDownloadLocation(reply->url()), _getCachedLocation(reply->url()));
	reply->deleteLater();
}