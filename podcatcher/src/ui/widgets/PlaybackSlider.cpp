#include "PlaybackSlider.h"

#include <QPainter>

PlaybackSlider::PlaybackSlider(QWidget *parent)
	: QSlider(parent)
{
	setStyle(new PlaybackSliderStyle(style()));
}

PlaybackSlider::~PlaybackSlider()
{
}

void PlaybackSlider::paintEvent(QPaintEvent* pe)
{
	QSlider::paintEvent(pe);
}

void PlaybackSlider::enterEvent(QEvent *event)
{
	QSlider::enterEvent(event);
}

void PlaybackSlider::leaveEvent(QEvent *event)
{
	QSlider::leaveEvent(event);
}
