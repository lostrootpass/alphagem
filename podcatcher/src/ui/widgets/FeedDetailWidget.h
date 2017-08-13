#pragma once

#include <QWidget>
#include <QModelIndex>
#include "ui_FeedDetailWidget.h"

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"


class FeedDetailWidget : public QWidget
{
	Q_OBJECT

public:
	FeedDetailWidget(QWidget *parent = Q_NULLPTR);
	~FeedDetailWidget();

	void setFeedCache(FeedCache& cache);

public slots:
	void onFeedSelected(const QModelIndex& index);

private:
	Ui::FeedDetailWidget ui;

	FeedCache* _feedCache;
};
