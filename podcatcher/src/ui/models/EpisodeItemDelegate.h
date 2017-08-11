#pragma once

#include <QStyledItemDelegate>

class EpisodeItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	EpisodeItemDelegate(QObject *parent);
	~EpisodeItemDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
