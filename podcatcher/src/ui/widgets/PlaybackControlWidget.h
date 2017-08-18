#pragma once

#include <QWidget>
#include "ui_PlaybackControlWidget.h"

struct Episode;

class AudioPlayer;

class PlaybackControlWidget : public QWidget
{
	Q_OBJECT

public:
	PlaybackControlWidget(QWidget *parent = Q_NULLPTR);
	~PlaybackControlWidget();

	void connectToAudioPlayer(AudioPlayer* player);

signals:
	void pauseToggled();

public slots:
	/* Custom slots */
	void onEpisodeChanged(const Episode* episode);
	void onPauseStatusChanged(bool paused);
	void onPlayerPositionChanged(qint64 milliseconds);

private:
	Ui::PlaybackControlWidget ui;

	AudioPlayer* _player;

private slots:
	/* Auto generated slots */
	void on_jumpBackButton_clicked();
	void on_jumpForwardButton_clicked();
	void on_playPauseButton_clicked();

	void onFinished();
};
