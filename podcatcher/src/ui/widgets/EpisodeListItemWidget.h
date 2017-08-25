#pragma once

#include <QWidget>
#include "ui_EpisodeListItemWidget.h"

#include <QModelIndex>

#include "core/Core.h"
#include "core/feeds/Feed.h"

#include "ui/models/EpisodeListModel.h"

class EpisodeCache;

class EpisodeListItemWidget : public QWidget
{
	Q_OBJECT

public:
	EpisodeListItemWidget(const EpisodeListModel& m, Core& core, const QModelIndex& idx, QWidget *parent = Q_NULLPTR);
	~EpisodeListItemWidget();

	void refresh();

private:
	Ui::EpisodeListItemWidget ui;
	QModelIndex _index;

	const EpisodeListModel* _model;
	Episode* _episode;
	Core* _core;

private slots:
	void onEpisodeUpdated();
};
