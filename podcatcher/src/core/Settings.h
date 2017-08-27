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

	void saveToDisk();

	void setFeedDefaults(const FeedSettings& settings);

	void updateFeedSettings(Feed& f, const FeedSettings& settings);

signals:
	void feedSettingsChanged(Feed* feed);

public slots:
	void onAboutToQuit();

private:
	FeedSettings _feedDefaults;
	Core* _core;
};
