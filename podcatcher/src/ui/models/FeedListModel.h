#pragma once

#include <QAbstractListModel>
#include <QPixmap>
#include <QVector>

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
	mutable QVector<QPixmap*> _thumbnails;

	void _clearThumbnails();

private slots:
	void onFeedListUpdated();
	void onThumbnailDownloaded(QPixmap* px, QString url);
};
