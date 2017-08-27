#include "FeedItemDelegate.h"

#include <QPainter>

#include "FeedListModel.h"

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
	painter->save();

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

	painter->setOpacity(1.0f);

	QVariant count = index.model()->data(index, FeedDataRole::UnplayedEpCount);
	if (count.value<int>() > 0)
	{
		QString strcount = QString("%1").arg(count.value<int>());

		QFont font = painter->font();
		font.setBold(true);
		font.setPointSize(12);
		painter->setFont(font);

		QRect bound = painter->fontMetrics().boundingRect(strcount);
		bound.adjust(0, 0, 9.5f, 4.5f);
		bound.moveTopRight(option.rect.topRight() - QPoint(10, -10));

		QBrush b(QColor(200, 0, 0, 255));
		QPainterPath path;
		path.addRoundedRect(bound, 5, 5);
		painter->setPen(Qt::NoPen);
		painter->fillPath(path, b);
		painter->drawPath(path);

		painter->setPen(QPen(Qt::white));
		painter->drawText(bound, Qt::AlignCenter, strcount);
	}

	painter->restore();
}

QSize FeedItemDelegate::sizeHint(const QStyleOptionViewItem &option, 
	const QModelIndex &index) const
{
	return QStyledItemDelegate::sizeHint(option, index);
}
