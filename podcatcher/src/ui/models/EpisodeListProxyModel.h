#pragma once

#include <QSortFilterProxyModel>
#include <QListView>

#include "../../core/Core.h"

class EpisodeListItemWidget;
struct Episode;

class EpisodeListProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	EpisodeListProxyModel(Core& core, QListView& view, QObject *parent);
	~EpisodeListProxyModel();

	bool filterNewOnly() const { return _filterNewOnly; }
	void setFilterNewOnly(bool filter) { _filterNewOnly = filter; }

	bool filterDownloadedOnly() const { return _filterDownloadedOnly; }
	void setFilterDownloadedOnly(bool filter) { _filterDownloadedOnly = filter; }

	Episode* getEpisode(const QModelIndex& idx) const;

	void refreshIndex(const QModelIndex& idx);

protected:

	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
	virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
	EpisodeListItemWidget* _getWidget(Episode* ep) const;

	Core* _core;
	QListView* _view;

	bool _filterNewOnly;
	bool _filterDownloadedOnly;
};
