#pragma once

#include <QWidget>
#include "ui_EpisodeDetailWidget.h"

#include <QModelIndex>

#include "ui/models/EpisodeListModel.h"

class EpisodeCache;

class EpisodeDetailWidget : public QWidget
{
	Q_OBJECT

public:
	EpisodeDetailWidget(const EpisodeListModel& m, const QModelIndex& idx, QWidget *parent = Q_NULLPTR);
	~EpisodeDetailWidget();

	void connectToCache(EpisodeCache* cache);

	void refresh();

signals:
	void download(const QModelIndex& index);
	void play(const QModelIndex& index);
	
public slots:
	void onDownloadProgressUpdate(const Episode& e, qint64 progress);
	void onDownloadFinished(const EpisodeCache*, const Episode& e);

private:
	Ui::EpisodeDetailWidget ui;

	QModelIndex _index;
	const EpisodeListModel* _model;

	void _setDownloadButtonStatus(const Episode* e);

private slots:
	void on_downloadButton_clicked();
	void on_playButton_clicked();
};
