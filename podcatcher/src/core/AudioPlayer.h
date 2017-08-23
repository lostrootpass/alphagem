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

	inline QMediaPlayer* getMediaPlayer() const { return _mediaPlayer; }

	const Episode* currentEpisode() const { return _current; }

	void nextEpisode();

	void playEpisode(const Episode* episode);

	void pause();

	void setPosition(qint64 pos);

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
	const Episode* _current;

private slots:
	void onStateChange(QMediaPlayer::MediaStatus state);
};
