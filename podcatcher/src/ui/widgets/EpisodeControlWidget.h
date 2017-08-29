#pragma once

#include <QWidget>
#include "ui_EpisodeControlWidget.h"

#include "core/feeds/EpisodeCache.h"

class Core;
struct Episode;

class EpisodeControlWidget : public QWidget
{
	Q_OBJECT

public:
	EpisodeControlWidget(QWidget *parent = Q_NULLPTR);
	~EpisodeControlWidget();

	void init(Core* core);

	void update(Episode* e);

private:
	Ui::EpisodeControlWidget ui;

	Core* _core;
	Episode* _episode;
	QMenu* _menu;
	DownloadStatus _downloadStatus;

	void _updatePlayButtonStatus();
	void _updateDownloadButtonStatus();
	void _updateMoreDetailsButton();
	void _updatePlaylistButtonStatus();

private slots:
	void on_addToPlaylistButton_clicked();
	void on_downloadButton_clicked();
	void on_playButton_clicked();

	void onCacheStatusUpdated(const Episode* e);
	void onDownloadProgressUpdate(const Episode& e, qint64 progress);
	void onDownloadFinished(const Episode& e);
	void onEpisodeChanged(const Episode* e);

	/* More Details Menu*/
	void onFileDelete();
	void onMarkAsListened();
	void onMarkAsNew();
};
