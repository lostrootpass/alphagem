#include "MainWindow.h"

#include <QProgressBar>

#include "AddFeedWindow.h"
#include "AboutWindow.h"

#include "core/AudioPlayer.h"
#include "core/Feed.h"
#include "core/FeedCache.h"

#include "ui/models/EpisodeListModel.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), _feedCache(nullptr)
{
	ui.setupUi(this);

	_progressBar = new QProgressBar();
	statusBar()->addPermanentWidget(_progressBar);

	connect(ui.listView, &QListView::doubleClicked, this, &MainWindow::onEpisodeSelected);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setAudioPlayer(AudioPlayer* player)
{
	_audioPlayer = player;

	ui.playbackControlWidget->connectToAudioPlayer(_audioPlayer);
}

void MainWindow::setFeedCache(FeedCache* cache)
{
	_feedCache = cache;

	connect(_feedCache, &FeedCache::feedListUpdated, this, &MainWindow::onFeedListUpdated);
}

void MainWindow::on_actionAdd_Feed_triggered()
{
	AddFeedWindow* feedWindow = new AddFeedWindow();
	feedWindow->setAttribute(Qt::WA_DeleteOnClose);

	if (_feedCache)
	{
		connect(feedWindow, &AddFeedWindow::feedAdded, _feedCache, &FeedCache::onFeedAdded);
	}

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

void MainWindow::onStatusBarUpdate(QString& text)
{
	statusBar()->showMessage(text);
}

void MainWindow::onFeedListUpdated()
{
	QVector<Feed>& feeds = _feedCache->feeds();

	if (!feeds.size()) return;

	Feed* feed = &feeds[0];

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
