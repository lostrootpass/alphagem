#ifndef CORE_H_
#define CORE_H_

class AudioPlayer;
class EpisodeCache;
class FeedCache;
class ImageDownloader;
class Notifier;
class Settings;
class State;

#include <QList>

#include "Playlist.h"

#include "core/feeds/Feed.h"
#include "core/feeds/FeedSettings.h"

class Core : public QObject
{
	Q_OBJECT
public:
	Core();
	~Core();

	inline AudioPlayer* audioPlayer() const { return _audioPlayer; }
	inline EpisodeCache* episodeCache() const { return _episodeCache; }
	inline FeedCache* feedCache() const { return _feedCache; }
	inline ImageDownloader* imageDownloader() const { return _imageDownloader; }
	inline Notifier* notifier() const { return _notifier; }
	inline Settings* settings() const { return _settings; }

	inline Playlist* defaultPlaylist() 
	{
		if (!_playlists.size())
			_playlists.push_back(new Playlist(QObject::tr("Playlist")));

		return _playlists.first();
	}

	void init(QApplication* app);

	void loadState();

	void removeEpisode(Episode* e);

private:
	QList<Playlist*> _playlists;

	AudioPlayer* _audioPlayer;
	EpisodeCache* _episodeCache;
	FeedCache* _feedCache;
	ImageDownloader* _imageDownloader;
	Notifier* _notifier;
	Settings* _settings;
	State* _state;

private slots:
	void onEpisodeDownloaded(Episode& e);
};

#endif