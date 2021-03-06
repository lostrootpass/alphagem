#pragma once

#include <QWidget>
#include "ui_PlaybackControlWidget.h"

struct Episode;

class AudioPlayer;
class Core;

class PlaybackControlWidget : public QWidget
{
	Q_OBJECT

public:
	PlaybackControlWidget(QWidget *parent = Q_NULLPTR);
	~PlaybackControlWidget();

	void setupConnections(Core* core);

signals:
	void episodeSelected(Episode* e);
	void muteToggled(bool muted);
	void pauseToggled();
	void volumeChanged(int volume);

public slots:
	/* Custom slots */
	void onDurationChanged(qint64 duration);
	void onEpisodeChanged(Episode* episode);
	void onPauseStatusChanged(bool paused);
	void onPlayerPositionChanged(qint64 milliseconds);

private:
	Ui::PlaybackControlWidget ui;

	AudioPlayer* _player;
	Core* _core;
	Episode* _episode;

	int _getJumpLen();
	void _updateMuteIcon(int volume);

private slots:
	/* Auto generated slots */
	void on_jumpBackButton_clicked();
	void on_jumpForwardButton_clicked();
	void on_muteButton_clicked();
	void on_nextEpisodeButton_clicked();
	void on_playPauseButton_clicked();
	void on_volumeSlider_valueChanged(int value);

	void onFinished();
	void onIconClicked();
	void onMutedChanged(bool muted);
	void onPlaylistUpdated();
	void onVolumeChanged(int volume);
};
