#ifndef FEED_H_

#include <QString>
#include <QVector>

struct Feed
{
	QString title;
	QVector<QString> episodes;
};

#endif