#include "EpisodeDetailWidget.h"

#include "core/feeds/Feed.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include <QDateTime>

#include "core/feeds/EpisodeCache.h"

EpisodeDetailWidget::EpisodeDetailWidget(const EpisodeListModel& m, const QModelIndex& idx, QWidget *parent)
	: QWidget(parent), _model(&m), _index(idx)
{
	ui.setupUi(this);

	refresh();
}

EpisodeDetailWidget::~EpisodeDetailWidget()
{
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

	QString formatString = QString(tr("Play (%1:%2)"))
		.arg(e.duration / 60, 2, 10, QChar('0'))
		.arg(e.duration % 60, 2, 10, QChar('0'));

	ui.playButton->setText(formatString);

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

void EpisodeDetailWidget::onDownloadFinished(const EpisodeCache* cache, const Episode& e)
{
	if (e.guid == _model->getEpisode(_index).guid)
	{
		cache->disconnect(this);

		refresh();
	}
}

void EpisodeDetailWidget::_setDownloadButtonStatus(const Episode* e)
{
	const bool downloaded = EpisodeCache::isDownloaded(e);
	ui.downloadButton->setEnabled(!downloaded);

	if (downloaded)
	{
		ui.downloadButton->setText(tr("Downloaded"));
	}
	else
	{
		qint64 bytesSoFar = EpisodeCache::getPartialDownloadSize(e);
		if (bytesSoFar != -1)
		{
			ui.downloadButton->setText(tr("Resume download"));
		}
	}
}

void EpisodeDetailWidget::on_downloadButton_clicked()
{
	emit download(_index);

	ui.downloadButton->setEnabled(false);
	ui.downloadButton->setText(tr("Download pending"));
}

void EpisodeDetailWidget::on_playButton_clicked()
{
	emit play(_index);

	refresh();
}
