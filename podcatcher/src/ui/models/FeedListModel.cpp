#include "FeedListModel.h"

FeedListModel::FeedListModel(FeedCache& cache, QObject *parent)
	: QAbstractListModel(parent), _feedCache(&cache)
{
	connect(_feedCache, &FeedCache::feedListUpdated,
		this, &FeedListModel::onFeedListUpdated);
}

FeedListModel::~FeedListModel()
{
}

int FeedListModel::rowCount(const QModelIndex&) const
{
	return _feedCache->feeds().size();
}

QVariant FeedListModel::data(const QModelIndex &index, int role) const
{
	const QVector<Feed>& feeds = _feedCache->feeds();

	if (!index.isValid())
		return QVariant();

	if (index.row() >= feeds.size())
		return QVariant();

	if (role == Qt::DisplayRole)
		return feeds.at(index.row()).title;
	else
		return QVariant();
}

void FeedListModel::onFeedListUpdated()
{
	emit layoutChanged();
}
