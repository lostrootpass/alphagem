#pragma once

#include <QLabel>

class FeedIconWidget : public QLabel
{
	Q_OBJECT

public:
	FeedIconWidget(QWidget *parent);
	~FeedIconWidget();

	void resetDefault();

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent *ev);
};
