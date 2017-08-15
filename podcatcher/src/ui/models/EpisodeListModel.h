#pragma once

#include <QAbstractListModel>
#include <QListView>

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

class EpisodeDetailWidget;

class EpisodeListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	EpisodeListModel(QListView& view, FeedCache& cache, int feed, QObject* parent = 0);
	~EpisodeListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	Episode& getEpisode(const QModelIndex& index) const;

	void markAsPlayed(const QModelIndex& index);

	void refreshIndex(const QModelIndex& index);

	void setFeedIndex(int newIndex);

private:
	QListView* _view;
	FeedCache* _feedCache;
	int _feedIndex;

	inline int _epCount() const 
	{
		return _feedIndex == -1 ? 0 : _feedCache->episodes(_feedIndex).size();
	}

	EpisodeDetailWidget* _getWidget(const QModelIndex& index) const;
};
