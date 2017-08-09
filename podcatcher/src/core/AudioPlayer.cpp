#include "AudioPlayer.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>

#include "core/Feed.h"

AudioPlayer::AudioPlayer(QObject *parent)
	: QObject(parent), _playlist(nullptr)
{
	_mediaPlayer = new QMediaPlayer();
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::playEpisode(const Episode* episode)
{
	if (!_mediaPlayer)
	{
		_mediaPlayer = new QMediaPlayer();
	}
	else
	{
		_mediaPlayer->stop();
	}

	if (!_playlist)
	{
		_playlist = new QMediaPlaylist();
		_mediaPlayer->setPlaylist(_playlist);
	}
	else
	{
		_playlist->clear();
	}

	QUrl episodeLocation(episode->mediaUrl);

	//Todo: check to see if it has been saved locally

	_playlist->addMedia(episodeLocation);
	_mediaPlayer->play();

	emit episodeChanged(episode);
}

void AudioPlayer::onPlayPauseToggle()
{
	bool wasPlaying = (_mediaPlayer->state() == QMediaPlayer::State::PlayingState);
	if (wasPlaying)
		_mediaPlayer->pause();
	else
		_mediaPlayer->play();

	emit pauseStatusChanged(!wasPlaying);
}
