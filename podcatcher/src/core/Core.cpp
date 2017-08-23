#include "Core.h"

#include <QtWidgets/QApplication>

#include "AudioPlayer.h"
#include "EpisodeCache.h"
#include "Feed.h"
#include "FeedCache.h"
#include "ImageDownloader.h"
#include "State.h"

Core::Core() : _audioPlayer(nullptr), _episodeCache(nullptr),
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
	_audioPlayer = new AudioPlayer(*this, nullptr);
	
	_feedCache = new FeedCache(nullptr);
	_feedCache->loadFromDisk();
	QObject::connect(app, &QApplication::aboutToQuit,
		_feedCache, &FeedCache::onAboutToQuit);

	_episodeCache = new EpisodeCache(nullptr);

	_imageDownloader = new ImageDownloader(nullptr);

	_state = new State(this);
	QObject::connect(app, &QApplication::aboutToQuit,
		_state, &State::onAboutToQuit);
}

void Core::loadState()
{
	if(_state)
		_state->loadFromDisk();
}
