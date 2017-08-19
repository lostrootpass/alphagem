#pragma once

#include <QAbstractListModel>
#include <QListView>

#include "core/Core.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

class EpisodeDetailWidget;

class EpisodeListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	EpisodeListModel(QListView& view, Core& core, int feed, QObject* parent = 0);
	~EpisodeListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	Episode& getEpisode(const QModelIndex& index) const;

	void refreshIndex(const QModelIndex& index);

	void setFeedIndex(int newIndex);

private:
	QListView* _view;
	Core* _core;
	int _feedIndex;

	inline int _epCount() const 
	{
		return _feedIndex == -1 ? 0 : _core->feedCache()->episodes(_feedIndex).size();
	}

	EpisodeDetailWidget* _getWidget(const QModelIndex& index) const;

private slots:
	void markAsPlayed(const QModelIndex& index);

};
