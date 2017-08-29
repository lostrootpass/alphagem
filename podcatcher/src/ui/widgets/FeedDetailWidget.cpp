#include "FeedDetailWidget.h"

#include "core/Core.h"
#include "core/ImageDownloader.h"
#include "core/feeds/FeedCache.h"

#include "ui/widgets/FeedIconWidget.h"

#include "ui/windows/FeedSettingsWindow.h"
#include "TagLayout.h"

FeedDetailWidget::FeedDetailWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.refreshButton->setVisible(false);

	connect(ui.feedIcon, &FeedIconWidget::clicked,
		this, &FeedDetailWidget::onIconClicked);

	ui.feedIcon->resetDefault();

	_tagLayout = new TagLayout(nullptr);
	ui.verticalLayout->addLayout(_tagLayout);
}

FeedDetailWidget::~FeedDetailWidget()
{
	delete _tagLayout;
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
	ui.descriptionLabel->updateGeometry();
	ui.descriptionLabel->adjustSize();
	QSize s = ui.descriptionLabel->sizeHint();
	s = ui.descriptionLabel->size();
	s = ui.descriptionLabel->minimumSizeHint();
	s = ui.descriptionLabel->maximumSize();
	ui.feedIcon->resetDefault();
	ui.refreshButton->setVisible(true);

	_core->imageDownloader()->getImage(QUrl(_feed->imageUrl), ui.feedIcon);

	_tagLayout->clearTags();
	_tagLayout->setTags(f->categories);
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
