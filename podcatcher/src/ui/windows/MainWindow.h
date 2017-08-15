#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class AudioPlayer;
class EpisodeCache;
class FeedCache;
class ImageDownloader;
struct Feed;
struct Episode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

	void setAudioPlayer(AudioPlayer* player);
	void setEpisodeCache(EpisodeCache* epCache);
	void setFeedCache(FeedCache* cache);
	void setImageDownloader(ImageDownloader* imageDownloader);

public slots:
	void onEpisodeSelected(const QModelIndex& index);

private:
	Ui::MainWindowClass ui;

	AudioPlayer* _audioPlayer;
	EpisodeCache* _epCache;
	FeedCache* _feedCache;
	ImageDownloader* _imageDownloader;

private slots:
	/* Auto generated slots */

	//File menu
	void on_actionAdd_Feed_triggered();
	void on_actionQuit_triggered();

	//Help menu
	void on_actionAbout_Qt_triggered();
	void on_actionAbout_triggered();

	//Toolbar
	void on_actionDownload_triggered();
	void on_actionHome_triggered();
	void on_actionRefresh_triggered();

	/* Custom slots */
	void onDownloadComplete(const Episode& e);
	void onDownloadFailed(const Episode& e, QString error);
	void onDownloadProgress(const Episode& e, qint64 bytesDownloaded);
	void onStatusBarUpdate(QString& text);
	void onFeedListUpdated();

	void onEpisodeHighlighted(const QModelIndex& index);

	void onFeedSelected(const QModelIndex& index);
};
