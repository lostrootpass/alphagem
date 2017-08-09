#include "ui/windows/MainWindow.h"
#include <QtWidgets/QApplication>

#include "core/AudioPlayer.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	AudioPlayer* player = new AudioPlayer(&a);

	MainWindow w;
	w.setAudioPlayer(player);
	w.show();
	return a.exec();
}
