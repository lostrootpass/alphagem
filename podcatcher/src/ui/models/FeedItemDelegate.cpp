#include "FeedItemDelegate.h"

#include <QPainter>

FeedItemDelegate::FeedItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

FeedItemDelegate::~FeedItemDelegate()
{
}

void FeedItemDelegate::paint(QPainter *painter, 
	const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QVariant v = index.model()->data(index, Qt::DecorationRole);
	
	if (option.state & QStyle::State_MouseOver)
	{
		painter->setOpacity(1.0f);
	}
	else
	{
		painter->setOpacity(0.85f);
	}

	painter->drawPixmap(option.rect, v.value<QPixmap>());
}

QSize FeedItemDelegate::sizeHint(const QStyleOptionViewItem &option, 
	const QModelIndex &index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}
