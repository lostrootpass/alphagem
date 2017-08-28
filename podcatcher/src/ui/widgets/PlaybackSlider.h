#pragma once

#include <QSlider>
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


class PlaybackSlider : public QSlider
{
	Q_OBJECT

public:
	PlaybackSlider(QWidget *parent);
	~PlaybackSlider();

protected:
	void paintEvent(QPaintEvent* pe) override;

	void enterEvent(QEvent *event) override;

	void leaveEvent(QEvent *event) override;
};
