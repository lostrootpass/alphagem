#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "core/Core.h"

class EpisodeDetailWidget;
struct Feed;
struct Episode;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Core& core, QWidget *parent = Q_NULLPTR);
	~MainWindow();

	void init();

private:
	Ui::MainWindowClass ui;

	Core* _core;

private slots:
	/* Auto generated slots */

	//File menu
	void on_actionAdd_Feed_triggered();
	void on_actionQuit_triggered();

	//Help menu
	void on_actionAbout_Qt_triggered();
	void on_actionAbout_triggered();

	//Toolbar
	void on_actionDownloads_triggered();
	void on_actionHome_triggered();

	//Context
	void on_action_DeleteFeed_triggered();

	/* Custom slots */
	void onDownloadComplete(const Episode& e);
	void onDownloadFailed(const Episode& e, QString error);
	void onDownloadProgress(const Episode& e, qint64 bytesDownloaded);
	void onFeedSelected(const QModelIndex& index);
	void onStatusBarUpdate(QString& text);
};
