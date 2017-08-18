#include "PlaybackSlider.h"

#include <QPainter>
#include <QProxyStyle>
#include <QStyleOption>

class PlaybackSliderStyle : public QProxyStyle
{
public:
	PlaybackSliderStyle(QStyle* s) : QProxyStyle(s) 
	{
	};

	int styleHint(QStyle::StyleHint hint, const QStyleOption* option = 0, const QWidget* widget = 0, QStyleHintReturn* returnData = 0) const
	{
		if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
			return (Qt::LeftButton | Qt::MidButton | Qt::RightButton);
		return QProxyStyle::styleHint(hint, option, widget, returnData);
	}
};

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
