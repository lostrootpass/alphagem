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

	inline const QString& saveDirectory() const { return _saveDirectory; }

	void saveToDisk();

	void setFeedDefaults(const FeedSettings& settings);

	inline void setSaveDirectory(const QString& dir) { _saveDirectory = dir; }

	void updateFeedSettings(Feed& f, const FeedSettings& settings);

signals:
	void feedSettingsChanged(Feed* feed);

public slots:
	void onAboutToQuit();

private:
	FeedSettings _feedDefaults;
	Core* _core;

	QString _saveDirectory;
};
