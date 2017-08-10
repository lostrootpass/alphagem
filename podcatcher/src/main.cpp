#include "ui/windows/MainWindow.h"
#include <QtWidgets/QApplication>

#include "core/AudioPlayer.h"
#include "core/FeedCache.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	AudioPlayer* player = new AudioPlayer(&a);
	FeedCache* cache = new FeedCache(&a);

	MainWindow w;
	w.setAudioPlayer(player);
	w.setFeedCache(cache);

	cache->loadFromDisk();

	w.show();
	return a.exec();
}
