#pragma once

#include <QWidget>
#include <QModelIndex>
#include <QNetworkAccessManager>
#include "ui_FeedDetailWidget.h"

class Core;

class QNetworkReply;

class FeedDetailWidget : public QWidget
{
	Q_OBJECT

public:
	FeedDetailWidget(QWidget *parent = Q_NULLPTR);
	~FeedDetailWidget();

	void setCore(Core* c);

public slots:
	void onFeedSelected(const QModelIndex& index);

private:
	Ui::FeedDetailWidget ui;

	Core* _core;
	int _feedIndex;

private slots:
	void on_refreshButton_clicked();

	void onFeedListUpdated();
};
