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

	void playEpisode(Episode* episode);

	void pause();

	void setPosition(qint64 pos);

public slots:
	void onPlayPauseToggle();

signals:
	void episodeChanged(Episode* ep);
	void finished();
	void pauseStatusChanged(bool paused);

private:
	QMediaPlayer* _mediaPlayer;
	QMediaPlaylist* _playlist;
	Core* _core;
	Episode* _current;

	qint64 _skipAtPosition;

private slots:
	void onDurationChanged(qint64 duration);
	void onPositionChanged(qint64 position);
	void onStateChange(QMediaPlayer::MediaStatus state);
};
