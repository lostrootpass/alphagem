#pragma once

#include <QObject>

#include "core/feeds/FeedSettings.h"

class Core;
struct Feed;


class Settings : public QObject
{
	Q_OBJECT

public:
	Settings(Core* core, QObject *parent = nullptr);
	~Settings();

	const FeedSettings& feed(const Feed* f) const;

	inline const FeedSettings& feedDefaults() const { return _feedDefaults; }

	void loadFromDisk();

	bool notifyOnNextEpisode() const { return _notifyOnNextEpisode; }

	inline const QString& saveDirectory() const { return _saveDirectory; }

	void saveToDisk();

	void setFeedDefaults(const FeedSettings& settings);

	inline void setAllowNotification(bool b) { _notifyOnNextEpisode = b; }

	inline void setSaveDirectory(const QString& dir) { _saveDirectory = dir; }

	void updateFeedSettings(Feed& f, const FeedSettings& settings);

signals:
	void feedSettingsChanged(Feed* feed);

public slots:
	void onAboutToQuit();

private:
	FeedSettings _feedDefaults;
	Core* _core;

	/* The actual settings */
	QString _saveDirectory;
	bool _notifyOnNextEpisode;
};
