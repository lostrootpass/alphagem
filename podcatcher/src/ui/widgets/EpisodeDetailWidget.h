#pragma once

#include <QWidget>
#include "ui_EpisodeDetailWidget.h"

#include <QPixmap>
#include <QVector>

class Core;
struct Episode;
class TagLayout;

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
	QPixmap _background;
	qint64 _backgroundRetrievedTimestamp;
	float _backgroundOpacity;

	Core* _core;
	Episode* _episode;
	TagLayout* _tagLayout;

	void _initBackground();
	void _refresh();
	void _setByline();
	void _setMetadata();

private slots:
	void episodeUpdated();
	void onImageDownloaded(QPixmap* px, QString url);
	void onLinkHovered(const QString& link);
protected:
	void paintEvent(QPaintEvent *event) override;

	void resizeEvent(QResizeEvent *event) override;
};
