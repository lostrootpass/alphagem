#ifndef PLAYLIST_H_
#define PLAYLIST_H_

#include <QObject>
#include <QString>

#include "feeds/Feed.h"

struct Playlist : public QObject
{
	Q_OBJECT
public:
	Playlist(QString name);

	void add(Episode* e);

	bool contains(const Episode* e) const;

	Episode* popFront();

	void remove(Episode* e);

	QString name;
	QList<Episode*> episodes;

signals:
	void playlistUpdated();
};

#endif
