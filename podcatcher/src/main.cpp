#include "ui/windows/MainWindow.h"
#include <QtWidgets/QApplication>

#include "core/AudioPlayer.h"
#include "Core/Core.h"
#include "core/ImageDownloader.h"

#include "core/feeds/EpisodeCache.h"
#include "core/feeds/FeedCache.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Core core;
	core.init();

	QObject::connect(&a, &QApplication::aboutToQuit, core.feedCache(), &FeedCache::onAboutToQuit);

	MainWindow w(core);
	w.init();
	w.show();
	return a.exec();
}
