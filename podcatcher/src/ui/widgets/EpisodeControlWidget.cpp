#include "EpisodeControlWidget.h"

#include "core/AudioPlayer.h"
#include "core/Core.h"
#include "core/Playlist.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"

EpisodeControlWidget::EpisodeControlWidget(QWidget *parent)
	: QWidget(parent), _core(nullptr), _episode(nullptr)
{
	ui.setupUi(this);
}

EpisodeControlWidget::~EpisodeControlWidget()
{
}

void EpisodeControlWidget::init(Core* core)
{
	_core = core;

	connect(_core->audioPlayer(), &AudioPlayer::episodeChanged,
		this, &EpisodeControlWidget::onEpisodeChanged);

	connect(_core->episodeCache(), &EpisodeCache::downloadProgressUpdated,
		this, &EpisodeControlWidget::onDownloadProgressUpdate);

	connect(_core->episodeCache(), &EpisodeCache::downloadComplete,
		this, &EpisodeControlWidget::onDownloadFinished);
}

void EpisodeControlWidget::update(Episode* e)
{
	_episode = e;

	_updatePlayButtonStatus();
	_updateDownloadButtonStatus();
	_updatePlaylistButtonStatus();
}

void EpisodeControlWidget::_updatePlayButtonStatus()
{
	if (_episode->duration > 0)
	{
		QString formatString = QString(tr("Play (%1:%2)"))
			.arg(_episode->duration / 60, 2, 10, QChar('0'))
			.arg(_episode->duration % 60, 2, 10, QChar('0'));

		ui.playButton->setText(formatString);
	}
	else
	{
		ui.playButton->setText(tr("Play"));
	}
}

void EpisodeControlWidget::_updateDownloadButtonStatus()
{
	DownloadStatus status = _core->episodeCache()->downloadStatus(*_episode);

	switch (status)
	{
	case DownloadStatus::DownloadComplete:
		ui.downloadButton->setEnabled(false);
		ui.downloadButton->setText(tr("Downloaded"));
		break;
	case DownloadStatus::DownloadInProgress:
		ui.downloadButton->setEnabled(false);
		ui.downloadButton->setText(tr("Downloading..."));
		break;
	case DownloadStatus::DownloadInQueue:
		ui.downloadButton->setEnabled(false);
		ui.downloadButton->setText(tr("Download pending"));
		break;
	case DownloadStatus::DownloadNotInQueue:
	{
		ui.downloadButton->setEnabled(true);

		qint64 bytesSoFar = EpisodeCache::getPartialDownloadSize(_episode);
		if (bytesSoFar != -1)
			ui.downloadButton->setText(tr("Resume download"));
		else
			ui.downloadButton->setText(tr("Download"));
	}

	break;
	}
}

void EpisodeControlWidget::_updatePlaylistButtonStatus()
{
	if (_core->defaultPlaylist()->contains(_episode))
		ui.addToPlaylistButton->setText(tr("Remove From Playlist"));
	else
		ui.addToPlaylistButton->setText(tr("Add To Playlist"));
}

void EpisodeControlWidget::on_addToPlaylistButton_clicked()
{
	Playlist* p = _core->defaultPlaylist();

	if (!p->contains(_episode))
	{
		p->add(_episode);
	}
	else
	{
		p->remove(_episode);
	}

	update(_episode);
}

void EpisodeControlWidget::on_downloadButton_clicked()
{
	_core->episodeCache()->enqueueDownload(_episode);

	update(_episode);
}

void EpisodeControlWidget::on_playButton_clicked()
{
	const QList<Episode*>& list = _core->defaultPlaylist()->episodes;
	if (list.size() && list.first() == _episode)
	{
		_core->audioPlayer()->nextEpisode();
	}
	else
	{
		_core->defaultPlaylist()->emplaceFront(_episode);
		_core->audioPlayer()->nextEpisode();
	}

	update(_episode);
}

void EpisodeControlWidget::onDownloadProgressUpdate(const Episode& e, qint64)
{
	if (&e == _episode)
	{
		ui.downloadButton->setText(tr("Downloading..."));
		ui.downloadButton->setEnabled(false);
	}
}

void EpisodeControlWidget::onDownloadFinished(const Episode& e)
{
	if (&e == _episode)
		update(_episode);
}

void EpisodeControlWidget::onEpisodeChanged(const Episode* e)
{
	if(e == _episode)
		update(_episode);
}
