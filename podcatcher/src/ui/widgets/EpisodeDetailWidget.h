#pragma once

#include <QWidget>
#include "ui_EpisodeDetailWidget.h"

#include <QModelIndex>

#include "core/Core.h"
#include "core/feeds/Feed.h"

#include "ui/models/EpisodeListModel.h"

class EpisodeCache;

class EpisodeDetailWidget : public QWidget
{
	Q_OBJECT

public:
	EpisodeDetailWidget(const EpisodeListModel& m, Core& core, const QModelIndex& idx, QWidget *parent = Q_NULLPTR);
	~EpisodeDetailWidget();

	void connectToCache();

	void refresh();

signals:
	void download(const QModelIndex& index);
	void play(const QModelIndex& index);
	
public slots:
	void onDownloadProgressUpdate(const Episode& e, qint64 progress);
	void onDownloadFinished(const Episode& e);

private:
	Ui::EpisodeDetailWidget ui;
	QModelIndex _index;

	const EpisodeListModel* _model;
	Episode* _episode;
	Core* _core;

	void _setDownloadButtonStatus();

private slots:
	void on_addToPlaylistButton_clicked();
	void on_downloadButton_clicked();
	void on_playButton_clicked();

	void onEpisodeChanged(const Episode* e);
};
