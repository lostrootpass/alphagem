#pragma once

#include <QAbstractListModel>

#include "core/Core.h"

class FeedListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	FeedListModel(Core& core, QObject *parent);
	~FeedListModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;

private:
	Core* _core;

private slots:
	void onFeedListUpdated();
};
