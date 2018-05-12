#include "ui/windows/MainWindow.h"
#include <QtWidgets/QApplication>

#include "core/AudioPlayer.h"
#include "Core/Core.h"
#include "core/ImageDownloader.h"

#include "core/feeds/EpisodeCache.h"
#include "core/feeds/FeedCache.h"

#include <QFile>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QApplication::setWindowIcon(QIcon(":/icons/alphagem/alphagem"));
	QIcon::setThemeName("breeze");
	{
		QFile f(":/stylesheet.css");
		if (f.exists())
		{
			QCoreApplication::setAttribute(
				Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
			f.open(QIODevice::ReadOnly);
			a.setStyleSheet(f.readAll());
			f.close();
		}
	}

	Core core;
	core.init(&a);

	core.feedCache()->startupRefresh();

	return a.exec();
}
