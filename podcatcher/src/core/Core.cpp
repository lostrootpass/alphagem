#include "Core.h"

#include <QtWidgets/QApplication>

#include "AudioPlayer.h"
#include "EpisodeCache.h"
#include "Feed.h"
#include "FeedCache.h"
#include "ImageDownloader.h"
#include "Notifier.h"
#include "Settings.h"
#include "State.h"

Core::Core() : QObject(nullptr), _audioPlayer(nullptr), _episodeCache(nullptr),
	_feedCache(nullptr), _imageDownloader(nullptr)
{

}

Core::~Core()
{
	delete _audioPlayer;
	delete _episodeCache;
	delete _feedCache;
	delete _imageDownloader;
	delete _notifier;
	delete _settings;
	delete _state;

	for (Playlist* p : _playlists)
	{
		delete p;
	}

	_playlists.clear();
}

void Core::init(QApplication* app)
{
	_settings = new Settings(this);
	connect(app, &QApplication::aboutToQuit,
		_settings, &Settings::onAboutToQuit);

	_audioPlayer = new AudioPlayer(*this, nullptr);
	
	_feedCache = new FeedCache(this);
	connect(app, &QApplication::aboutToQuit,
		_feedCache, &FeedCache::onAboutToQuit);
	_feedCache->startRefreshTimer();

	_notifier = new Notifier(this);
	connect(_audioPlayer, &AudioPlayer::episodeChanged,
		_notifier, &Notifier::episodeStarted);
	connect(_feedCache, &FeedCache::queueParsed,
		_notifier, &Notifier::flushNotifications);
	connect(_feedCache, &FeedCache::newEpisodeAdded,
		_notifier, &Notifier::episodeReleased);

	_episodeCache = new EpisodeCache(this);
	connect(_episodeCache, &EpisodeCache::downloadComplete,
		this, &Core::onEpisodeDownloaded);
	connect(_episodeCache, &EpisodeCache::downloadComplete,
		_notifier, &Notifier::episodeDownloaded);

	_imageDownloader = new ImageDownloader(nullptr);

	_state = new State(this);
	connect(app, &QApplication::aboutToQuit,
		_state, &State::onAboutToQuit);
}

void Core::loadState()
{
	if(_feedCache)
		_feedCache->loadFromDisk();

	if(_state)
		_state->loadFromDisk();

	if (_settings)
		_settings->loadFromDisk();
}

void Core::removeEpisode(Episode* e)
{
	defaultPlaylist()->remove(e);

	if (_audioPlayer->currentEpisode() == e)
	{
		_audioPlayer->getMediaPlayer()->stop();
	}

	episodeCache()->cancelDownload(e);
	episodeCache()->deleteLocalFile(e);

	delete e;
}

void Core::onEpisodeDownloaded(Episode& e)
{
	const Feed* f = _feedCache->feedForEpisode(&e);
	const FeedSettings& settings = _settings->feed(f);

	switch (settings.autoPlaylistMode)
	{
	case AutoPlaylistMode::AutoEnqueueAfterCurrent:
		defaultPlaylist()->addAfterCurrent(&e);
		break;
	case AutoPlaylistMode::AutoEnqueueAtEnd:
		defaultPlaylist()->add(&e);
		break;
	case AutoPlaylistMode::NoAutoEnqueue:
	default:
		break;
	}
}
