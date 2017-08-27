#ifndef FEED_SETTINGS_H_
#define FEED_SETTINGS_H_

enum class EpisodeOrder
{
	NewestFirst,
	OldestFirst
};

enum class AutoPlaylistMode
{
	NoAutoEnqueue,
	AutoEnqueueAfterCurrent,
	AutoEnqueueAtEnd
};

struct FeedSettings
{
	/* Is the first episode displayed the oldest or newest, chronologically */
	EpisodeOrder episodeOrder = EpisodeOrder::NewestFirst;

	/* Automatically add episodes to the playlist after downloading them */
	AutoPlaylistMode autoPlaylistMode = AutoPlaylistMode::NoAutoEnqueue;

	/* Hours between scheduled refreshes */
	int refreshPeriod = 24;

	/* Episodes older than this number of days should be ignored/deleted */
	int ignoreThreshold = 14;

	/* Maximum number of episodes from this feed to store locally */
	int localEpisodeStorageLimit = 5;

	/* if enableSkipIntroSting is true, this number of seconds is skipped */
	int introStingLength = 15;

	/* if enableSkipOutroSting is true, this number of seconds is skipped */
	int outroStingLength = 15;

	/* Desktop notification on new episode appearing in the episode list */
	bool showReleaseNotification = false;

	/* Desktop notification on episode download completing */
	bool showDownloadNotification = false;

	/* Enable skipping the intro sting */
	bool enableSkipIntroSting = false;

	/* Enable skipping the outro sting */
	bool enableSkipOutroSting = false;

	/* Intro/Outro stings are only skipped when playing episodes consecutively
	from the same feed. */
	bool skipOnlyOnBackToBack = false;

	/* Automatically add the next episode to the download queue 
	That is the newest (chronologically) if episodeOrder is NewestFirst
	or the oldest unplayed episode if episodeOrder is OldestFirst*/
	bool autoDownloadNextEpisodes = false;

	/* Delete episodes from the local cache after playback */
	bool deleteAfterPlayback = false;

	/* If true, only localEpisodeStorageLimit eps can be stored */
	bool enableLocalStorageLimit = false;

	/* Include this feed in the list of feeds refreshed at startup */
	bool refreshAtStartup = true;

	/* Allow the feed to refresh as per refreshEvery */
	bool enableRefreshPeriod = true;

	/* Enable the ignore threshold for old episodes */
	bool enableThreshold = false;

	/* Queue another episode from the same feed if this is the last episode
	in the playlist */
	bool autoContinueListening = false;
};

#endif