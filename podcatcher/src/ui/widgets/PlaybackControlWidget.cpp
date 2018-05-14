#include "PlaybackControlWidget.h"

#include "core/AudioPlayer.h"
#include "core/Core.h"
#include "core/ImageDownloader.h"
#include "core/Playlist.h"
#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

#include "ui/widgets/FeedIconWidget.h"

//Skip five seconds per forward/back
const int SHORT_JUMP_MS = 5000;

//10s per Ctrl+Jump
const int MEDIUM_JUMP_MS = 10000;

//30s per Shift+Jump
const int LONG_JUMP_MS = 30000;

QString formatMS(qint64 milliseconds)
{
	qint64 seconds = milliseconds / 1000;
	qint64 minutes = seconds / 60;
	QString formatString;
	formatString = QString("%1:%2")
		.arg(minutes, 2, 10, QChar('0'))
		.arg(seconds % 60, 2, 10, QChar('0'));

	return formatString;

}

PlaybackControlWidget::PlaybackControlWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.episodeIcon->resetDefault();
	PlaybackSliderStyle* s = new PlaybackSliderStyle(ui.volumeSlider->style());
	ui.volumeSlider->setStyle(s);
}

PlaybackControlWidget::~PlaybackControlWidget()
{
}

void PlaybackControlWidget::setupConnections(Core* core)
{
	_core = core;
	_player = core->audioPlayer();
	QMediaPlayer* mp = _player->getMediaPlayer();

	connect(_player, &AudioPlayer::episodeChanged,
		this, &PlaybackControlWidget::onEpisodeChanged);
	connect(_player, &AudioPlayer::finished,
		this, &PlaybackControlWidget::onFinished);
	connect(_player, &AudioPlayer::pauseStatusChanged,
		this, &PlaybackControlWidget::onPauseStatusChanged);
	connect(mp, &QMediaPlayer::durationChanged,
		this, &PlaybackControlWidget::onDurationChanged);
	connect(mp, &QMediaPlayer::positionChanged,
		this, &PlaybackControlWidget::onPlayerPositionChanged);

	connect(this, &PlaybackControlWidget::pauseToggled,
		_player, &AudioPlayer::onPlayPauseToggle);

	connect(ui.playbackSlider, &PlaybackSlider::valueChanged,
		mp, &QMediaPlayer::setPosition);

	connect(core->defaultPlaylist(), &Playlist::playlistUpdated,
		this, &PlaybackControlWidget::onPlaylistUpdated);

	connect(ui.episodeIcon, &FeedIconWidget::clicked,
		this, &PlaybackControlWidget::onIconClicked);

	connect(this, &PlaybackControlWidget::muteToggled,
		mp, &QMediaPlayer::setMuted);
	connect(mp, &QMediaPlayer::mutedChanged,
		this, &PlaybackControlWidget::onMutedChanged);

	connect(this, &PlaybackControlWidget::volumeChanged,
		mp, &QMediaPlayer::setVolume);
	connect(mp, &QMediaPlayer::volumeChanged,
		this, &PlaybackControlWidget::onVolumeChanged);
}

void PlaybackControlWidget::onDurationChanged(qint64 duration)
{
	ui.playbackSlider->setMinimum(0);
	ui.playbackSlider->setMaximum(duration);
	ui.playbackSlider->setTickInterval(duration);
	ui.playbackSlider->setValue(0);
}

void PlaybackControlWidget::onEpisodeChanged(Episode* episode)
{
	_episode = episode;
	ui.episodeName->setText(episode->title);

	ui.playPauseButton->setIcon(QIcon::fromTheme("media-playback-pause"));

	QUrl url(_core->feedCache()->feedForEpisode(episode)->imageUrl);
	ui.episodeIcon->resetDefault();
	_core->imageDownloader()->getImage(url, ui.episodeIcon);

	setEnabled(true);
}

void PlaybackControlWidget::onPauseStatusChanged(bool paused)
{
	QString icon = paused ? "media-playback-start" : "media-playback-pause";
	ui.playPauseButton->setIcon(QIcon::fromTheme(icon));
}

void PlaybackControlWidget::onPlayerPositionChanged(qint64 milliseconds)
{
	QString formatString = QString("%1/%2")
		.arg(formatMS(milliseconds))
		.arg(formatMS(_player->getMediaPlayer()->duration()));
	
	ui.positionLabel->setText(formatString);
	ui.playbackSlider->setSliderPosition(milliseconds);
}

void PlaybackControlWidget::onIconClicked()
{
	emit episodeSelected(_episode);
}

void PlaybackControlWidget::onMutedChanged(bool muted)
{
	if (muted)
	{
		ui.muteButton->setIcon(QIcon::fromTheme("audio-volume-muted"));
	}
	else
	{
		int volume = _core->audioPlayer()->getMediaPlayer()->volume();
		_updateMuteIcon(volume);
	}
}

int PlaybackControlWidget::_getJumpLen()
{
	Qt::KeyboardModifiers mask = QApplication::queryKeyboardModifiers();

	if (mask & Qt::ControlModifier)
		return MEDIUM_JUMP_MS;
	if (mask & Qt::ShiftModifier)
		return LONG_JUMP_MS;
	else
		return SHORT_JUMP_MS;
}

void PlaybackControlWidget::_updateMuteIcon(int volume)
{
	QString icon;

	if (volume > 65)
		icon = "audio-volume-high";
	else if (volume > 25)
		icon = "audio-volume-medium";
	else
		icon = "audio-volume-low";

	ui.muteButton->setIcon(QIcon::fromTheme(icon));
}

void PlaybackControlWidget::on_jumpBackButton_clicked()
{
	int jumpLen = _getJumpLen();
	ui.playbackSlider->setValue(ui.playbackSlider->sliderPosition() - jumpLen);
}

void PlaybackControlWidget::on_jumpForwardButton_clicked()
{
	int jumpLen = _getJumpLen();
	ui.playbackSlider->setValue(ui.playbackSlider->sliderPosition() + jumpLen);
}

void PlaybackControlWidget::on_muteButton_clicked()
{
	bool muted = _core->audioPlayer()->getMediaPlayer()->isMuted();
	emit muteToggled(!muted);
}

void PlaybackControlWidget::on_nextEpisodeButton_clicked()
{
	qint64 duration = _player->getMediaPlayer()->duration();
	_player->getMediaPlayer()->setPosition(duration);
}

void PlaybackControlWidget::on_playPauseButton_clicked()
{
	emit pauseToggled();
}

void PlaybackControlWidget::on_volumeSlider_valueChanged(int value)
{
	emit muteToggled(false);
	emit volumeChanged(value);
}

void PlaybackControlWidget::onFinished()
{
	ui.playbackSlider->setValue(0);
	ui.episodeName->setText("");
	ui.positionLabel->setText("");
	ui.episodeIcon->resetDefault();
	setEnabled(false);
}

void PlaybackControlWidget::onPlaylistUpdated()
{
	const bool hasMore = (_core->defaultPlaylist()->episodes.size() > 1);
	ui.nextEpisodeButton->setEnabled(hasMore);
}

void PlaybackControlWidget::onVolumeChanged(int volume)
{
	ui.volumeSlider->setValue(volume);
	_updateMuteIcon(volume);
}
