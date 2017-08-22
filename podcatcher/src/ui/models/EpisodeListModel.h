#pragma once

#include <QAbstractListModel>
#include <QListView>

#include "core/Core.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

class EpisodeDetailWidget;

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
	EpisodeListModel(QListView& view, Core& core, int feed, QObject* parent = 0);
	~EpisodeListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	Episode* getEpisode(const QModelIndex& index) const;

	void refreshIndex(const QModelIndex& index);

	void refreshList();

	void setFeedIndex(int newIndex);

	void showDownloadList();
	
	void showPlaylist();

private:
	QListView* _view;
	Core* _core;
	int _feedIndex;
	EpisodeListType _listType;

	int _epCount() const;

	EpisodeDetailWidget* _getWidget(const QModelIndex& index) const;

private slots:
	void markAsPlayed(const QModelIndex& index);
	void onPlaylistChanged();

};
