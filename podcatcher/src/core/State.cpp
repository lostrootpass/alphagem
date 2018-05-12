#include "State.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QMediaPlayer>
#include <QStandardPaths>

#include "AudioPlayer.h"
#include "Core.h"
#include "Version.h"
#include "core/feeds/EpisodeCache.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

State::State(Core* core, QObject *parent) : QObject(parent), _core(core)
{

}

State::~State()
{
}

void State::loadFromDisk()
{
	QString path =
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	QString filePath = dir.filePath("state.pod");

	QFile file(filePath);
	if (!file.exists())
		return;

	file.open(QIODevice::ReadOnly);
	QDataStream inStream(&file);
	qint64 fileVersion;
	inStream >> fileVersion;

	QVector<QString> guids;
	inStream >> guids;

	for (const QString& s : guids)
	{
		Episode* e = _core->feedCache()->getEpisode(s);
		_core->episodeCache()->enqueueDownload(e);
	}

	bool downloadsPaused;
	inStream >> downloadsPaused;
	if (downloadsPaused)
		_core->episodeCache()->pauseCurrent();

	qint64 currentPosition;
	QString currentGuid;

	inStream >> currentPosition;
	inStream >> currentGuid;

	guids.clear();

	inStream >> guids;

	Playlist* p = _core->defaultPlaylist();
	FeedCache* feedCache = _core->feedCache();
	for (const QString& s : guids)
	{
		p->episodes.push_back(feedCache->getEpisode(s));
	}

	if (currentGuid != "")
	{
		p->emplaceFront(feedCache->getEpisode(currentGuid));
		AudioPlayer* player = _core->audioPlayer();
		player->nextEpisode();
		
		//Duration won't be set straight away, so store it & update later
		player->setResumePos(currentPosition);

		player->pause();
	}

	file.close();
}

void State::saveToDisk()
{
	QString path =
		QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	QDir dir(path);
	if (!dir.exists())
		dir.mkdir(".");

	QString filePath = dir.filePath("state.pod");

	QFile file(filePath);
	file.open(QIODevice::WriteOnly);
	QDataStream outStream(&file);
	outStream << VERSION;

	QVector<QString> guids;

	const QList<DownloadInfo*> dl = _core->episodeCache()->downloadList();
	for (const DownloadInfo* i : dl)
	{
		guids.push_back(i->episode->guid);
	}

	outStream << guids;

	outStream << (dl.size() && dl[0]->paused);

	const AudioPlayer* player = _core->audioPlayer();

	const Episode* currentEp = player->currentEpisode();
	qint64 currentPosition = 0;
	QString currentGuid = "";

	if (currentEp)
	{
		currentPosition = player->getMediaPlayer()->position();
		currentGuid = currentEp->guid;
	}

	outStream << currentPosition;
	outStream << currentGuid;

	guids.clear();
	const Playlist* playlist = _core->defaultPlaylist();
	for (const Episode* e : playlist->episodes)
	{
		guids.push_back(e->guid);
	}

	outStream << guids;

	file.close();
}

void State::onAboutToQuit()
{
	saveToDisk();
}
