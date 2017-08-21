#include "PlaybackControlWidget.h"

#include "core/AudioPlayer.h"
#include "core/Core.h"
#include "core/Playlist.h"
#include "core/feeds/Feed.h"

//Skip five seconds per forward/back
const int JUMP_MS = 5000;

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
}

PlaybackControlWidget::~PlaybackControlWidget()
{
}

void PlaybackControlWidget::setupConnections(Core* core)
{
	_core = core;
	_player = core->audioPlayer();

	connect(_player, &AudioPlayer::episodeChanged,
		this, &PlaybackControlWidget::onEpisodeChanged);
	connect(_player, &AudioPlayer::finished,
		this, &PlaybackControlWidget::onFinished);
	connect(_player, &AudioPlayer::pauseStatusChanged,
		this, &PlaybackControlWidget::onPauseStatusChanged);
	connect(_player->getMediaPlayer(), &QMediaPlayer::positionChanged,
		this, &PlaybackControlWidget::onPlayerPositionChanged);

	connect(this, &PlaybackControlWidget::pauseToggled,
		_player, &AudioPlayer::onPlayPauseToggle);

	connect(ui.playbackSlider, &PlaybackSlider::valueChanged,
		_player->getMediaPlayer(), &QMediaPlayer::setPosition);

	connect(core->defaultPlaylist(), &Playlist::playlistUpdated,
		this, &PlaybackControlWidget::onPlaylistUpdated);
}

void PlaybackControlWidget::onEpisodeChanged(const Episode* episode)
{
	ui.episodeName->setText(episode->title);

	ui.playbackSlider->setMinimum(0);
	ui.playbackSlider->setMaximum(episode->duration * 1000);
	ui.playbackSlider->setTickInterval(episode->duration);
	ui.playbackSlider->setValue(0);
	
	ui.playPauseButton->setText("Pause");

	setEnabled(true);
}

void PlaybackControlWidget::onPauseStatusChanged(bool paused)
{
	ui.playPauseButton->setText(paused ? "Pause" : "Play");
}

void PlaybackControlWidget::onPlayerPositionChanged(qint64 milliseconds)
{
	QString formatString = QString("%1/%2")
		.arg(formatMS(milliseconds))
		.arg(formatMS(_player->getMediaPlayer()->duration()));
	
	ui.positionLabel->setText(formatString);
	ui.playbackSlider->setSliderPosition(milliseconds);
}

void PlaybackControlWidget::on_jumpBackButton_clicked()
{
	ui.playbackSlider->setValue(ui.playbackSlider->sliderPosition() - JUMP_MS);
}

void PlaybackControlWidget::on_jumpForwardButton_clicked()
{
	ui.playbackSlider->setValue(ui.playbackSlider->sliderPosition() + JUMP_MS);
}

void PlaybackControlWidget::on_nextEpisodeButton_clicked()
{
	_player->nextEpisode();
}

void PlaybackControlWidget::on_playPauseButton_clicked()
{
	emit pauseToggled();
}

void PlaybackControlWidget::onFinished()
{
	ui.episodeName->setText("");
	setEnabled(false);
}

void PlaybackControlWidget::onPlaylistUpdated()
{
	const bool hasMore = (bool)(_core->defaultPlaylist()->episodes.size());
	ui.nextEpisodeButton->setEnabled(hasMore);
}
