#include "FeedIconWidget.h"

FeedIconWidget::FeedIconWidget(QWidget *parent)
	: QLabel(parent)
{
}

FeedIconWidget::~FeedIconWidget()
{
}

void FeedIconWidget::mousePressEvent(QMouseEvent*)
{
	emit clicked();
}
