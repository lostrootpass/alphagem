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

	bool isCached(QUrl url);

	void loadPixmap(QUrl url, QPixmap* px);

signals:
	void imageDownloaded(QPixmap* px, QString url);

private:
	QNetworkAccessManager _mgr;
	QVector<ImageDownload*> _downloads;

	QString _getCachedLocation(QUrl url);
	QString _getDownloadLocation(QUrl url);
	QString _getDownloadName(QUrl url);

	void _setPixmap(QPixmap& pixmap, QLabel* label);

private slots:
	void onImageDownloaded(QNetworkReply* reply);
};
