#pragma once

#include <QObject>
#include <QMediaPlayer>

#include "core/Core.h"

class QMediaPlaylist;

struct Episode;

class AudioPlayer : public QObject
{
	Q_OBJECT

public:
	AudioPlayer(Core& core, QObject *parent);
	~AudioPlayer();

	inline const QMediaPlayer* getMediaPlayer() const { return _mediaPlayer; }

	void nextEpisode();

	void playEpisode(const Episode* episode);

public slots:
	void onPlayPauseToggle();

signals:
	void episodeChanged(const Episode* ep);
	void finished();
	void pauseStatusChanged(bool paused);

private:
	QMediaPlayer* _mediaPlayer;
	QMediaPlaylist* _playlist;
	Core* _core;

private slots:
	void onStateChange(QMediaPlayer::MediaStatus state);
};
