#include "EpisodeListModel.h"

#include <QList>

#include "core/AudioPlayer.h"
#include "core/feeds/EpisodeCache.h"
#include "ui/widgets/EpisodeListItemWidget.h"
#include "ui/windows/MainWindow.h"

EpisodeListModel::EpisodeListModel(QListView& view, Core& core, int feed, QObject* parent) 
	: QAbstractListModel(parent), _view(&view), _core(&core), _feedIndex(feed),
	_listType(EpisodeListType::Feed)
{
	connect(_core->defaultPlaylist(), &Playlist::playlistUpdated,
		this, &EpisodeListModel::onPlaylistChanged);

	connect(_core->feedCache(), &FeedCache::feedListUpdated,
		this, &EpisodeListModel::onFeedUpdated);

	connect(_core->episodeCache(), &EpisodeCache::downloadQueueUpdated,
		this, &EpisodeListModel::onDownloadQueueUpdated);
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

Episode* EpisodeListModel::getEpisode(const QModelIndex& index) const
{
	switch (_listType)
	{
	case EpisodeListType::Downloads:
	{
		const QList<DownloadInfo*>& list = _core->episodeCache()->downloadList();
		return (list.at(index.row())->episode);
	}
		break;

	case EpisodeListType::Playlist:
	{
		Playlist* p = _core->defaultPlaylist();
		if (p->episodes.size() > index.row())
			return p->episodes.at(index.row());
		return nullptr;
	}
		break;

	case EpisodeListType::Feed:
	default:
		return _core->feedCache()->episodes(_feedIndex)[_epCount() - 1 - index.row()];
		break;
	}
}

void EpisodeListModel::onDownloadQueueUpdated()
{
	if (_listType == EpisodeListType::Downloads)
		refreshList();
}

void EpisodeListModel::onFeedUpdated()
{
	refreshList();
}

void EpisodeListModel::onPlaylistChanged()
{
	if(_listType == EpisodeListType::Playlist)
		refreshList();
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

void EpisodeListModel::showPlaylist()
{
	if (_listType == EpisodeListType::Playlist) return;

	_listType = EpisodeListType::Playlist;

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

	case EpisodeListType::Playlist:
		return _core->defaultPlaylist()->episodes.size();
		break;

	case EpisodeListType::Feed:
	default:
		return _feedIndex == -1 ? 
			0 : _core->feedCache()->episodes(_feedIndex).size();
		break;
	}
	
}

EpisodeListItemWidget* EpisodeListModel::_getWidget(const QModelIndex& index) const
{
	//Item view will take ownership of the widget for us.
	return new EpisodeListItemWidget(*this, *_core, index, nullptr);
}
