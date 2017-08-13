#pragma once

#include <QWidget>
#include <QModelIndex>
#include <QNetworkAccessManager>
#include "ui_FeedDetailWidget.h"

#include "core/ImageDownloader.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

class QNetworkReply;

class FeedDetailWidget : public QWidget
{
	Q_OBJECT

public:
	FeedDetailWidget(QWidget *parent = Q_NULLPTR);
	~FeedDetailWidget();

	void setFeedCache(FeedCache& cache);
	void setImageDownloader(ImageDownloader& downloader);

public slots:
	void onFeedSelected(const QModelIndex& index);

private:
	Ui::FeedDetailWidget ui;

	FeedCache* _feedCache;
	ImageDownloader* _imageDownloader;
};
