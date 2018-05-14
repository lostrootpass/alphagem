#include "MainWindow.h"

#include <QFileDialog>
#include <QProgressBar>
#include <QTimer>

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

#include "ui/windows/ApplicationSettingsWindow.h"
#include "EpisodeListProxyModel.h"

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
	connect(_core->episodeCache(), &EpisodeCache::downloadPaused,
		this, &MainWindow::onDownloadPaused);
	connect(_core->episodeCache(), &EpisodeCache::downloadProgressUpdated,
		this, &MainWindow::onDownloadProgress);


	FeedListModel* flm = new FeedListModel(*_core, this);
	ui.feedListView->setModel(flm);

	connect(ui.feedListView, &QListView::clicked,
		this, &MainWindow::onFeedSelected);

	ui.feedDetailWidget->setCore(_core);
	connect(ui.feedListView, &QListView::clicked,
		ui.feedDetailWidget, &FeedDetailWidget::onFeedSelected);
	connect(ui.feedDetailWidget, &FeedDetailWidget::iconClicked,
		this, &MainWindow::onFeedJump);
	ui.feedListView->addAction(ui.action_DeleteFeed);


	_episodeListModel = new EpisodeListModel(*_core, -1, this);
	_episodeProxyModel = new EpisodeListProxyModel(*_core, 
		*ui.episodeListView, this);
	_episodeProxyModel->setSourceModel(_episodeListModel);
	ui.episodeListView->setModel(_episodeProxyModel);
	ui.episodeListView->setMouseTracking(true);
	connect(ui.episodeListView->selectionModel(), 
		&QItemSelectionModel::currentChanged,
		this, &MainWindow::onEpisodeHighlighted);
	connect(ui.episodeListView, &QListView::doubleClicked,
		this, &MainWindow::onEpisodeSelected);

	ui.episodeDetailWidget->init(_core);

	connect(ui.playbackControlWidget, &PlaybackControlWidget::episodeSelected,
		this, &MainWindow::onEpisodeJump);

	connect(_core->defaultPlaylist(), &Playlist::playlistUpdated,
		this, &MainWindow::onPlaylistUpdated);

	_searchbarTimer = new QTimer(this);
	_searchbarTimer->setSingleShot(true);
	connect(_searchbarTimer, &QTimer::timeout,
		this, &MainWindow::on_searchbar_textChanged_timeout);
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

void MainWindow::on_actionSettings_triggered()
{
	ApplicationSettingsWindow* asw = 
		new ApplicationSettingsWindow(_core, nullptr);
	asw->setAttribute(Qt::WA_DeleteOnClose);
	asw->setWindowModality(Qt::ApplicationModal);
	asw->setWindowTitle(tr("Settings"));
	asw->show();
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

	_episodeListModel->showDownloadList();

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
	_episodeListModel->showPlaylist();

	Playlist* p = _core->defaultPlaylist();
	if (!p->episodes.size())
	{
		ui.feedDetailWidget->setFeed(nullptr);
		ui.feedDetailWidget->setVisible(false);
	}
	else
	{
		Feed* f = _core->feedCache()->feedForEpisode(p->episodes.first());
		ui.feedDetailWidget->setFeed(f);
		ui.feedDetailWidget->setVisible(true);
	}

	ui.stackedWidget->setCurrentWidget(ui.episodeListLayout);
}

void MainWindow::on_searchbar_textChanged(const QString&)
{
	_searchbarTimer->start(150);
}

void MainWindow::on_searchbar_textChanged_timeout()
{
	_episodeProxyModel->setFilterFixedString(ui.searchbar->text());
}

void MainWindow::on_filterDropdown_currentIndexChanged(int idx)
{
	switch (idx)
	{
	case 0:
		//All episodes
		_episodeProxyModel->setFilterDownloadedOnly(false);
		_episodeProxyModel->setFilterNewOnly(false);
		break;
	case 1:
		//New only
		_episodeProxyModel->setFilterDownloadedOnly(false);
		_episodeProxyModel->setFilterNewOnly(true);
		break;
	case 2:
		//Downloaded only
		_episodeProxyModel->setFilterDownloadedOnly(true);
		_episodeProxyModel->setFilterNewOnly(false);
		break;
	case 3:
		//Downloaded & new only
		_episodeProxyModel->setFilterDownloadedOnly(true);
		_episodeProxyModel->setFilterNewOnly(true);
		break;
	default:
		break;
	}

	_episodeListModel->refreshList();
}

void MainWindow::on_clearFilterButton_clicked()
{
	ui.searchbar->clear();
	ui.filterDropdown->setCurrentIndex(0);
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

	_episodeListModel->refreshList();
}

void MainWindow::onDownloadFailed(const Episode&, QString error)
{
	QString message = QString(tr("Download failed: %1")).arg(error);
	statusBar()->showMessage(message);
}

void MainWindow::onDownloadPaused()
{
	statusBar()->clearMessage();
}

void MainWindow::onDownloadProgress(const Episode& e, qint64 bytesDownloaded)
{
	QString message = QString(tr("Downloading... (%1KB) [%2]"))
		.arg(bytesDownloaded / 1024).arg(e.title);
	statusBar()->showMessage(message);
}

void MainWindow::onEpisodeHighlighted(const QModelIndex& index)
{
	Episode* e = _episodeProxyModel->getEpisode(index);
	ui.feedDetailWidget->setFeed(_core->feedCache()->feedForEpisode(e));
}

void MainWindow::onEpisodeJump(Episode* e)
{
	Feed* f = _core->feedCache()->feedForEpisode(e);
	ui.feedDetailWidget->setFeed(f);
	ui.feedDetailWidget->setVisible(true);
	ui.episodeDetailWidget->setEpisode(e);
	ui.stackedWidget->setCurrentWidget(ui.episodeDetailLayout);
}

void MainWindow::onEpisodeSelected(const QModelIndex& index)
{
	Episode* e = _episodeProxyModel->getEpisode(index);
	ui.episodeDetailWidget->setEpisode(e);
	ui.stackedWidget->setCurrentWidget(ui.episodeDetailLayout);
}

void MainWindow::onFeedJump(Feed* f)
{
	ui.feedDetailWidget->setFeed(f);
	_episodeListModel->setFeedIndex(_core->feedCache()->feeds().indexOf(f));
	ui.stackedWidget->setCurrentWidget(ui.episodeListLayout);
}

void MainWindow::onStatusBarUpdate(QString& text)
{
	statusBar()->showMessage(text);
}

void MainWindow::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed*>& feeds = _core->feedCache()->feeds();

	if (!feeds.size()) return;

	_episodeListModel->setFeedIndex(index.row());

	ui.feedDetailWidget->setVisible(true);
	ui.stackedWidget->setCurrentWidget(ui.episodeListLayout);
}

void MainWindow::onPlaylistUpdated()
{
	Playlist* p = _core->defaultPlaylist();
	ui.actionPlaylist->setEnabled(!p->episodes.isEmpty());

	if (!(_episodeListModel->listType() == EpisodeListType::Playlist &&
		ui.stackedWidget->currentWidget() == ui.episodeListLayout))
	{
		return;
	}

	if (!p->episodes.size())
	{
		ui.feedDetailWidget->setFeed(nullptr);
		ui.feedDetailWidget->setVisible(false);
	}
	else if(!ui.episodeListView->currentIndex().isValid())
	{
		Feed* f = _core->feedCache()->feedForEpisode(p->episodes.first());
		ui.feedDetailWidget->setFeed(f);
		ui.feedDetailWidget->setVisible(true);
	}
}
