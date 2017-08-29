#include "TagLayout.h"

#include "TagLabel.h"

TagLayout::TagLayout(QWidget *parent)
	: FlowLayout(parent)
{
	
}

TagLayout::~TagLayout()
{
}

void TagLayout::clearTags()
{
	while (QLayoutItem* item = takeAt(0))
	{
		delete item->widget();
	}
}

void TagLayout::setTags(const QVector<QString>& tags)
{
	for (QString s : tags)
	{
		TagLabel* l = new TagLabel(s, qobject_cast<QWidget*>(parent()));
		addWidget(l);
		l->show();
	}
}
