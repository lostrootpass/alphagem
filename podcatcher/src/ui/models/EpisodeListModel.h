#pragma once

#include <QAbstractListModel>

#include "core/feeds/Feed.h"

class EpisodeListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	EpisodeListModel(QVector<Episode>& episodes, QObject* parent = 0);
	~EpisodeListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	const Episode& getEpisode(const QModelIndex& index) const;

private:
	QVector<Episode> _episodes;
};
