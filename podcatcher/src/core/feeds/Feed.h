#ifndef FEED_H_
#define FEED_H_

#include <QObject>
#include <QString>
#include <QVector>
#include <ctime>

struct Episode : public QObject
{
	Q_OBJECT;
public:
	QString title;
	QString description;
	QString mediaUrl;
	QString imageUrl;
	QString guid;
	QString mediaFormat;
	QString shareLink;
	QString author;

	QVector<QString> categories;

	std::time_t published;

	qint32 duration;

	bool isExplicit;
	bool listened;

	inline void setListened(bool l = true)
	{
		listened = l;
		emit updated();
	}

signals:
	void updated();
};

struct Feed
{
	Feed() {}
	~Feed()
	{
		for (Episode* e : episodes)
		{
			delete e;
		}
	}

	QString feedUrl;
	QString title;
	QString link;
	QString description;
	QString imageUrl;

	QString creator;
	QString ownerName;
	QString ownerEmail;

	std::time_t lastUpdated;

	QVector<Episode*> episodes;
	QVector<QString> categories;

	bool isExplicit;
};

#endif