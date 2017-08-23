#pragma once

#include <QStyledItemDelegate>

class FeedItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	FeedItemDelegate(QObject *parent);
	~FeedItemDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;

	QSize sizeHint(const QStyleOptionViewItem &option,
		const QModelIndex &index) const override;
};
