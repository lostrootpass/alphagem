#include "EpisodeDetailWidget.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include <QDateTime>

#include "core/AudioPlayer.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"

EpisodeDetailWidget::EpisodeDetailWidget(const EpisodeListModel& m, 
	Core& core, const QModelIndex& idx, QWidget *parent)
	: QWidget(parent), _core(&core), _model(&m), _index(idx)
{
	ui.setupUi(this);

	_episode = &_model->getEpisode(_index);

	refresh();
}

EpisodeDetailWidget::~EpisodeDetailWidget()
{
}

void EpisodeDetailWidget::connectToCache()
{
	EpisodeCache* cache = _core->episodeCache();

	connect(cache, &EpisodeCache::downloadProgressUpdated,
		this, &EpisodeDetailWidget::onDownloadProgressUpdate);

	connect(cache, &EpisodeCache::downloadComplete,
		this, &EpisodeDetailWidget::onDownloadFinished);
}

void EpisodeDetailWidget::refresh()
{
	Episode& e = _model->getEpisode(_index);
	ui.titleLabel->setText(e.title);
	ui.descLabel->setText(e.description);

	struct tm now = { 0 };
	std::time_t t = time(0);
	localtime_s(&now, &t);

	struct tm episode = { 0 };
	localtime_s(&episode, &e.published);

	QString format(tr("ddd dd MMM"));
	if (now.tm_year > episode.tm_year)
		format = QString(tr("ddd dd MMM yyyy"));

	QString s = QDateTime::fromTime_t(e.published).toString(format);

	ui.dateLabel->setText(s);

	QString listenText("");
	if (!e.listened)
		listenText = QString(tr("New"));
	ui.listenStatusLabel->setText(listenText);

	if (e.duration > 0)
	{
		QString formatString = QString(tr("Play (%1:%2)"))
			.arg(e.duration / 60, 2, 10, QChar('0'))
			.arg(e.duration % 60, 2, 10, QChar('0'));

		ui.playButton->setText(formatString);
	}
	else
	{
		ui.playButton->setText(tr("Play"));
	}

	if (_core->defaultPlaylist()->contains(&_model->getEpisode(_index)))
		ui.addToPlaylistButton->setText(tr("Remove From Playlist"));
	else
		ui.addToPlaylistButton->setText(tr("Add To Playlist"));

	_setDownloadButtonStatus(&e);
}

void EpisodeDetailWidget::onDownloadProgressUpdate(const Episode& e, qint64)
{
	if (e.guid == _model->getEpisode(_index).guid)
	{
		ui.downloadButton->setText(tr("Downloading..."));
		ui.downloadButton->setEnabled(false);
	}
}

void EpisodeDetailWidget::onDownloadFinished(const Episode& e)
{
	if (e.guid == _model->getEpisode(_index).guid)
	{
		_core->episodeCache()->disconnect(this);

		refresh();
	}
}

void EpisodeDetailWidget::_setDownloadButtonStatus(const Episode* e)
{
	DownloadStatus status = _core->episodeCache()->downloadStatus(_model->getEpisode(_index));

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

			qint64 bytesSoFar = EpisodeCache::getPartialDownloadSize(e);
			if (bytesSoFar != -1)
				ui.downloadButton->setText(tr("Resume download"));
			else
				ui.downloadButton->setText(tr("Download"));
		}
		
		break;
	}
}

void EpisodeDetailWidget::on_addToPlaylistButton_clicked()
{
	Playlist* p = _core->defaultPlaylist();
	Episode* e = &_model->getEpisode(_index);

	if (!p->contains(e))
	{
		p->add(e);

		connect(_core->audioPlayer(), &AudioPlayer::episodeChanged,
			this, &EpisodeDetailWidget::onEpisodeChanged);
	}
	else
	{
		p->remove(e);

		_core->audioPlayer()->disconnect(this);
	}

	refresh();
}

void EpisodeDetailWidget::on_downloadButton_clicked()
{
	emit download(_index);

	_core->episodeCache()->enqueueDownload(_model->getEpisode(_index));

	refresh();
}

void EpisodeDetailWidget::on_playButton_clicked()
{
	emit play(_index);

	const Episode* ep = &_model->getEpisode(_index);

	const QList<Episode*>& list = _core->defaultPlaylist()->episodes;
	if (list.size() && list.first() == ep)
		_core->audioPlayer()->nextEpisode();
	else
		_core->audioPlayer()->playEpisode(ep);

	refresh();
}

void EpisodeDetailWidget::onEpisodeChanged(const Episode* e)
{
	if (e->guid == _episode->guid)
	{
		_core->audioPlayer()->disconnect(this);
		refresh();
	}
}
