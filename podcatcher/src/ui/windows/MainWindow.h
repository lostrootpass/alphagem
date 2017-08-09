#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class QProgressBar;

class AudioPlayer;
class FeedParser;
struct Feed;
struct Episode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = Q_NULLPTR);
	~MainWindow();

	void setAudioPlayer(AudioPlayer* player);

private:
	Ui::MainWindowClass ui;

	AudioPlayer* _audioPlayer;
	FeedParser* _feedParser;
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
	void onFeedAdded(QString& feed);
	void onStatusBarUpdate(QString& text);
	void onFeedRetrieved(Feed* feed);

	void onEpisodeSelected(const QModelIndex& index);
};
