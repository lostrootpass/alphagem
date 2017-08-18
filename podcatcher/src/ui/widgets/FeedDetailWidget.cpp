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

	connect(_feedCache, &FeedCache::feedListUpdated,
		this, &FeedDetailWidget::onFeedListUpdated);
}

void FeedDetailWidget::setImageDownloader(ImageDownloader& downloader)
{
	_imageDownloader = &downloader;
}

void FeedDetailWidget::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed>& feeds = _feedCache->feeds();

	if (!feeds.size()) return;

	_feedIndex = index.row();

	Feed* feed = &feeds[_feedIndex];

	ui.titleLabel->setText(feed->title);
	ui.descriptionLabel->setText(feed->description);
	ui.feedIcon->clear();

	if (_imageDownloader)
		_imageDownloader->setImage(QUrl(feed->imageUrl), *ui.feedIcon);
}

void FeedDetailWidget::on_refreshButton_clicked()
{
	ui.refreshButton->setEnabled(false);
	ui.refreshButton->setText(tr("Refreshing..."));

	_feedCache->refresh(_feedIndex);
}

void FeedDetailWidget::onFeedListUpdated()
{
	ui.refreshButton->setEnabled(true);
	ui.refreshButton->setText(tr("Refresh"));
}
