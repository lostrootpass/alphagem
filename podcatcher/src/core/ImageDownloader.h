#pragma once

#include <QObject>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QPixmap>

class QFile;
class QNetworkReply;

class ImageDownloader : public QObject
{
	Q_OBJECT

public:
	ImageDownloader(QObject *parent);
	~ImageDownloader();

	void setImage(QUrl url, QLabel& label);

private:
	QNetworkAccessManager _mgr;
	QNetworkReply* _reply;
	QLabel* _label;
	QFile* _handle;

	QString _getCachedLocation(QUrl url);
	QString _getDownloadLocation(QUrl url);
	QString _getDownloadName(QUrl url);

	void _readyRead();
	void _setPixmap(QPixmap& pixmap);

private slots:
	void onImageDownloaded(QNetworkReply* reply);
};
