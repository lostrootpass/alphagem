#pragma once

#include <QAbstractListModel>

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

class FeedListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	FeedListModel(FeedCache& cache, QObject *parent);
	~FeedListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

private:
	FeedCache* _feedCache;

private slots:
	void onFeedListUpdated();
};
