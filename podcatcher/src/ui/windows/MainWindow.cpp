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

#include "ui/widgets/EpisodeDetailWidget.h"

MainWindow::MainWindow(Core& core, QWidget *parent)
	: QMainWindow(parent), _core(&core)
{
	ui.setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::init()
{
	ui.episodeListView->setVisible(false);
	ui.feedDetailWidget->setVisible(false);

	ui.episodeListView->setItemDelegate(new EpisodeItemDelegate(ui.episodeListView));


	ui.playbackControlWidget->connectToAudioPlayer(_core->audioPlayer());


	connect(_core->episodeCache(), &EpisodeCache::downloadComplete,
		this, &MainWindow::onDownloadComplete);
	connect(_core->episodeCache(), &EpisodeCache::downloadFailed, this, &MainWindow::onDownloadFailed);
	connect(_core->episodeCache(), &EpisodeCache::downloadProgressUpdated, this, &MainWindow::onDownloadProgress);


	FeedListModel* flm = new FeedListModel(*_core, this);
	ui.feedListView->setModel(flm);

	connect(ui.feedListView, &QListView::activated, this, &MainWindow::onFeedSelected);

	ui.feedDetailWidget->setCore(_core);
	connect(ui.feedListView, &QListView::activated, ui.feedDetailWidget, &FeedDetailWidget::onFeedSelected);
	ui.feedListView->addAction(ui.action_DeleteFeed);


	QAbstractItemModel* model = new EpisodeListModel(*ui.episodeListView, *_core, -1, this);
	ui.episodeListView->setModel(model);
}

void MainWindow::on_actionAdd_Feed_triggered()
{
	AddFeedWindow* feedWindow = new AddFeedWindow();
	feedWindow->setAttribute(Qt::WA_DeleteOnClose);

	connect(feedWindow, &AddFeedWindow::feedAdded, _core->feedCache(), &FeedCache::onFeedAdded);

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

void MainWindow::on_actionHome_triggered()
{
	ui.feedDetailWidget->setVisible(false);
	ui.feedListView->setVisible(true);
	ui.episodeListView->setVisible(false);
}

void MainWindow::on_actionRefresh_triggered()
{
	_core->feedCache()->refresh(ui.feedListView->currentIndex().row());
}

void MainWindow::on_action_DeleteFeed_triggered()
{
	if (!ui.feedListView->isVisible()) return;

	const QModelIndex& i = ui.feedListView->selectionModel()->currentIndex();

	if (!i.isValid()) return;

	_core->feedCache()->removeFeed(i.row());
}

void MainWindow::onDownloadComplete(const Episode& e)
{
	QString message = QString(tr("Download complete: %1")).arg(e.title);
	statusBar()->showMessage(message);
}

void MainWindow::onDownloadFailed(const Episode&, QString error)
{
	QString message = QString(tr("Download failed: %1")).arg(error);
	statusBar()->showMessage(message);
}

void MainWindow::onDownloadProgress(const Episode& e, qint64 bytesDownloaded)
{
	QString message = QString(tr("Downloading... (%1KB) [%2]"))
		.arg(bytesDownloaded / 1024).arg(e.title);
	statusBar()->showMessage(message);
}

void MainWindow::onStatusBarUpdate(QString& text)
{
	statusBar()->showMessage(text);
}

void MainWindow::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed>& feeds = _core->feedCache()->feeds();

	if (!feeds.size()) return;

	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	elm->setFeedIndex(index.row());

	ui.feedDetailWidget->setVisible(true);
	ui.feedListView->setVisible(false);
	ui.episodeListView->setVisible(true);
}
