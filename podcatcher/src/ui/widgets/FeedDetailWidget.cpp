#include "FeedDetailWidget.h"

#include "core/Core.h"
#include "core/ImageDownloader.h"
#include "core/feeds/FeedCache.h"

FeedDetailWidget::FeedDetailWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.refreshButton->setVisible(false);
}

FeedDetailWidget::~FeedDetailWidget()
{
}

void FeedDetailWidget::setCore(Core* core)
{
	_core = core;
	connect(_core->feedCache(), &FeedCache::feedListUpdated,
		this, &FeedDetailWidget::onFeedListUpdated);
}

void FeedDetailWidget::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed*>& feeds = _core->feedCache()->feeds();

	if (!feeds.size()) return;

	_feedIndex = index.row();

	Feed* feed = feeds[_feedIndex];

	ui.titleLabel->setText(feed->title);
	ui.descriptionLabel->setText(feed->description);
	ui.feedIcon->clear();
	ui.refreshButton->setVisible(true);

	_core->imageDownloader()->setImage(QUrl(feed->imageUrl), *ui.feedIcon);
}

void FeedDetailWidget::on_refreshButton_clicked()
{
	ui.refreshButton->setEnabled(false);
	ui.refreshButton->setText(tr("Refreshing..."));

	_core->feedCache()->refresh(_feedIndex);
}

void FeedDetailWidget::onFeedListUpdated()
{
	ui.refreshButton->setEnabled(true);
	ui.refreshButton->setText(tr("Refresh"));
}
