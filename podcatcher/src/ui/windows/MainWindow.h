#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class QProgressBar;

class AudioPlayer;
class FeedCache;
struct Feed;
struct Episode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

	void setAudioPlayer(AudioPlayer* player);
	void setFeedCache(FeedCache* cache);

private:
	Ui::MainWindowClass ui;

	AudioPlayer* _audioPlayer;
	FeedCache* _feedCache;
	QProgressBar* _progressBar;

private slots:
	/* Auto generated slots */

	//File menu
	void on_actionAdd_Feed_triggered();
	void on_actionQuit_triggered();

	//Help menu
	void on_actionAbout_Qt_triggered();
	void on_actionAbout_triggered();

	/* Custom slots */
	void onStatusBarUpdate(QString& text);
	void onFeedListUpdated();

	void onEpisodeSelected(const QModelIndex& index);
};
