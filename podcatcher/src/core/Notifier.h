#pragma once

#include <QObject>
#include <QMultiMap>
#include <QPixmap>

class QSystemTrayIcon;

class Core;
struct Episode;
struct Feed;

enum class NotificationType
{
	EpisodeChanged,
	EpisodeDownloaded,
	EpisodeReleased
};

class Notifier : public QObject
{
	Q_OBJECT

public:
	Notifier(Core* core);
	~Notifier();

	bool canNotify(NotificationType type, Feed* feed) const;

	void episodeReleased(const Episode* e);

public slots:
	void episodeDownloaded(Episode& e);
	void episodeStarted(Episode* e);
	void flushNotifications();

private:
	QSystemTrayIcon* _icon;
	Core* _core;

	QMultiMap<NotificationType, const Episode*> _queuedNotifications;

	bool _iconForFeed(QIcon* icon, const Feed* f);
};
