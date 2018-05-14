#pragma once

#include <QAbstractListModel>

#include "core/Core.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

class EpisodeListItemWidget;

enum class EpisodeListType
{
	Downloads,
	Feed,
	Playlist
};

class EpisodeListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	EpisodeListModel(Core& core, int feed, QObject* parent = 0);
	~EpisodeListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	Episode* getEpisode(const QModelIndex& index) const;
	Episode* getEpisode(int row) const;

	inline EpisodeListType listType() const
	{
		return _listType;
	}

	void refreshList();

	void setFeedIndex(int newIndex);

	void showDownloadList();
	
	void showPlaylist();

private:
	Core* _core;
	Feed* _feed;
	int _feedIndex;
	EpisodeListType _listType;

	int _epCount() const;

private slots:
	void onDownloadQueueUpdated();
	void onFeedSettingsChanged(Feed* f);
	void onFeedUpdated();
	void onPlaylistChanged();

};
