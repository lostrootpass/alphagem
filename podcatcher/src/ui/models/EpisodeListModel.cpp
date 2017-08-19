#include "EpisodeListModel.h"

#include "ui/widgets/EpisodeDetailWidget.h"
#include "MainWindow.h"

EpisodeListModel::EpisodeListModel(QListView& view, Core& core, int feed, QObject* parent) 
	: QAbstractListModel(parent), _view(&view), _core(&core), _feedIndex(feed)
{
}

EpisodeListModel::~EpisodeListModel()
{
}

int EpisodeListModel::rowCount(const QModelIndex&) const
{
	return _epCount();
}

QVariant EpisodeListModel::data(const QModelIndex &, int ) const
{
	return QVariant();
}

Episode& EpisodeListModel::getEpisode(const QModelIndex& index) const
{
	return _core->feedCache()->episodes(_feedIndex)[_epCount() - 1 - index.row()];
}

void EpisodeListModel::markAsPlayed(const QModelIndex& index)
{
	getEpisode(index).listened = true;

	emit dataChanged(index, index);
}

void EpisodeListModel::refreshIndex(const QModelIndex& index)
{
	if(!_view->indexWidget(index))
		_view->setIndexWidget(index, _getWidget(index));
}

void EpisodeListModel::setFeedIndex(int newIndex)
{
	if (_feedIndex == newIndex)
		return;

	_feedIndex = newIndex;

	beginResetModel();
	endResetModel();
}

EpisodeDetailWidget* EpisodeListModel::_getWidget(const QModelIndex& index) const
{
	EpisodeDetailWidget* e = new EpisodeDetailWidget(*this, *_core, index, nullptr);
	connect(e, &EpisodeDetailWidget::play, this, &EpisodeListModel::markAsPlayed);

	//Item view will take ownership of the widget for us.
	return e;
}
