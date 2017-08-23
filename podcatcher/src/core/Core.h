#ifndef CORE_H_
#define CORE_H_

class AudioPlayer;
class EpisodeCache;
class FeedCache;
class ImageDownloader;
class State;

#include <QList>

#include "Playlist.h"

class Core
{
public:
	Core();
	~Core();

	inline AudioPlayer* audioPlayer() const { return _audioPlayer; }
	inline EpisodeCache* episodeCache() const { return _episodeCache; }
	inline FeedCache* feedCache() const { return _feedCache; }
	inline ImageDownloader* imageDownloader() const { return _imageDownloader; }

	inline Playlist* defaultPlaylist() 
	{
		if (!_playlists.size())
			_playlists.push_back(new Playlist(QObject::tr("Playlist")));

		return _playlists.first();
	}

	void init(QApplication* app);

	void loadState();

private:
	QList<Playlist*> _playlists;

	AudioPlayer* _audioPlayer;
	EpisodeCache* _episodeCache;
	FeedCache* _feedCache;
	ImageDownloader* _imageDownloader;
	State* _state;
};

#endif