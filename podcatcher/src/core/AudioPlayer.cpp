#include "AudioPlayer.h"

#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QStandardPaths>

#include "core/Settings.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"
#include "core/feeds/FeedSettings.h"
#include "core/feeds/EpisodeCache.h"

AudioPlayer::AudioPlayer(Core& core, QObject *parent)
	: QObject(parent), _core(&core), _playlist(nullptr), _current(nullptr),
	_skipAtPosition(-1)
{
	_mediaPlayer = new QMediaPlayer();
	connect(_mediaPlayer, &QMediaPlayer::mediaStatusChanged,
		this, &AudioPlayer::onStateChange);

	connect(_mediaPlayer, &QMediaPlayer::positionChanged,
		this, &AudioPlayer::onPositionChanged);

	connect(_mediaPlayer, &QMediaPlayer::durationChanged,
		this, &AudioPlayer::onDurationChanged);
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
	_mediaPlayer->play();

	const Feed* f = _core->feedCache()->feedForEpisode(episode);
	const FeedSettings& settings = _core->settings()->feed(f);
	if (settings.enableSkipIntroSting)
		_mediaPlayer->setPosition(settings.introStingLength * 1000);
	else
		_mediaPlayer->setPosition(0);

	_skipAtPosition = -1;

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

void AudioPlayer::onDurationChanged(qint64 duration)
{
	const Feed* f = _core->feedCache()->feedForEpisode(_current);
	const FeedSettings& settings = _core->settings()->feed(f);
	if (settings.enableSkipOutroSting)
		_skipAtPosition = duration - (settings.outroStingLength * 1000);
	else
		_skipAtPosition = -1;
}

void AudioPlayer::onPositionChanged(qint64 position)
{
	if (_skipAtPosition > -1 && position >= _skipAtPosition)
		_mediaPlayer->setPosition(_mediaPlayer->duration());
}

void AudioPlayer::onStateChange(QMediaPlayer::MediaStatus state)
{
	if (state == QMediaPlayer::MediaStatus::EndOfMedia)
	{
		Episode* justFinished = _core->defaultPlaylist()->popFront();

		//Need to close the file before trying to delete it on Windows.
		nextEpisode();

		const Feed* f = _core->feedCache()->feedForEpisode(justFinished);
		const FeedSettings& settings = _core->settings()->feed(f);
		if (settings.deleteAfterPlayback)
			_core->episodeCache()->deleteLocalFile(justFinished);
	}
}
