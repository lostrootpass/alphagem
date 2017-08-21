#include "AudioPlayer.h"

#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardPaths>

#include "core/feeds/Feed.h"
#include "core/feeds/EpisodeCache.h"

AudioPlayer::AudioPlayer(Core& core, QObject *parent)
	: QObject(parent), _core(&core), _playlist(nullptr)
{
	_mediaPlayer = new QMediaPlayer();
	connect(_mediaPlayer, &QMediaPlayer::mediaStatusChanged,
		this, &AudioPlayer::onStateChange);
}

AudioPlayer::~AudioPlayer()
{
}

void AudioPlayer::nextEpisode()
{
	Playlist* p = _core->defaultPlaylist();

	if (!p->episodes.size())
		emit finished();
	else
	{
		Episode* e = p->popFront();

		playEpisode(e);
	}
}

void AudioPlayer::playEpisode(const Episode* episode)
{
	if (!_mediaPlayer)
	{
		_mediaPlayer = new QMediaPlayer();
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

	_playlist->addMedia(EpisodeCache::getEpisodeUrl(episode));
	_mediaPlayer->setPosition(0);
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

void AudioPlayer::onStateChange(QMediaPlayer::MediaStatus state)
{
	if (state == QMediaPlayer::MediaStatus::EndOfMedia)
	{
		nextEpisode();
	}
}
