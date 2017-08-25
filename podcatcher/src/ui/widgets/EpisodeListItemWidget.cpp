#include "EpisodeListItemWidget.h"

#include <ctime>
#include <iomanip>
#include <sstream>

#include <QDateTime>

#include "core/AudioPlayer.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"

EpisodeListItemWidget::EpisodeListItemWidget(const EpisodeListModel& m, 
	Core& core, const QModelIndex& idx, QWidget *parent)
	: QWidget(parent), _core(&core), _model(&m), _index(idx)
{
	ui.setupUi(this);

	_episode = _model->getEpisode(_index);

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

	struct tm now = { 0 };
	std::time_t t = time(0);
	localtime_s(&now, &t);

	struct tm episode = { 0 };
	localtime_s(&episode, &_episode->published);

	QString format(tr("ddd dd MMM"));
	if (now.tm_year > episode.tm_year)
		format = QString(tr("ddd dd MMM yyyy"));

	QString s = QDateTime::fromTime_t(_episode->published).toString(format);

	ui.dateLabel->setText(s);

	QString listenText("");
	if (!_episode->listened)
		listenText = QString(tr("New"));
	ui.listenStatusLabel->setText(listenText);

	ui.controlWidget->update(_episode);
}

void EpisodeListItemWidget::onEpisodeUpdated()
{
	refresh();
}
