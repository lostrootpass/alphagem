#include "FeedDetailWidget.h"

FeedDetailWidget::FeedDetailWidget(QWidget *parent)
	: QWidget(parent), _feedCache(nullptr)
{
	ui.setupUi(this);
}

FeedDetailWidget::~FeedDetailWidget()
{
}

void FeedDetailWidget::setFeedCache(FeedCache& cache)
{
	_feedCache = &cache;
}

void FeedDetailWidget::setImageDownloader(ImageDownloader& downloader)
{
	_imageDownloader = &downloader;
}

void FeedDetailWidget::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed>& feeds = _feedCache->feeds();

	if (!feeds.size()) return;

	const int feedIndex = index.row();

	Feed* feed = &feeds[feedIndex];

	ui.titleLabel->setText(feed->title);
	ui.descriptionLabel->setText(feed->description);
	ui.feedIcon->clear();

	if (_imageDownloader)
		_imageDownloader->setImage(QUrl(feed->imageUrl), *ui.feedIcon);
}
