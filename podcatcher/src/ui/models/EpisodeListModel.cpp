#include "EpisodeListModel.h"

EpisodeListModel::EpisodeListModel(QVector<Episode>& episodes, QObject* parent) 
	: QAbstractListModel(parent), _episodes(episodes)
{
}

EpisodeListModel::~EpisodeListModel()
{
}

int EpisodeListModel::rowCount(const QModelIndex &parent) const
{
	return _episodes.size();
}

QVariant EpisodeListModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= _episodes.size())
		return QVariant();

	if (role == Qt::DisplayRole)
		return _episodes.at(index.row()).title;
	else
		return QVariant();
}

const Episode& EpisodeListModel::getEpisode(const QModelIndex& index) const
{
	return _episodes.at(index.row());
}
