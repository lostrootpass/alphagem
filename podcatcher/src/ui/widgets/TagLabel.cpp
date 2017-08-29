#include "TagLabel.h"

#include <QPainter>
#include <QPixmap>

TagLabel::TagLabel(const QString& text, QWidget *parent)
	: QLabel(text, parent)
{
	ui.setupUi(this);

	setText(text);

	_icon = QPixmap(":/icons/breeze/16/tag.svg");
}

TagLabel::~TagLabel()
{
}

void TagLabel::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.drawPixmap(QPoint(6, 6), _icon);

	QLabel::paintEvent(event);
}
