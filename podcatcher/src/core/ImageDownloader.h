#pragma once

#include <QObject>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QPixmap>

class QFile;
class QNetworkReply;

struct ImageDownload : public QObject
{
	Q_OBJECT
public:
	ImageDownload(QString url) : QObject(nullptr), requestedUrl(url),
		label(nullptr), reply(nullptr), handle(nullptr)
	{}
	~ImageDownload();

	QString requestedUrl;
	QLabel* label;
	QNetworkReply* reply;
	QFile* handle;

public slots:
	void onReadyRead();
};

class ImageDownloader : public QObject
{
	Q_OBJECT

public:
	ImageDownloader(QObject *parent);
	~ImageDownloader();

	void getImage(QUrl url, QLabel* label = nullptr);

	void getImage(QUrl url, QPixmap* px);

	bool isCached(QUrl url);

signals:
	void imageDownloaded(QPixmap* px, QString url);

private:
	QNetworkAccessManager _mgr;
	QVector<ImageDownload*> _downloads;

	QHash<QUrl, QPixmap> _pixmapCache;

	QString _getCachedLocation(QUrl url);
	QString _getDownloadLocation(QUrl url);
	QString _getDownloadName(QUrl url);

	void _loadPixmap(QUrl url, QPixmap* px);

	void _setPixmap(QPixmap& pixmap, QLabel* label);

private slots:
	void onImageDownloaded(QNetworkReply* reply);
};
