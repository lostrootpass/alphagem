#include "FeedListModel.h"

#include "core/ImageDownloader.h"

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

#include <QPainter>

const QSize pixSize(180, 180);

FeedListModel::FeedListModel(Core& core, QObject *parent)
	: QAbstractListModel(parent), _core(&core)
{
	connect(_core->feedCache(), &FeedCache::feedListUpdated,
		this, &FeedListModel::onFeedListUpdated);

	connect(_core->imageDownloader(), &ImageDownloader::imageDownloaded,
		this, &FeedListModel::onThumbnailDownloaded);
}

FeedListModel::~FeedListModel()
{
	
}

int FeedListModel::rowCount(const QModelIndex&) const
{
	return _core->feedCache()->feeds().size();
}

QVariant FeedListModel::data(const QModelIndex &index, int role) const
{
	const QVector<Feed*>& feeds = _core->feedCache()->feeds();

	if (!index.isValid())
		return QVariant();

	if (index.row() >= feeds.size())
		return QVariant();

	Feed* feed = feeds.at(index.row());
	if (role == Qt::ToolTipRole)
	{
		return feed->title;
	}
	else if (role == Qt::DecorationRole)
	{
		QPixmap p(pixSize);

		ImageDownloader* img = _core->imageDownloader();
		if (feed->imageUrl != "" && img->isCached(feed->imageUrl))
		{
			img->getImage(QUrl(feed->imageUrl), &p);
			if (!p.isNull())
			{
				p = p.scaled(pixSize, Qt::IgnoreAspectRatio,
					Qt::SmoothTransformation);
			}
		}
		else
		{
			if(feed->imageUrl != "")
				img->getImage(QUrl(feed->imageUrl));

			QPainter painter(&p);
			painter.drawText(QRect(0, 0, pixSize.width(), pixSize.height()),
				Qt::AlignCenter, feed->title);
		}

		return p;
	}
	else if (role == FeedDataRole::UnplayedEpCount)
	{
		return feed->countUnplayed();
	}
	
	return QVariant();
}

void FeedListModel::onFeedListUpdated()
{
	beginResetModel();
	endResetModel();
}

void FeedListModel::onThumbnailDownloaded(QPixmap*, QString)
{
	layoutChanged();
}
