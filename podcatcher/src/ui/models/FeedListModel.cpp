#include "FeedListModel.h"

#include "core/ImageDownloader.h"

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

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
	_clearThumbnails();
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

	if (index.row() >= feeds.size() || index.row() >= _thumbnails.size())
		return QVariant();

	Feed* feed = feeds.at(index.row());
	if (role == Qt::ToolTipRole)
	{
		return feed->title;
	}
	else if (role == Qt::DecorationRole)
	{
		QPixmap*& p = _thumbnails[index.row()];
		
		if (p)
		{
			return *p;
		}

		p = new QPixmap(pixSize);

		ImageDownloader* img = _core->imageDownloader();
		if (img->isCached(feed->imageUrl))
		{
			img->loadPixmap(QUrl(feed->imageUrl), p);
			*p = p->scaled(pixSize, Qt::KeepAspectRatio,
				Qt::SmoothTransformation);
		}
		else
		{
			img->getImage(QUrl(feed->imageUrl));
		}

		return *p;
	}
	else if (role == FeedDataRole::UnplayedEpCount)
	{
		return feed->countUnplayed();
	}
	
	return QVariant();
}

void FeedListModel::_clearThumbnails()
{
	for (QPixmap* p : _thumbnails)
	{
		delete p;
	}
	_thumbnails.clear();
}

void FeedListModel::onFeedListUpdated()
{
	_clearThumbnails();
	_thumbnails.resize(_core->feedCache()->feeds().size());
	
	beginResetModel();
	endResetModel();
}

void FeedListModel::onThumbnailDownloaded(QPixmap* px, QString url)
{
	const QVector<Feed*>& feeds = _core->feedCache()->feeds();
	for (int i = 0; i < feeds.size(); ++i)
	{
		if (feeds[i]->imageUrl == url)
		{
			*_thumbnails[i] = px->scaled(pixSize, Qt::KeepAspectRatio,
				Qt::SmoothTransformation);

			break;
		}
	}

	beginResetModel();
	endResetModel();
}
