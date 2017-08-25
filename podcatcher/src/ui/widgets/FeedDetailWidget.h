#pragma once

#include <QWidget>
#include <QModelIndex>
#include <QNetworkAccessManager>
#include "ui_FeedDetailWidget.h"

class Core;
struct Feed;

class QNetworkReply;

class FeedDetailWidget : public QWidget
{
	Q_OBJECT

public:
	FeedDetailWidget(QWidget *parent = Q_NULLPTR);
	~FeedDetailWidget();

	void setCore(Core* c);

	void setFeed(Feed* f);

public slots:
	void onFeedSelected(const QModelIndex& index);

private:
	Ui::FeedDetailWidget ui;

	Core* _core;
	Feed* _feed;

private slots:
	void on_refreshButton_clicked();

	void onFeedListUpdated();
};
