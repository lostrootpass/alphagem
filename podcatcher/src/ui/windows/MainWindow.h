#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "core/Core.h"

class EpisodeListItemWidget;
struct Feed;
struct Episode;
class EpisodeListModel;
class EpisodeListProxyModel;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Core& core, QWidget *parent = Q_NULLPTR);
	~MainWindow();

	void init();

private:
	Ui::MainWindowClass ui;
	QTimer* _searchbarTimer;

	Core* _core;
	EpisodeListModel* _episodeListModel;
	EpisodeListProxyModel* _episodeProxyModel;

private slots:
	/* Auto generated slots */

	//File menu
	void on_actionAdd_Feed_triggered();
	void on_actionExport_OPML_triggered();
	void on_actionImport_OPML_triggered();
	void on_actionSettings_triggered();
	void on_actionQuit_triggered();

	//Help menu
	void on_actionAbout_Qt_triggered();
	void on_actionAbout_triggered();

	//Toolbar
	void on_actionDownloads_triggered();
	void on_actionHome_triggered();
	void on_actionPlaylist_triggered();

	//Filter bar
	void on_searchbar_textChanged(const QString& text);
	void on_searchbar_textChanged_timeout();
	void on_filterDropdown_currentIndexChanged(int idx);
	void on_clearFilterButton_clicked();

	//Context
	void on_action_DeleteFeed_triggered();

	/* Custom slots */
	void onDownloadComplete(const Episode& e);
	void onDownloadFailed(const Episode& e, QString error);
	void onDownloadPaused();
	void onDownloadProgress(const Episode& e, qint64 bytesDownloaded);
	void onEpisodeHighlighted(const QModelIndex& index);
	void onEpisodeJump(Episode* e);
	void onEpisodeSelected(const QModelIndex& index);
	void onFeedJump(Feed* f);
	void onFeedSelected(const QModelIndex& index);
	void onPlaylistUpdated();
	void onStatusBarUpdate(QString& text);
};
