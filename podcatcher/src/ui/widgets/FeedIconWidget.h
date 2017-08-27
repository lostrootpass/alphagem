#pragma once

#include <QLabel>

class FeedIconWidget : public QLabel
{
	Q_OBJECT

public:
	FeedIconWidget(QWidget *parent);
	~FeedIconWidget();

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent *ev);
};
