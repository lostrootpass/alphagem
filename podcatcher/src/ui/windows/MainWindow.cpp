#include "MainWindow.h"

#include <QProgressBar>

#include "AddFeedWindow.h"
#include "AboutWindow.h"

#include "core/AudioPlayer.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

#include "ui/models/EpisodeListModel.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), _feedCache(nullptr), _epCache(nullptr)
{
	ui.setupUi(this);

	_progressBar = new QProgressBar();
	statusBar()->addPermanentWidget(_progressBar);

	connect(ui.listView, &QListView::activated, this, &MainWindow::onEpisodeSelected);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setAudioPlayer(AudioPlayer* player)
{
	_audioPlayer = player;

	ui.playbackControlWidget->connectToAudioPlayer(_audioPlayer);
}

void MainWindow::setEpisodeCache(EpisodeCache* epCache)
{
	_epCache = epCache;

	connect(_epCache, &EpisodeCache::downloadComplete, this, &MainWindow::onDownloadComplete);
	connect(_epCache, &EpisodeCache::downloadFailed, this, &MainWindow::onDownloadFailed);
	connect(_epCache, &EpisodeCache::downloadProgressUpdated, this, &MainWindow::onDownloadProgress);
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

void MainWindow::on_actionDownload_triggered()
{
	EpisodeListModel* model = (EpisodeListModel*)ui.listView->model();

	const Episode& ep = model->getEpisode(ui.listView->currentIndex());

	if(_epCache)
		_epCache->downloadEpisode(ep);
}

void MainWindow::onDownloadComplete(const Episode& e)
{
	QString message = QString("Download complete: %1").arg(e.title);
	statusBar()->showMessage(message);

	EpisodeListModel* model = (EpisodeListModel*)ui.listView->model();
	const Episode& ep = model->getEpisode(ui.listView->currentIndex());
	bool downloaded = _epCache->isDownloaded(&ep);
	ui.actionDownload->setEnabled(!downloaded);
}

void MainWindow::onDownloadFailed(const Episode&, QString error)
{
	QString message = QString("Download failed: %1").arg(error);
	statusBar()->showMessage(message);

	EpisodeListModel* model = (EpisodeListModel*)ui.listView->model();
	const Episode& e = model->getEpisode(ui.listView->currentIndex());
	bool downloaded = _epCache->isDownloaded(&e);
	ui.actionDownload->setEnabled(!downloaded);
}

void MainWindow::onDownloadProgress(const Episode&, qint64 bytesDownloaded)
{
	QString message = QString("Downloading... (%1KB)").arg(bytesDownloaded / 1024);
	statusBar()->showMessage(message);

	ui.actionDownload->setEnabled(false);
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

	connect(ui.listView->selectionModel(), &QItemSelectionModel::currentChanged,
		this, &MainWindow::onEpisodeHighlighted);
}

void MainWindow::onEpisodeHighlighted(const QModelIndex& index)
{
	EpisodeListModel* model = (EpisodeListModel*)ui.listView->model();

	const Episode& ep = model->getEpisode(index);

	if (_epCache)
	{
		if (_epCache->isDownloaded(&ep))
		{
			ui.actionDownload->setEnabled(false);
		}
		else if (!_epCache->downloadInProgress())
		{
			ui.actionDownload->setEnabled(true);
		}
	}
}

void MainWindow::onEpisodeSelected(const QModelIndex& index)
{
	EpisodeListModel* model = (EpisodeListModel*)ui.listView->model();

	const Episode& ep = model->getEpisode(index);

	_audioPlayer->playEpisode(&ep);
}
