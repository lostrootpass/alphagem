#pragma once

#include <QSlider>

class PlaybackSlider : public QSlider
{
	Q_OBJECT

public:
	PlaybackSlider(QWidget *parent);
	~PlaybackSlider();

protected:
	void paintEvent(QPaintEvent* pe) override;
};
