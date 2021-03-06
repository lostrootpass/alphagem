#include "Settings.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QStandardPaths>

#include "Core/Core.h"
#include "Core/Version.h"
#include "Core/Feeds/FeedCache.h"

#include "../ui/windows/MainWindow.h"

typedef QVector<QPair<QString, FeedSettings> > SettingsList;

QDataStream& operator<<(QDataStream& stream, const FeedSettings& settings)
{
	stream << static_cast<int>(settings.episodeOrder);
	stream << static_cast<int>(settings.autoPlaylistMode);
	stream << settings.refreshPeriod;
	stream << settings.ignoreThreshold;
	stream << settings.localEpisodeStorageLimit;
	stream << settings.introStingLength;
	stream << settings.outroStingLength;
	stream << settings.maxSimultaneousDownloads;
	stream << settings.showReleaseNotification;
	stream << settings.showDownloadNotification;
	stream << settings.enableSkipIntroSting;
	stream << settings.enableSkipOutroSting;
	stream << settings.autoDownloadNextEpisodes;
	stream << settings.deleteAfterPlayback;
	stream << settings.enableLocalStorageLimit;
	stream << settings.refreshAtStartup;
	stream << settings.enableRefreshPeriod;
	stream << settings.enableThreshold;
	stream << settings.autoContinueListening;
	stream << settings.autoPlaylistRequiresDownload;

	return stream;
}

QDataStream& operator>>(QDataStream& stream, FeedSettings& settings)
{
	int tmp;
	stream >> tmp;
	settings.episodeOrder = static_cast<EpisodeOrder>(tmp);

	stream >> tmp;
	settings.autoPlaylistMode = static_cast<AutoPlaylistMode>(tmp);

	stream >> settings.refreshPeriod;
	stream >> settings.ignoreThreshold;
	stream >> settings.localEpisodeStorageLimit;
	stream >> settings.introStingLength;
	stream >> settings.outroStingLength;
	stream >> settings.maxSimultaneousDownloads;
	stream >> settings.showReleaseNotification;
	stream >> settings.showDownloadNotification;
	stream >> settings.enableSkipIntroSting;
	stream >> settings.enableSkipOutroSting;
	stream >> settings.autoDownloadNextEpisodes;
	stream >> settings.deleteAfterPlayback;
	stream >> settings.enableLocalStorageLimit;
	stream >> settings.refreshAtStartup;
	stream >> settings.enableRefreshPeriod;
	stream >> settings.enableThreshold;
	stream >> settings.autoContinueListening;
	stream >> settings.autoPlaylistRequiresDownload;

	return stream;
}

Settings::Settings(Core* core, QObject *parent)
	: QObject(parent), _core(core)
{
	_saveDirectory = 
		QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
	_saveDirectory += "/podcasts";
	_saveDirectory = QDir::toNativeSeparators(_saveDirectory);

	_notifyOnNextEpisode = false;
}

Settings::~Settings()
{
}

const FeedSettings& Settings::feed(const Feed* f) const
{
	if (!f || f->useGlobalSettings)
		return _feedDefaults;
	else
		return f->settings;
}

void Settings::loadFromDisk()
{
	QString path =
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	QString filePath = dir.filePath("settings.pod");

	QFile file(filePath);
	if (!file.exists())
		return;

	file.open(QIODevice::ReadOnly);
	QDataStream inStream(&file);
	qint64 fileVersion;
	inStream >> fileVersion;

	//First read in global application settings
	inStream >> _saveDirectory;
	inStream >> _notifyOnNextEpisode;

	//Read in window settings - added in 0.2
	if (fileVersion >= 2)
	{
		QRect rect;
		bool maximised;

		inStream >> rect;
		inStream >> maximised;

		_core->mainWindow()->setGeometry(rect);

		if(maximised)
			_core->mainWindow()->showMaximized();
	}

	//Then read in feed defaults
	inStream >> _feedDefaults;

	//Then read in pairs of guids and feed settings
	SettingsList list;
	inStream >> list;

	for (QPair<QString, FeedSettings> pair : list)
	{
		Feed* f = _core->feedCache()->feedForUrl(pair.first);
		f->settings = pair.second;
	}

	file.close();
}

void Settings::saveToDisk()
{
	QString path =
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	if (!dir.exists())
		dir.mkdir(".");

	QString filePath = dir.filePath("settings.pod");

	QFile file(filePath);
	file.open(QIODevice::WriteOnly);
	QDataStream outStream(&file);
	outStream << VERSION;

	//First write global application settings
	outStream << _saveDirectory;
	outStream << _notifyOnNextEpisode;

	//Write out window settings
	outStream << _core->mainWindow()->geometry();
	outStream << _core->mainWindow()->isMaximized();


	//Then feed defaults
	outStream << _feedDefaults;

	//Then pairs of feed guids and their settings
	SettingsList list;
	for (Feed* f : _core->feedCache()->feeds())
	{
		QPair<QString, FeedSettings> pair;
		list.push_back(qMakePair(f->feedUrl, f->settings));
	}

	outStream << list;

	file.close();
}

void Settings::setFeedDefaults(const FeedSettings& settings)
{
	_feedDefaults = settings;

	emit feedSettingsChanged(nullptr);
}

void Settings::updateFeedSettings(Feed& f, const FeedSettings& settings)
{
	f.settings = settings;
	f.useGlobalSettings = false;

	emit feedSettingsChanged(&f);
}

void Settings::onAboutToQuit()
{
	saveToDisk();
}
