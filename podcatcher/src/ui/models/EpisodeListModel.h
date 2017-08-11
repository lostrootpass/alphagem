#pragma once

#include <QAbstractListModel>

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

class EpisodeListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	EpisodeListModel(FeedCache& cache, int feed, QObject* parent = 0);
	~EpisodeListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	Episode& getEpisode(const QModelIndex& index) const;

	void markAsPlayed(const QModelIndex& index);

private:
	FeedCache* _feedCache;
	int _feedIndex;

	inline int _epCount() const 
	{
		return _feedCache->episodes(_feedIndex).size();
	}
};
