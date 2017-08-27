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
	core.init(&a);

	MainWindow w(core);
	w.init();

	//Load the state now that the main window is hooked up, but...
	core.loadState();

	//...avoid showing the UI until here, to avoid unresponsiveness.
	w.show();

	core.feedCache()->startupRefresh();

	return a.exec();
}
