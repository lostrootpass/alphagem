#pragma once

#include <QWidget>
#include "ui_EpisodeDetailWidget.h"

#include <QModelIndex>

#include "ui/models/EpisodeListModel.h"

class EpisodeDetailWidget : public QWidget
{
	Q_OBJECT

public:
	EpisodeDetailWidget(const EpisodeListModel& m, const QModelIndex& idx, QWidget *parent = Q_NULLPTR);
	~EpisodeDetailWidget();

	void refresh();

signals:
	void play(const QModelIndex& index);

private:
	Ui::EpisodeDetailWidget ui;

	QModelIndex _index;
	const EpisodeListModel* _model;

private slots:
	void on_playButton_clicked();
};
