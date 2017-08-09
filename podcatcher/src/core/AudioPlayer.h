#pragma once

#include <QObject>
#include <QMediaPlayer>

class QMediaPlaylist;

struct Episode;

class AudioPlayer : public QObject
{
	Q_OBJECT

public:
	AudioPlayer(QObject *parent);
	~AudioPlayer();

	inline const QMediaPlayer* getMediaPlayer() const { return _mediaPlayer; }

	void playEpisode(const Episode* episode);

public slots:
	void onPlayPauseToggle();

signals:
	void episodeChanged(const Episode* ep);
	void pauseStatusChanged(bool paused);

private:
	QMediaPlayer* _mediaPlayer;
	QMediaPlaylist* _playlist;
};
