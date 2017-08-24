#include "AudioPlayer.h"

#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardPaths>

#include "core/feeds/Feed.h"
#include "core/feeds/EpisodeCache.h"

AudioPlayer::AudioPlayer(Core& core, QObject *parent)
	: QObject(parent), _core(&core), _playlist(nullptr), _current(nullptr)
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
		_current = p->front();
		
		playEpisode(_current);
	}
}

void AudioPlayer::playEpisode(Episode* episode)
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

	_current = episode;
	_current->setListened(true);
	emit episodeChanged(episode);
}

void AudioPlayer::pause()
{
	_mediaPlayer->pause();
	emit pauseStatusChanged(true);
}

void AudioPlayer::setPosition(qint64 pos)
{
	_mediaPlayer->setPosition(pos);
}

void AudioPlayer::onPlayPauseToggle()
{
	QMediaPlayer::State state = _mediaPlayer->state();
	bool wasPlaying = (state == QMediaPlayer::State::PlayingState);
	if (wasPlaying)
		_mediaPlayer->pause();
	else
		_mediaPlayer->play();

	emit pauseStatusChanged(wasPlaying);
}

void AudioPlayer::onStateChange(QMediaPlayer::MediaStatus state)
{
	if (state == QMediaPlayer::MediaStatus::EndOfMedia)
	{
		_core->defaultPlaylist()->popFront();
		nextEpisode();
	}
}
