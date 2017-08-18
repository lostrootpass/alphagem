#include "PlaybackControlWidget.h"

#include "core/AudioPlayer.h"
#include "core/feeds/Feed.h"

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

void PlaybackControlWidget::connectToAudioPlayer(AudioPlayer* player)
{
	_player = player;
	connect(player, &AudioPlayer::episodeChanged, this, &PlaybackControlWidget::onEpisodeChanged);
	connect(player, &AudioPlayer::pauseStatusChanged, this, &PlaybackControlWidget::onPauseStatusChanged);
	connect(player->getMediaPlayer(), &QMediaPlayer::positionChanged, this, &PlaybackControlWidget::onPlayerPositionChanged);

	connect(this, &PlaybackControlWidget::pauseToggled, player, &AudioPlayer::onPlayPauseToggle);

	connect(ui.playbackSlider, &PlaybackSlider::valueChanged, player->getMediaPlayer(), &QMediaPlayer::setPosition);
}

void PlaybackControlWidget::onEpisodeChanged(const Episode* episode)
{
	ui.episodeName->setText(episode->title);

	ui.playbackSlider->setEnabled(true);
	ui.playbackSlider->setMinimum(0);
	ui.playbackSlider->setMaximum(episode->duration * 1000);
	ui.playbackSlider->setTickInterval(episode->duration);

	ui.playPauseButton->setText("Pause");
	ui.playPauseButton->setEnabled(true);
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

void PlaybackControlWidget::on_playPauseButton_clicked()
{
	emit pauseToggled();
}
