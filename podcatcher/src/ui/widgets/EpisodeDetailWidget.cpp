#include "EpisodeDetailWidget.h"

#include <QDateTime>
#include <QLabel>

#include "core/Core.h"
#include "core/feeds/Feed.h"

EpisodeDetailWidget::EpisodeDetailWidget(QWidget *parent)
	: QWidget(parent), _core(nullptr), _episode(nullptr)
{
	ui.setupUi(this);
	ui.metadata->setAlignment(Qt::AlignTop);
}

EpisodeDetailWidget::~EpisodeDetailWidget()
{
}

void EpisodeDetailWidget::init(Core* core)
{
	_core = core;

	ui.controlWidget->init(core);
}

void EpisodeDetailWidget::setEpisode(Episode* e)
{
	_episode = e;
	ui.title->setText(e->title);

	if(e->description != "")
		ui.description->setText(e->description);

	ui.newLabel->setVisible(!e->listened);

	_setByline();
	_setMetadata();

	ui.controlWidget->update(e);
}

void EpisodeDetailWidget::_setByline()
{
	struct tm now = { 0 };
	std::time_t t = time(0);
	gmtime_s(&now, &t);

	struct tm episode = { 0 };
	gmtime_s(&episode, &_episode->published);

	QString dateFormat(tr("dddd dd MMM HH:mm"));
	if (now.tm_year > episode.tm_year)
		dateFormat = QString(tr("ddd dd MMM yyyy"));

	QDateTime pub = QDateTime::fromTime_t(_episode->published);
	QString s = pub.toString(dateFormat);

	QString timeAgo("");
	{
		QDateTime cur = QDateTime::fromTime_t(t);
		qint64 secs = pub.secsTo(cur);
		qint64 mins = secs / 60;
		qint64 hours = mins / 60;
		qint64 days = hours / 24;

		if (days > 0)
		{
			if (days > 7)
			{
				timeAgo = QString("%1w %2d ago")
					.arg(days / 7).arg(days % 7);
			}
			else
			{
				hours -= (days * 24);
				timeAgo = QString("%1d %2h ago")
					.arg(days).arg((hours));
			}
		}
		else
		{
			mins -= (hours * 60);
			timeAgo = QString("%1h %2m ago").arg(hours).arg(mins);
		}
	}

	QString format = QString(tr("%1 (%2)")).arg(s).arg(timeAgo);

	QString bylineText("");
	if (_episode->author == "")
	{
		bylineText = QString(tr("Posted on %2"))
			.arg(format);
	}
	else
	{
		bylineText = QString(tr("Posted by %1 on %2"))
			.arg(_episode->author).arg(format);
	}

	ui.byline->setText(bylineText);
}

void EpisodeDetailWidget::_setMetadata()
{
	QString metaFormat("<span style='font-weight: bold'>%1:</span> %2<br />");

	QString metatext("");

	if (_episode->shareLink != "")
	{
		QString linkFormat("<a href='%1'>%1</a>");
		QString link = linkFormat.arg(_episode->shareLink);
		metatext += metaFormat.arg(tr("Link")).arg(link);
	}

	ui.metadata->setText(metatext);
}
