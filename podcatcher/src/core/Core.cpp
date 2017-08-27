#include "Core.h"

#include <QtWidgets/QApplication>

#include "AudioPlayer.h"
#include "EpisodeCache.h"
#include "Feed.h"
#include "FeedCache.h"
#include "ImageDownloader.h"
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

	_episodeCache = new EpisodeCache(this);
	connect(_episodeCache, &EpisodeCache::downloadComplete,
		this, &Core::onEpisodeDownloaded);

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
	episodeCache()->cancelDownload(e);

	if (_audioPlayer->currentEpisode() == e)
	{
		_audioPlayer->getMediaPlayer()->stop();
	}

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
