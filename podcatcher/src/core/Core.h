#ifndef CORE_H_
#define CORE_H_

class AudioPlayer;
class EpisodeCache;
class FeedCache;
class ImageDownloader;

class Core
{
public:
	Core();
	~Core();

	inline AudioPlayer* audioPlayer() const { return _audioPlayer; }
	inline EpisodeCache* episodeCache() const { return _episodeCache; }
	inline FeedCache* feedCache() const { return _feedCache; }
	inline ImageDownloader* imageDownloader() const { return _imageDownloader; }

	void init();

private:
	AudioPlayer* _audioPlayer;
	EpisodeCache* _episodeCache;
	FeedCache* _feedCache;
	ImageDownloader* _imageDownloader;
};

#endif