#include "MainWindow.h"

#include <QProgressBar>

#include "AddFeedWindow.h"
#include "AboutWindow.h"

#include "core/AudioPlayer.h"
#include "core/ImageDownloader.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

#include "ui/models/EpisodeItemDelegate.h"
#include "ui/models/EpisodeListModel.h"
#include "ui/models/FeedListModel.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), _feedCache(nullptr), _epCache(nullptr)
{
	ui.setupUi(this);

	ui.episodeListView->setVisible(false);
	ui.feedDetailWidget->setVisible(false);

	ui.episodeListView->setItemDelegate(new EpisodeItemDelegate(ui.episodeListView));
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

	FeedListModel* flm = new FeedListModel(*cache, this);
	ui.feedListView->setModel(flm);

	connect(ui.feedListView, &QListView::activated, this, &MainWindow::onFeedSelected);

	ui.feedDetailWidget->setFeedCache(*cache);
	connect(ui.feedListView, &QListView::activated, ui.feedDetailWidget, &FeedDetailWidget::onFeedSelected);



	QAbstractItemModel* model = new EpisodeListModel(*ui.episodeListView, *_feedCache, -1, this);

	ui.episodeListView->setModel(model);

	connect(ui.episodeListView->selectionModel(), &QItemSelectionModel::currentChanged,
		this, &MainWindow::onEpisodeHighlighted);
}

void MainWindow::setImageDownloader(ImageDownloader* imageDownloader)
{
	_imageDownloader = imageDownloader;

	ui.feedDetailWidget->setImageDownloader(*imageDownloader);
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
	EpisodeListModel* model = (EpisodeListModel*)ui.episodeListView->model();

	const Episode& ep = model->getEpisode(ui.episodeListView->currentIndex());

	if(_epCache)
		_epCache->downloadEpisode(ep);
}

void MainWindow::on_actionHome_triggered()
{
	ui.feedDetailWidget->setVisible(false);
	ui.feedListView->setVisible(true);
	ui.episodeListView->setVisible(false);
}

void MainWindow::on_actionRefresh_triggered()
{
	if (_feedCache)
	{
		_feedCache->refresh(ui.feedListView->currentIndex().row());
	}
}

void MainWindow::onDownloadComplete(const Episode& e)
{
	QString message = QString(tr("Download complete: %1")).arg(e.title);
	statusBar()->showMessage(message);

	EpisodeListModel* model = (EpisodeListModel*)ui.episodeListView->model();
	const Episode& ep = model->getEpisode(ui.episodeListView->currentIndex());
	bool downloaded = _epCache->isDownloaded(&ep);
	ui.actionDownload->setEnabled(!downloaded);
}

void MainWindow::onDownloadFailed(const Episode&, QString error)
{
	QString message = QString(tr("Download failed: %1")).arg(error);
	statusBar()->showMessage(message);

	EpisodeListModel* model = (EpisodeListModel*)ui.episodeListView->model();
	const Episode& e = model->getEpisode(ui.episodeListView->currentIndex());
	bool downloaded = _epCache->isDownloaded(&e);
	ui.actionDownload->setEnabled(!downloaded);
}

void MainWindow::onDownloadProgress(const Episode&, qint64 bytesDownloaded)
{
	QString message = QString(tr("Downloading... (%1KB)")).arg(bytesDownloaded / 1024);
	statusBar()->showMessage(message);

	ui.actionDownload->setEnabled(false);
}

void MainWindow::onStatusBarUpdate(QString& text)
{
	statusBar()->showMessage(text);
}

void MainWindow::onFeedListUpdated()
{

}

void MainWindow::onEpisodeHighlighted(const QModelIndex& index)
{
	EpisodeListModel* model = (EpisodeListModel*)ui.episodeListView->model();

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
	EpisodeListModel* model = (EpisodeListModel*)ui.episodeListView->model();

	const Episode& ep = model->getEpisode(index);

	_audioPlayer->playEpisode(&ep);
	model->markAsPlayed(index);
}

void MainWindow::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed>& feeds = _feedCache->feeds();

	if (!feeds.size()) return;

	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	elm->setFeedIndex(index.row());

	ui.feedDetailWidget->setVisible(true);
	ui.feedListView->setVisible(false);
	ui.episodeListView->setVisible(true);
}
