#include "Core.h"

#include <QtWidgets/QApplication>

#include "AudioPlayer.h"
#include "EpisodeCache.h"
#include "Feed.h"
#include "FeedCache.h"
#include "ImageDownloader.h"

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

	_episodeCache = new EpisodeCache(nullptr);
	_episodeCache->loadDownloadQueueFromDisk(this);
	
	_imageDownloader = new ImageDownloader(nullptr);


	QObject::connect(app, &QApplication::aboutToQuit,
		_feedCache, &FeedCache::onAboutToQuit);
	QObject::connect(app, &QApplication::aboutToQuit,
		_episodeCache, &EpisodeCache::onAboutToQuit);
}
