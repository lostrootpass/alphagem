#include "EpisodeListModel.h"

#include <QList>

#include "core/AudioPlayer.h"
#include "core/Core.h"
#include "core/Settings.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/FeedSettings.h"
#include "ui/widgets/EpisodeListItemWidget.h"
#include "ui/windows/MainWindow.h"

EpisodeListModel::EpisodeListModel(Core& core, int feed, QObject* parent) 
	: QAbstractListModel(parent), _core(&core), _feedIndex(feed),
	_listType(EpisodeListType::Feed)
{
	if(_feedIndex > -1)
		_feed = _core->feedCache()->feeds()[_feedIndex];

	connect(_core->defaultPlaylist(), &Playlist::playlistUpdated,
		this, &EpisodeListModel::onPlaylistChanged);

	connect(_core->feedCache(), &FeedCache::feedListUpdated,
		this, &EpisodeListModel::onFeedUpdated);

	connect(_core->episodeCache(), &EpisodeCache::downloadQueueUpdated,
		this, &EpisodeListModel::onDownloadQueueUpdated);

	connect(_core->settings(), &Settings::feedSettingsChanged,
		this, &EpisodeListModel::onFeedSettingsChanged);
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
	return getEpisode(index.row());
}

Episode* EpisodeListModel::getEpisode(int row) const
{
	switch (_listType)
	{
	case EpisodeListType::Downloads:
	{
		const QList<DownloadInfo*>& list = _core->episodeCache()->downloadList();
		return (list.at(row)->episode);
	}
		break;

	case EpisodeListType::Playlist:
	{
		Playlist* p = _core->defaultPlaylist();
		if (p->episodes.size() > row)
			return p->episodes.at(row);
		return nullptr;
	}
		break;

	case EpisodeListType::Feed:
	default:
	{
		const FeedSettings& settings = _core->settings()->feed(_feed);
		if (settings.episodeOrder == EpisodeOrder::NewestFirst)
			return _feed->episodes[_epCount() - 1 - row];
		else
			return _feed->episodes[row];
		break;
	}
	}
}

void EpisodeListModel::onDownloadQueueUpdated()
{
	if (_listType == EpisodeListType::Downloads)
		refreshList();
}

void EpisodeListModel::onFeedSettingsChanged(Feed* f)
{
	if(f == _feed || f == nullptr)
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
	_feed = _core->feedCache()->feeds()[_feedIndex];

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
			0 : _feed->episodes.size();
		break;
	}
	
}
