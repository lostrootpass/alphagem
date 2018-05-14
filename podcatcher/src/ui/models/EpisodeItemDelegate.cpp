#include "EpisodeItemDelegate.h"

#include <QPainter>
#include "EpisodeListProxyModel.h"

EpisodeItemDelegate::EpisodeItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

EpisodeItemDelegate::~EpisodeItemDelegate()
{
}

void EpisodeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	EpisodeListProxyModel* proxyModel = (EpisodeListProxyModel*)index.model();
	proxyModel->refreshIndex(index);

	QStyledItemDelegate::paint(painter, option, index);
}

QSize EpisodeItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSize s = QStyledItemDelegate::sizeHint(option, index);
	s.setHeight(130);
	return s;
}
