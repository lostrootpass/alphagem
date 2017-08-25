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

void FeedDetailWidget::setFeed(Feed* f)
{
	if (_feed == f)
		return;

	if (f == nullptr)
	{
		_feed = nullptr;
		setVisible(false);
		return;
	}
	
	setVisible(true);
	_feed = f;

	ui.titleLabel->setText(_feed->title);
	ui.descriptionLabel->setText(_feed->description);
	ui.feedIcon->clear();
	ui.refreshButton->setVisible(true);

	_core->imageDownloader()->getImage(QUrl(_feed->imageUrl), ui.feedIcon);
}

void FeedDetailWidget::onFeedSelected(const QModelIndex& index)
{
	QVector<Feed*>& feeds = _core->feedCache()->feeds();

	if (!feeds.size()) return;

	setFeed(feeds[index.row()]);
}

void FeedDetailWidget::on_refreshButton_clicked()
{
	ui.refreshButton->setEnabled(false);
	ui.refreshButton->setText(tr("Refreshing..."));

	_core->feedCache()->refresh(_feed);
}

void FeedDetailWidget::onFeedListUpdated()
{
	ui.refreshButton->setEnabled(true);
	ui.refreshButton->setText(tr("Refresh"));
}
