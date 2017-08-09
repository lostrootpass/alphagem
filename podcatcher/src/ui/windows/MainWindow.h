#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class QProgressBar;
class QMediaPlayer;
class QMediaPlaylist;

class FeedParser;
struct Feed;
struct Episode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

private:
	Ui::MainWindowClass ui;

	FeedParser* _feedParser;
	QProgressBar* _progressBar;

	QMediaPlayer* _mediaPlayer;
	QMediaPlaylist* _playlist;

	void _playEpisode(const Episode* episode);

private slots:
	/* Auto generated slots */

	//File menu
	void on_actionAdd_Feed_triggered();
	void on_actionQuit_triggered();

	//Help menu
	void on_actionAbout_Qt_triggered();
	void on_actionAbout_triggered();

	/* Custom slots */
	void onFeedAdded(QString& feed);
	void onStatusBarUpdate(QString& text);
	void onFeedRetrieved(Feed* feed);

	void onEpisodeSelected(const QModelIndex& index);
};
