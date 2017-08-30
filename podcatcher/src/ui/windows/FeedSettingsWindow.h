#pragma once

#include <QDialog>
#include "ui_FeedSettingsWindow.h"

#include "core/feeds/FeedSettings.h"

class Core;
struct Feed;

class FeedSettingsWindow : public QDialog
{
	Q_OBJECT

public:
	FeedSettingsWindow(Core* c, Feed* f, QWidget *parent = Q_NULLPTR);
	~FeedSettingsWindow();

signals:
	void feedSettingsChanged(Feed* feed);

private:
	Ui::FeedSettingsWindow ui;

	Core* _core;
	Feed* _feed;

	void _updateUI(const FeedSettings& settings);

private slots:
	/* Meta */
	void on_useGlobalDefaultsCheckBox_stateChanged(int state);
	void on_markAllAsListenedButton_clicked();

	/* Save/Cancel buttons */
	void on_saveButtonBox_accepted();
	void on_saveButtonBox_clicked(QAbstractButton*);
	void on_saveButtonBox_rejected();

	/* Feed */
	void on_feedUrlButton_clicked();
	void on_refreshEveryCheckBox_stateChanged(int state);
	void on_ignoreOlderThanCheckBox_stateChanged(int state);

	/* Playback */
	void on_skipFirstCheckBox_stateChanged(int state);
	void on_skipLastCheckBox_stateChanged(int state);

	/* Download */
	void on_autoDownloadCheckBox_stateChanged(int state);
	void on_autoPlaylistCheckBox_stateChanged(int state);
	void on_limitStorageCheckBox_stateChanged(int state);
	void on_deleteLocalPlayedButton_clicked();
};
