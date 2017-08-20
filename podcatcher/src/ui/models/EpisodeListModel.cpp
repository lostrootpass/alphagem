#include "EpisodeListModel.h"

#include <QList>

#include "core/feeds/EpisodeCache.h"
#include "ui/widgets/EpisodeDetailWidget.h"
#include "ui/windows/MainWindow.h"

EpisodeListModel::EpisodeListModel(QListView& view, Core& core, int feed, QObject* parent) 
	: QAbstractListModel(parent), _view(&view), _core(&core), _feedIndex(feed),
	_listType(EpisodeListType::Feed)
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
	switch (_listType)
	{
	case EpisodeListType::Downloads:
	{
		const QList<DownloadInfo*>& list = _core->episodeCache()->downloadList();
		return *(list.at(index.row())->episode);
	}
		break;

	case EpisodeListType::Feed:
	default:
		return _core->feedCache()->episodes(_feedIndex)[_epCount() - 1 - index.row()];
		break;
	}
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

void EpisodeListModel::refreshList()
{
	beginResetModel();
	endResetModel();
}

void EpisodeListModel::setFeedIndex(int newIndex)
{
	if (_feedIndex == newIndex && _listType == EpisodeListType::Feed)
		return;

	_listType = EpisodeListType::Feed;
	_feedIndex = newIndex;

	refreshList();
}

void EpisodeListModel::showDownloadList()
{
	if (_listType == EpisodeListType::Downloads) return;

	_listType = EpisodeListType::Downloads;

	refreshList();
}

int EpisodeListModel::_epCount() const
{
	switch (_listType)
	{
	case EpisodeListType::Downloads:
	{
		const QList<DownloadInfo*>& list = _core->episodeCache()->downloadList();
		return list.size();
	}
	break;

	case EpisodeListType::Feed:
	default:
		return _feedIndex == -1 ? 
			0 : _core->feedCache()->episodes(_feedIndex).size();
		break;
	}
	
}

EpisodeDetailWidget* EpisodeListModel::_getWidget(const QModelIndex& index) const
{
	EpisodeDetailWidget* e = new EpisodeDetailWidget(*this, *_core, index, nullptr);
	connect(e, &EpisodeDetailWidget::play, this, &EpisodeListModel::markAsPlayed);

	//Item view will take ownership of the widget for us.
	return e;
}
