#include "PlaybackControlWidget.h"

#include "core/AudioPlayer.h"
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

void PlaybackControlWidget::connectToAudioPlayer(AudioPlayer* player)
{
	_player = player;
	connect(player, &AudioPlayer::episodeChanged, this, &PlaybackControlWidget::onEpisodeChanged);
	connect(player, &AudioPlayer::finished, this, &PlaybackControlWidget::onFinished);
	connect(player, &AudioPlayer::pauseStatusChanged, this, &PlaybackControlWidget::onPauseStatusChanged);
	connect(player->getMediaPlayer(), &QMediaPlayer::positionChanged, this, &PlaybackControlWidget::onPlayerPositionChanged);

	connect(this, &PlaybackControlWidget::pauseToggled, player, &AudioPlayer::onPlayPauseToggle);

	connect(ui.playbackSlider, &PlaybackSlider::valueChanged, player->getMediaPlayer(), &QMediaPlayer::setPosition);
}

void PlaybackControlWidget::onEpisodeChanged(const Episode* episode)
{
	ui.episodeName->setText(episode->title);

	ui.playbackSlider->setMinimum(0);
	ui.playbackSlider->setMaximum(episode->duration * 1000);
	ui.playbackSlider->setTickInterval(episode->duration);

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

void PlaybackControlWidget::on_playPauseButton_clicked()
{
	emit pauseToggled();
}

void PlaybackControlWidget::onFinished()
{
	ui.episodeName->setText("");
	setEnabled(false);
}
