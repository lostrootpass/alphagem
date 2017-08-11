#include "PlaybackControlWidget.h"

#include "core/AudioPlayer.h"
#include "core/feeds/Feed.h"

PlaybackControlWidget::PlaybackControlWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

PlaybackControlWidget::~PlaybackControlWidget()
{
}

void PlaybackControlWidget::connectToAudioPlayer(AudioPlayer* player)
{
	connect(player, &AudioPlayer::episodeChanged, this, &PlaybackControlWidget::onEpisodeChanged);
	connect(player, &AudioPlayer::pauseStatusChanged, this, &PlaybackControlWidget::onPauseStatusChanged);
	connect(player->getMediaPlayer(), &QMediaPlayer::positionChanged, this, &PlaybackControlWidget::onPlayerPositionChanged);

	connect(this, &PlaybackControlWidget::pauseToggled, player, &AudioPlayer::onPlayPauseToggle);
}

void PlaybackControlWidget::onEpisodeChanged(const Episode* episode)
{
	ui.episodeName->setText(episode->title);

	ui.playbackPositionBar->setMinimum(0);
	ui.playbackPositionBar->setMaximum(episode->duration * 1000);
	ui.playbackPositionBar->setValue(0);

	ui.playPauseButton->setText("Pause");
	ui.playPauseButton->setEnabled(true);
}

void PlaybackControlWidget::onPauseStatusChanged(bool paused)
{
	ui.playPauseButton->setText(paused ? "Pause" : "Play");
}

void PlaybackControlWidget::onPlayerPositionChanged(qint64 milliseconds)
{
	/* Store this here so it can be changed easily if needed in 2064 */
	const qint64 secondsPerMinute = 60;

	qint64 seconds = milliseconds / 1000;
	qint64 minutes = seconds / secondsPerMinute;
	QString formatString;
	formatString = QString("%1:%2")
						.arg(minutes, 2, 10, QChar('0'))
						.arg(seconds % secondsPerMinute, 2, 10, QChar('0'));

	ui.playbackPositionBar->setValue(milliseconds);
	ui.playbackPositionBar->setFormat(formatString);
}

void PlaybackControlWidget::on_playPauseButton_clicked()
{
	emit pauseToggled();
}
