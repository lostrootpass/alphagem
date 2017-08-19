#include "FeedListModel.h"

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

FeedListModel::FeedListModel(Core& core, QObject *parent)
	: QAbstractListModel(parent), _core(&core)
{
	connect(_core->feedCache(), &FeedCache::feedListUpdated,
		this, &FeedListModel::onFeedListUpdated);
}

FeedListModel::~FeedListModel()
{
}

int FeedListModel::rowCount(const QModelIndex&) const
{
	return _core->feedCache()->feeds().size();
}

QVariant FeedListModel::data(const QModelIndex &index, int role) const
{
	const QVector<Feed>& feeds = _core->feedCache()->feeds();

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
