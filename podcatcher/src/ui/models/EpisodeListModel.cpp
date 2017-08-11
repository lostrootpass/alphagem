#include "EpisodeListModel.h"

EpisodeListModel::EpisodeListModel(FeedCache& cache, int feed, QObject* parent) 
	: QAbstractListModel(parent), _feedCache(&cache), _feedIndex(feed)
{
}

EpisodeListModel::~EpisodeListModel()
{
}

int EpisodeListModel::rowCount(const QModelIndex&) const
{
	return _epCount();
}

QVariant EpisodeListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _epCount())
		return QVariant();

	if (role == Qt::DisplayRole)
		return getEpisode(index).title;
	else
		return QVariant();
}

Episode& EpisodeListModel::getEpisode(const QModelIndex& index) const
{
	return _feedCache->episodes(_feedIndex)[_epCount() - 1 - index.row()];
}

void EpisodeListModel::markAsPlayed(const QModelIndex& index)
{
	getEpisode(index).listened = true;

	emit dataChanged(index, index);
}
