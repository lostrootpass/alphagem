#include "FeedSettingsWindow.h"

#include <QApplication>
#include <QClipboard>

#include "core/Core.h"
#include "core/Settings.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedSettings.h"

#include "ui/windows/MainWindow.h"

FeedSettingsWindow::FeedSettingsWindow(Core* c, Feed* f, QWidget *parent)
	: QDialog(parent), _core(c), _feed(f)
{
	ui.setupUi(this);

	if (_feed)
	{
		ui.feedTitleLabel->setText(_feed->title);
		ui.useGlobalDefaultsCheckBox->setChecked(_feed->useGlobalSettings);
		ui.feedUrlLineEdit->setText(f->feedUrl);
	}
	else
	{
		ui.feedTitleLabel->setText(tr("Default Settings"));
		ui.markAllAsListenedButton->setVisible(false);
		ui.useGlobalDefaultsCheckBox->setVisible(false);
		ui.feedUrlGroupBox->setVisible(false);
	}
}

FeedSettingsWindow::~FeedSettingsWindow()
{
}

void FeedSettingsWindow::_updateUI(const FeedSettings& settings)
{
	if (settings.episodeOrder == EpisodeOrder::NewestFirst)
		ui.episodeOrderComboBox->setCurrentIndex(0);
	else
		ui.episodeOrderComboBox->setCurrentIndex(1);

	ui.refreshOnStartupCheckBox->setChecked(settings.refreshAtStartup);
	
	ui.refreshEveryCheckBox->setChecked(settings.enableRefreshPeriod);
	if (settings.refreshPeriod < 24)
	{
		ui.refreshEverySpinBox->setValue(settings.refreshPeriod);
		ui.refreshEveryComboBox->setCurrentIndex(0);
	}
	else
	{
		ui.refreshEverySpinBox->setValue(settings.refreshPeriod / 24);
		ui.refreshEveryComboBox->setCurrentIndex(1);
	}

	ui.ignoreOlderThanCheckBox->setChecked(settings.enableThreshold);
	if (settings.ignoreThreshold < 7)
	{
		ui.ignoreOlderThanSpinBox->setValue(settings.ignoreThreshold);
		ui.ignoreOlderThanComboBox->setCurrentIndex(0);
	}
	else
	{
		ui.ignoreOlderThanSpinBox->setValue(settings.ignoreThreshold / 7);
		ui.ignoreOlderThanComboBox->setCurrentIndex(1);
	}

	bool release = settings.showReleaseNotification;
	bool download = settings.showDownloadNotification;
	ui.releaseNotificationCheckBox->setChecked(release);
	ui.downloadNotificationCheckBox->setChecked(download);

	ui.skipFirstCheckBox->setChecked(settings.enableSkipIntroSting);
	ui.skipFirstSpinBox->setValue(settings.introStingLength);

	ui.skipLastCheckBox->setChecked(settings.enableSkipOutroSting);
	ui.skipLastSpinBox->setValue(settings.outroStingLength);

	ui.skipBackToBackCheckBox->setChecked(settings.skipOnlyOnBackToBack);
	ui.continueListeningCheckBox->setChecked(settings.autoContinueListening);

	ui.autoDownloadCheckBox->setChecked(settings.autoDownloadNextEpisodes);
	ui.maximumSimtaneousSpinBox->setValue(settings.maxSimultaneousDownloads);

	if (settings.autoPlaylistMode == AutoPlaylistMode::NoAutoEnqueue)
	{
		ui.autoPlaylistCheckBox->setChecked(false);
	}
	else
	{
		ui.autoPlaylistCheckBox->setChecked(true);

		if (settings.autoPlaylistMode == AutoPlaylistMode::AutoEnqueueAtEnd)
			ui.autoPlaylistComboBox->setCurrentIndex(0);
		else
			ui.autoPlaylistComboBox->setCurrentIndex(1);
	}
	ui.autoDeleteCheckBox->setChecked(settings.deleteAfterPlayback);

	ui.limitStorageCheckBox->setEnabled(settings.autoDownloadNextEpisodes);
	ui.limitStorageCheckBox->setChecked(settings.enableLocalStorageLimit);
	ui.limitStorageSpinBox->setValue(settings.localEpisodeStorageLimit);
}

void FeedSettingsWindow::on_useGlobalDefaultsCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Unchecked);

	ui.feedGroupBox->setEnabled(e);
	ui.playbackGroupBox->setEnabled(e);
	ui.downloadsGroupBox->setEnabled(e);
	ui.notificationGroupBox->setEnabled(e);

	if (e)
		_updateUI(_core->settings()->feedDefaults());
	else
		_updateUI(_feed->settings);
}

void FeedSettingsWindow::on_markAllAsListenedButton_clicked()
{
	for (Episode* e : _feed->episodes)
	{
		e->setListened();
	}
}

void FeedSettingsWindow::on_saveButtonBox_accepted()
{
	FeedSettings f;

	/* Feed section*/
	if (ui.episodeOrderComboBox->currentIndex() == 0)
		f.episodeOrder = EpisodeOrder::NewestFirst;
	else
		f.episodeOrder = EpisodeOrder::OldestFirst;

	f.refreshAtStartup = ui.refreshOnStartupCheckBox->isChecked();
	
	f.enableRefreshPeriod = ui.refreshEveryCheckBox->isChecked();
	f.refreshPeriod = ui.refreshEverySpinBox->value();
	if (ui.refreshEveryComboBox->currentIndex() == 1)
		f.refreshPeriod *= 24;
	
	f.enableThreshold = ui.ignoreOlderThanCheckBox->isChecked();
	f.ignoreThreshold = ui.ignoreOlderThanSpinBox->value();
	if (ui.ignoreOlderThanComboBox->currentIndex() == 1)
		f.ignoreThreshold *= 7;

	/* Notification setting */
	f.showReleaseNotification = ui.releaseNotificationCheckBox->isChecked();
	f.showDownloadNotification = ui.downloadNotificationCheckBox->isChecked();

	/* Playback section */
	f.enableSkipIntroSting = ui.skipFirstCheckBox->isChecked();
	f.introStingLength = ui.skipFirstSpinBox->value();

	f.enableSkipOutroSting = ui.skipLastCheckBox->isChecked();
	f.outroStingLength = ui.skipLastSpinBox->value();

	f.skipOnlyOnBackToBack = ui.skipBackToBackCheckBox->isChecked();

	f.autoContinueListening = ui.continueListeningCheckBox->isChecked();

	/* Download section */
	f.autoDownloadNextEpisodes = ui.autoDownloadCheckBox->isChecked();
	f.maxSimultaneousDownloads = ui.maximumSimtaneousSpinBox->value();
	if (!ui.autoPlaylistCheckBox->isChecked())
	{
		f.autoPlaylistMode = AutoPlaylistMode::NoAutoEnqueue;
	}
	else
	{
		if (ui.autoPlaylistComboBox->currentIndex() == 0)
			f.autoPlaylistMode = AutoPlaylistMode::AutoEnqueueAtEnd;
		else
			f.autoPlaylistMode = AutoPlaylistMode::AutoEnqueueAfterCurrent;
	}

	f.deleteAfterPlayback = ui.autoDeleteCheckBox->isChecked();

	f.enableLocalStorageLimit = ui.limitStorageCheckBox->isChecked();
	f.localEpisodeStorageLimit = ui.limitStorageSpinBox->value();



	if (_feed)
	{
		_core->settings()->updateFeedSettings(*_feed, f);
		_feed->useGlobalSettings = ui.useGlobalDefaultsCheckBox->isChecked();
	}
	else
	{
		_core->settings()->setFeedDefaults(f);
	}

	emit feedSettingsChanged(_feed);
	close();
}

void FeedSettingsWindow::on_saveButtonBox_clicked(QAbstractButton* which)
{
	if (which == ui.saveButtonBox->button(QDialogButtonBox::RestoreDefaults))
	{
		if (_feed)
			_updateUI(_core->settings()->feedDefaults());
		else
			_updateUI(FeedSettings());
	}
	else if (which == ui.saveButtonBox->button(QDialogButtonBox::Reset))
	{
		if (_feed)
			_updateUI(_feed->settings);
		else
			_updateUI(_core->settings()->feedDefaults());
	}
}

void FeedSettingsWindow::on_saveButtonBox_rejected()
{
	close();
}

void FeedSettingsWindow::on_feedUrlButton_clicked()
{
	QApplication::clipboard()->setText(_feed->feedUrl);
}

void FeedSettingsWindow::on_refreshEveryCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Checked);
	ui.refreshEverySpinBox->setEnabled(e);
	ui.refreshEveryComboBox->setEnabled(e);
}

void FeedSettingsWindow::on_ignoreOlderThanCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Checked);
	ui.ignoreOlderThanSpinBox->setEnabled(e);
	ui.ignoreOlderThanComboBox->setEnabled(e);
}

void FeedSettingsWindow::on_skipFirstCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Checked);
	ui.skipFirstSpinBox->setEnabled(e);
}

void FeedSettingsWindow::on_skipLastCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Checked);
	ui.skipLastSpinBox->setEnabled(e);
}

void FeedSettingsWindow::on_autoDownloadCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Checked);
	ui.limitStorageCheckBox->setEnabled(e);
	ui.maximumSimtaneousSpinBox->setEnabled(e);
}

void FeedSettingsWindow::on_autoPlaylistCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Checked);
	ui.autoPlaylistComboBox->setEnabled(e);
}

void FeedSettingsWindow::on_limitStorageCheckBox_stateChanged(int state)
{
	bool e = (state == Qt::CheckState::Checked);
	ui.limitStorageSpinBox->setEnabled(e);
}

void FeedSettingsWindow::on_deleteLocalPlayedButton_clicked()
{
	for (Episode* e : _feed->episodes)
	{
		_core->episodeCache()->deleteLocalFile(e);
	}

	ui.deleteLocalPlayedButton->setEnabled(false);
}
