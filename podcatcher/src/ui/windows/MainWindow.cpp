#include "MainWindow.h"

#include <QFileDialog>
#include <QProgressBar>

#include "AddFeedWindow.h"
#include "AboutWindow.h"

#include "core/AudioPlayer.h"
#include "core/ImageDownloader.h"
#include "core/Settings.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

#include "ui/models/EpisodeItemDelegate.h"
#include "ui/models/EpisodeListModel.h"
#include "ui/models/FeedItemDelegate.h"
#include "ui/models/FeedListModel.h"

#include "ui/widgets/EpisodeListItemWidget.h"

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
	ui.stackedWidget->setCurrentWidget(ui.feedListLayout);
	ui.feedDetailWidget->setVisible(false);

	{
		EpisodeItemDelegate* eid = new EpisodeItemDelegate(ui.episodeListView);
		ui.episodeListView->setItemDelegate(eid);
		ui.feedListView->setItemDelegate(new FeedItemDelegate(ui.feedListView));
	}

	ui.playbackControlWidget->setupConnections(_core);


	connect(_core->episodeCache(), &EpisodeCache::downloadComplete,
		this, &MainWindow::onDownloadComplete);
	connect(_core->episodeCache(), &EpisodeCache::downloadFailed,
		this, &MainWindow::onDownloadFailed);
	connect(_core->episodeCache(), &EpisodeCache::downloadProgressUpdated,
		this, &MainWindow::onDownloadProgress);


	FeedListModel* flm = new FeedListModel(*_core, this);
	ui.feedListView->setModel(flm);

	connect(ui.feedListView, &QListView::clicked,
		this, &MainWindow::onFeedSelected);

	ui.feedDetailWidget->setCore(_core);
	connect(ui.feedListView, &QListView::clicked,
		ui.feedDetailWidget, &FeedDetailWidget::onFeedSelected);
	ui.feedListView->addAction(ui.action_DeleteFeed);


	QAbstractItemModel* model = new EpisodeListModel(*ui.episodeListView,
		*_core, -1, this);
	ui.episodeListView->setModel(model);
	ui.episodeListView->setMouseTracking(true);
	connect(ui.episodeListView->selectionModel(), 
		&QItemSelectionModel::currentChanged,
		this, &MainWindow::onEpisodeHighlighted);
	connect(ui.episodeListView, &QListView::doubleClicked,
		this, &MainWindow::onEpisodeSelected);

	ui.episodeDetailWidget->init(_core);
}

void MainWindow::on_actionAdd_Feed_triggered()
{
	AddFeedWindow* feedWindow = new AddFeedWindow();
	feedWindow->setAttribute(Qt::WA_DeleteOnClose);

	connect(feedWindow, &AddFeedWindow::feedAdded,
		_core->feedCache(), &FeedCache::onFeedAdded);

	feedWindow->show();
}

void MainWindow::on_actionExport_OPML_triggered()
{
	QFileDialog* dialog = new QFileDialog(this);
	dialog->setModal(true);
	dialog->setDefaultSuffix(".opml");
	dialog->setFileMode(QFileDialog::AnyFile);
	dialog->setNameFilter(tr("OPML Files (*.opml)"));
	dialog->setWindowTitle(tr("Export OPML"));
	
	connect(dialog, &QFileDialog::fileSelected,
		_core->feedCache(), &FeedCache::onOPMLExported);

	dialog->show();
}

void MainWindow::on_actionImport_OPML_triggered()
{
	QFileDialog* dialog = new QFileDialog(this);
	dialog->setModal(true);
	dialog->setNameFilter(tr("OPML Files (*.opml)"));
	dialog->setWindowTitle(tr("Import OPML"));

	connect(dialog, &QFileDialog::fileSelected, 
		_core->feedCache(), &FeedCache::onOPMLImported);

	dialog->show();
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

void MainWindow::on_actionDownloads_triggered()
{
	QVector<Feed*>& feeds = _core->feedCache()->feeds();

	if (!feeds.size()) return;

	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	elm->showDownloadList();

	const QList<DownloadInfo*> dl = _core->episodeCache()->downloadList();
	if (!dl.size())
	{
		ui.feedDetailWidget->setFeed(nullptr);
	}
	else
	{
		Feed* f = _core->feedCache()->feedForEpisode(dl.first()->episode);
		ui.feedDetailWidget->setFeed(f);
	}

	ui.stackedWidget->setCurrentWidget(ui.episodeListLayout);
}

void MainWindow::on_actionHome_triggered()
{
	ui.feedDetailWidget->setVisible(false);
	ui.stackedWidget->setCurrentWidget(ui.feedListLayout);
}

void MainWindow::on_actionPlaylist_triggered()
{
	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	elm->showPlaylist();

	Playlist* p = _core->defaultPlaylist();
	if (!p->episodes.size())
	{
		ui.feedDetailWidget->setFeed(nullptr);
	}
	else
	{
		Feed* f = _core->feedCache()->feedForEpisode(p->episodes.first());
		ui.feedDetailWidget->setFeed(f);
	}

	ui.stackedWidget->setCurrentWidget(ui.episodeListLayout);
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

	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	elm->refreshList();
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

void MainWindow::onEpisodeHighlighted(const QModelIndex& index)
{
	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	Episode* e = elm->getEpisode(index);
	ui.feedDetailWidget->setFeed(_core->feedCache()->feedForEpisode(e));
}

void MainWindow::onEpisodeSelected(const QModelIndex& index)
{
	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	ui.episodeDetailWidget->setEpisode(elm->getEpisode(index));
	ui.stackedWidget->setCurrentWidget(ui.episodeDetailLayout);
}

void MainWindow::onStatusBarUpdate(QString& text)
{
	statusBar()->showMessage(text);
}

void MainWindow::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed*>& feeds = _core->feedCache()->feeds();

	if (!feeds.size()) return;

	EpisodeListModel* elm = (EpisodeListModel*)ui.episodeListView->model();
	elm->setFeedIndex(index.row());

	ui.feedDetailWidget->setVisible(true);
	ui.stackedWidget->setCurrentWidget(ui.episodeListLayout);
}
