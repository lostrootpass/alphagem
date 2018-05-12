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
	_skipAtPosition(-1), _resumePos(-1)
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
	{
		_current = nullptr;
		_playlist->clear();

		emit finished();
	}
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

	_playlist->addMedia(_core->episodeCache()->getEpisodeUrl(episode));
	_mediaPlayer->play();

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
	
	if (_resumePos != -1)
	{
		_mediaPlayer->setPosition(_resumePos);
		_resumePos = -1;
	}
	else if (settings.enableSkipIntroSting)
	{
		_mediaPlayer->setPosition(settings.introStingLength * 1000);
	}

	if (settings.enableSkipOutroSting)
		_skipAtPosition = duration - (settings.outroStingLength * 1000);
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
		Episode* justFinished = _core->defaultPlaylist()->front();
		
		if (justFinished == _current)
			_core->defaultPlaylist()->popFront();

		//Need to close the file before trying to delete it on Windows.

		//TODO: qt5.9 bug, remove this line after 5.10 upgrade
		_mediaPlayer->stop(); 
		
		nextEpisode();

		EpisodeCache* epCache = _core->episodeCache();
		Feed* f = _core->feedCache()->feedForEpisode(justFinished);
		const FeedSettings& settings = _core->settings()->feed(f);
		if (settings.deleteAfterPlayback)
			epCache->deleteLocalFile(justFinished);

		if (settings.autoContinueListening)
		{
			Episode* nextEp = nullptr;
			int index = f->episodes.indexOf(justFinished);

			if (settings.episodeOrder == EpisodeOrder::OldestFirst)
			{
				if (index < f->episodes.size() - 1)
					nextEp = f->episodes.at(index + 1);
			}
			else
			{
				if (index > 0)
					nextEp = f->episodes.at(index - 1);
			}

			
			if (nextEp && !nextEp->listened)
			{
				bool canQueue = true;

				if (settings.autoPlaylistRequiresDownload)
				{
					DownloadStatus status = epCache->downloadStatus(*nextEp);
					if (status != DownloadStatus::DownloadComplete)
					{
						canQueue = false;
					}
				}

				if (canQueue)
				{
					_core->defaultPlaylist()->add(nextEp);
					nextEpisode();
				}
			}
		}
	}
}
