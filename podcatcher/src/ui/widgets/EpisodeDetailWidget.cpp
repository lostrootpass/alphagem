#include "EpisodeDetailWidget.h"

#include <QDateTime>
#include <QLabel>
#include <QToolTip>

#include "core/Core.h"
#include "core/TimeUtil.h"
#include "core/feeds/Feed.h"

EpisodeDetailWidget::EpisodeDetailWidget(QWidget *parent)
	: QWidget(parent), _core(nullptr), _episode(nullptr)
{
	ui.setupUi(this);
	ui.metadata->setAlignment(Qt::AlignTop);

	connect(ui.description, &QLabel::linkHovered,
		this, &EpisodeDetailWidget::onLinkHovered);
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
	if (_episode)
	{
		_episode->disconnect(this);
	}

	_episode = e;
	connect(_episode, &Episode::updated,
		this, &EpisodeDetailWidget::episodeUpdated);

	_refresh();
}

void EpisodeDetailWidget::_refresh()
{
	ui.title->setText(_episode->title);

	if (_episode->description != "")
		ui.description->setText(_episode->description);
	else
	{
		QString default =
			tr("<span style='font-style: italic'>No description.</span>");
		ui.description->setText(default);
	}

	ui.newLabel->setVisible(!_episode->listened);

	_setByline();
	_setMetadata();

	ui.controlWidget->update(_episode);
}

void EpisodeDetailWidget::_setByline()
{
	QDateTime dt = parseDateTime(_episode->published);
	QDateTime now = QDateTime::currentDateTimeUtc();
	QString timeAgo = relativeTimeBetween(now, dt);

	QString timestamp = commonTimestamp(_episode->published);
	timestamp = QString(tr("%1 (%2)")).arg(timestamp).arg(timeAgo);

	QString bylineText("");
	if (_episode->author == "")
	{
		bylineText = QString(tr("Posted on %2"))
			.arg(timestamp);
	}
	else
	{
		bylineText = QString(tr("Posted by %1 on %2"))
			.arg(_episode->author).arg(timestamp);
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

	metatext += metaFormat
		.arg(tr("Published")).arg(_episode->published);

	ui.metadata->setText(metatext);
}

void EpisodeDetailWidget::episodeUpdated()
{
	_refresh();
}

void EpisodeDetailWidget::onLinkHovered(const QString& link)
{
	QToolTip::showText(QCursor::pos(), link);
}
