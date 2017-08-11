#include "ui/windows/MainWindow.h"
#include <QtWidgets/QApplication>

#include "core/AudioPlayer.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/FeedCache.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	AudioPlayer* player = new AudioPlayer(&a);
	EpisodeCache* epCache = new EpisodeCache(&a);
	FeedCache* feedCache = new FeedCache(&a);

	QObject::connect(&a, &QApplication::aboutToQuit, feedCache, &FeedCache::onAboutToQuit);

	MainWindow w;
	w.setAudioPlayer(player);
	w.setEpisodeCache(epCache);
	w.setFeedCache(feedCache);

	feedCache->loadFromDisk();

	w.show();
	return a.exec();
}
