#pragma once

#include <QWidget>
#include "ui_EpisodeListItemWidget.h"

#include <QModelIndex>

#include "core/Core.h"
#include "core/feeds/Feed.h"

#include "ui/models/EpisodeListProxyModel.h"

class EpisodeCache;

class EpisodeListItemWidget : public QWidget
{
	Q_OBJECT

public:
	EpisodeListItemWidget(Core& core, Episode& episode, QWidget *parent = Q_NULLPTR);
	~EpisodeListItemWidget();

	void refresh();

private:
	Ui::EpisodeListItemWidget ui;

	Episode* _episode;
	Core* _core;

private slots:
	void onEpisodeUpdated();
};
