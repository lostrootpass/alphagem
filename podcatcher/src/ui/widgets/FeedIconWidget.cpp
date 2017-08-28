#include "FeedIconWidget.h"

#include <QIcon>

FeedIconWidget::FeedIconWidget(QWidget *parent)
	: QLabel(parent)
{

}

FeedIconWidget::~FeedIconWidget()
{
}

void FeedIconWidget::resetDefault()
{
	QIcon placeholder = QIcon::fromTheme("media-album-cover");
	setPixmap(placeholder.pixmap(sizeHint()));
}

void FeedIconWidget::mousePressEvent(QMouseEvent*)
{
	emit clicked();
}
