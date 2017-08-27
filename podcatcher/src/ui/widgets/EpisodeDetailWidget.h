#pragma once

#include <QWidget>
#include "ui_EpisodeDetailWidget.h"

#include <QVector>

class Core;
struct Episode;

class EpisodeDetailWidget : public QWidget
{
	Q_OBJECT

public:
	EpisodeDetailWidget(QWidget *parent = Q_NULLPTR);
	~EpisodeDetailWidget();

	void init(Core* core);

	void setEpisode(Episode* e);

private:
	Ui::EpisodeDetailWidget ui;
	
	Core* _core;
	Episode* _episode;

	void _setByline();
	void _setMetadata();

private slots:
	void onLinkHovered(const QString& link);
};
