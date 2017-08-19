#include "Core.h"

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
}

void Core::init()
{
	_audioPlayer = new AudioPlayer(nullptr);
	
	_episodeCache = new EpisodeCache(nullptr);
	
	_feedCache = new FeedCache(nullptr);
	_feedCache->loadFromDisk();

	_imageDownloader = new ImageDownloader(nullptr);
}