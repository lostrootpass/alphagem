#include "EpisodeListProxyModel.h"

#include "EpisodeListModel.h"

#include "../widgets/EpisodeListItemWidget.h"

#include "../../core/feeds/EpisodeCache.h"

EpisodeListProxyModel::EpisodeListProxyModel(Core& core, QListView& view, QObject *parent)
	: QSortFilterProxyModel(parent), _filterDownloadedOnly(false),
	_filterNewOnly(false), _core(&core), _view(&view)
{
	setFilterCaseSensitivity(Qt::CaseInsensitive);
}

EpisodeListProxyModel::~EpisodeListProxyModel()
{
}

Episode* EpisodeListProxyModel::getEpisode(const QModelIndex& idx) const
{
	const QModelIndex sourceIdx = mapToSource(idx);
	if (sourceIdx.isValid())
	{
		EpisodeListModel* source = qobject_cast<EpisodeListModel*>(sourceModel());
		return source->getEpisode(sourceIdx);
	}

	return nullptr;
}

void EpisodeListProxyModel::refreshIndex(const QModelIndex& idx)
{
	EpisodeListModel* source = qobject_cast<EpisodeListModel*>(sourceModel());

	if (!_view->indexWidget(idx))
	{
		const QModelIndex sourceIdx = mapToSource(idx);
		if(sourceIdx.isValid())
			_view->setIndexWidget(idx, _getWidget(source->getEpisode(sourceIdx)));
	}
}

bool EpisodeListProxyModel::filterAcceptsRow(int source_row, const QModelIndex &) const
{
	EpisodeListModel* source = qobject_cast<EpisodeListModel*>(sourceModel());
	Episode* ep = source->getEpisode(source_row);

	if (_filterNewOnly && ep->listened)
		return false;

	if (_filterDownloadedOnly && !_core->episodeCache()->isDownloaded(ep))
		return false;

	if (!filterRegExp().isEmpty())
	{
		if (filterRegExp().indexIn(ep->title) == -1 &&
			filterRegExp().indexIn(ep->description) == -1)
		{
			return false;
		}
	}

	return true;
}

bool EpisodeListProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
	return (source_left.row() <= source_right.row());
}

EpisodeListItemWidget* EpisodeListProxyModel::_getWidget(Episode* ep) const
{
	//Item view will take ownership of the widget for us.
	return new EpisodeListItemWidget(*_core, *ep, nullptr);
}
