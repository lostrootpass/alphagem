#include "EpisodeItemDelegate.h"

#include <QPainter>

#include "EpisodeListModel.h"

EpisodeItemDelegate::EpisodeItemDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
}

EpisodeItemDelegate::~EpisodeItemDelegate()
{
}

void EpisodeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyleOptionViewItem opt = option;
	initStyleOption(&opt, index);

	EpisodeListModel* model = (EpisodeListModel*)index.model();
	opt.font.setBold(!model->getEpisode(index).listened);

	QStyledItemDelegate::paint(painter, opt, index);
}
