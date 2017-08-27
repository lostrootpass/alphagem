#include "FeedDetailWidget.h"

#include "core/Core.h"
#include "core/ImageDownloader.h"
#include "core/feeds/FeedCache.h"

#include "ui/widgets/FeedIconWidget.h"

#include "ui/windows/FeedSettingsWindow.h"

FeedDetailWidget::FeedDetailWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.refreshButton->setVisible(false);

	connect(ui.feedIcon, &FeedIconWidget::clicked,
		this, &FeedDetailWidget::onIconClicked);
}

FeedDetailWidget::~FeedDetailWidget()
{
}

void FeedDetailWidget::setCore(Core* core)
{
	_core = core;
	connect(_core->feedCache(), &FeedCache::feedListUpdated,
		this, &FeedDetailWidget::onFeedListUpdated);

	connect(_core->feedCache(), &FeedCache::refreshStarted,
		this, &FeedDetailWidget::onRefreshStarted);
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
	_core->feedCache()->refresh(_feed);
}

void FeedDetailWidget::on_settingsButton_clicked()
{
	FeedSettingsWindow* fsw = new FeedSettingsWindow(_core, _feed, nullptr);
	fsw->setAttribute(Qt::WA_DeleteOnClose);
	fsw->setWindowModality(Qt::ApplicationModal);
	fsw->setWindowTitle(tr("Feed Settings"));
	fsw->show();
}

void FeedDetailWidget::onIconClicked()
{
	emit iconClicked(_feed);
}

void FeedDetailWidget::onRefreshStarted(Feed*)
{
	ui.refreshButton->setEnabled(false);
	ui.refreshButton->setText(tr("Refreshing..."));
}

void FeedDetailWidget::onFeedListUpdated()
{
	ui.refreshButton->setEnabled(true);
	ui.refreshButton->setText(tr("Refresh"));
}
