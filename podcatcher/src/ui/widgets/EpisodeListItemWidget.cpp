#include "EpisodeListItemWidget.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include <QDateTime>

#include "core/AudioPlayer.h"
#include "core/TimeUtil.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"

EpisodeListItemWidget::EpisodeListItemWidget(Core& core, Episode& episode, 
	QWidget *parent)
	: QWidget(parent), _core(&core), _episode(&episode)
{
	ui.setupUi(this);

	connect(_episode, &Episode::updated, 
		this, &EpisodeListItemWidget::onEpisodeUpdated);

	ui.controlWidget->init(&core);

	refresh();
}

EpisodeListItemWidget::~EpisodeListItemWidget()
{
}

void EpisodeListItemWidget::refresh()
{
	ui.titleLabel->setText(_episode->title);

	if (_episode->description != "")
		ui.descLabel->setText(_episode->description);
	else
	{
		QString default = 
			tr("<span style='font-style: italic'>No description.</span>");
		ui.descLabel->setText(default);
	}

	ui.dateLabel->setText(commonTimestamp(_episode->published));
	ui.listenStatusLabel->setVisible(!_episode->listened);

	ui.controlWidget->update(_episode);
}

void EpisodeListItemWidget::onEpisodeUpdated()
{
	refresh();
}
