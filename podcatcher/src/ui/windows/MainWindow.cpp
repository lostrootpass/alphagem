#include "MainWindow.h"

#include <QProgressBar>

#include "AddFeedWindow.h"
#include "AboutWindow.h"

#include "core/AudioPlayer.h"
#include "core/Feed.h"
#include "core/FeedParser.h"

#include "ui/models/EpisodeListModel.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), _feedParser(nullptr)
{
	ui.setupUi(this);

	_progressBar = new QProgressBar();
	statusBar()->addPermanentWidget(_progressBar);

	connect(ui.listView, &QListView::doubleClicked, this, &MainWindow::onEpisodeSelected);
}

MainWindow::~MainWindow()
{
	delete _feedParser;
}

void MainWindow::setAudioPlayer(AudioPlayer* player)
{
	_audioPlayer = player;

	ui.playbackControlWidget->connectToAudioPlayer(_audioPlayer);
}

void MainWindow::on_actionAdd_Feed_triggered()
{
	AddFeedWindow* feedWindow = new AddFeedWindow();
	feedWindow->setAttribute(Qt::WA_DeleteOnClose);

	connect(feedWindow, &AddFeedWindow::feedAdded, this, &MainWindow::onFeedAdded);

	feedWindow->show();
}

void MainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
	QApplication::aboutQt();
}

void MainWindow::on_actionAbout_triggered()
{
	AboutWindow* window = new AboutWindow();
	window->setAttribute(Qt::WA_DeleteOnClose);

	window->show();
}

void MainWindow::onFeedAdded(QString& feed)
{
	if (!_feedParser)
	{
		_feedParser = new FeedParser(nullptr);

		connect(_feedParser, &FeedParser::downloadFailed, this, &MainWindow::onStatusBarUpdate);
		connect(_feedParser, &FeedParser::updateProgress, _progressBar, &QProgressBar::setValue);
		connect(_feedParser, &FeedParser::feedRetrieved, this, &MainWindow::onFeedRetrieved);
	}



	_feedParser->parseFromRemoteFile(feed);
}

void MainWindow::onStatusBarUpdate(QString& text)
{
	statusBar()->showMessage(text);
}

void MainWindow::onFeedRetrieved(Feed* feed)
{
	ui.feedNameLabel->setText(feed->title);

	QAbstractItemModel* model = new EpisodeListModel(feed->episodes, 0);

	ui.listView->setModel(model);
}

void MainWindow::onEpisodeSelected(const QModelIndex& index)
{
	EpisodeListModel* model = (EpisodeListModel*)ui.listView->model();

	const Episode& ep = model->getEpisode(index);

	_audioPlayer->playEpisode(&ep);
}
