#include "EpisodeDetailWidget.h"

#include <QDateTime>
#include <QGraphicsEffect>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QPainter>
#include <QTimer>
#include <QToolTip>

#include "core/Core.h"
#include "core/ImageDownloader.h"
#include "core/TimeUtil.h"
#include "core/feeds/Feed.h"

#include "ui/widgets/TagLayout.h"

static const int BGSIZE = 400;

#define max(a,b) (((a) > (b)) ? (a) : (b))

EpisodeDetailWidget::EpisodeDetailWidget(QWidget *parent)
	: QWidget(parent), _core(nullptr), _episode(nullptr)
{
	ui.setupUi(this);
	ui.metadata->setAlignment(Qt::AlignTop);

	connect(ui.description, &QLabel::linkHovered,
		this, &EpisodeDetailWidget::onLinkHovered);

	_tagLayout = new TagLayout(nullptr);
	ui.tagPlaceholder->addLayout(_tagLayout);
}

EpisodeDetailWidget::~EpisodeDetailWidget()
{
	delete _tagLayout;
}

void EpisodeDetailWidget::init(Core* core)
{
	_core = core;

	ui.controlWidget->init(core);

	connect(_core->imageDownloader(), &ImageDownloader::imageDownloaded,
		this, &EpisodeDetailWidget::onImageDownloaded);
}

void EpisodeDetailWidget::setEpisode(Episode* e)
{
	if (_episode)
	{
		_episode->disconnect(this);
	}

	_backgroundOpacity = 0.0f;
	repaint();

	_episode = e;
	connect(_episode, &Episode::updated,
		this, &EpisodeDetailWidget::episodeUpdated);

	_refresh();
}

void EpisodeDetailWidget::_initBackground()
{
	_backgroundRetrievedTimestamp = QDateTime::currentMSecsSinceEpoch();
	_backgroundOpacity = 0.0f;

	QTimer* timer = new QTimer(this);
	connect(timer, &QTimer::timeout, [&, timer]() {
		float opacity = 200.0f;
		qint64 fadeDurationMS = 1000;
		qint64 curr = QDateTime::currentMSecsSinceEpoch();
		qint64 delta = curr - _backgroundRetrievedTimestamp;

		fadeDurationMS; delta;
		if (delta <= fadeDurationMS)
			_backgroundOpacity = opacity * (delta / (float)fadeDurationMS);
		else
		{
			timer->stop();
			timer->deleteLater();
		}

		repaint();
	});

	//60fps
	timer->start(1000 / 60);
}

void EpisodeDetailWidget::_refresh()
{
	ui.title->setText(_episode->title);

	if (_episode->description != "")
	{
		QString text = _episode->description;
		text += _episode->encodedContent;

		ui.description->setText(text);
	}
	else
	{
		QString default =
			tr("<span style='font-style: italic'>No description.</span>");
		ui.description->setText(default);
	}

	ui.newLabel->setVisible(!_episode->listened);

	_setByline();
	_setMetadata();

	ui.controlWidget->update(_episode);

	_tagLayout->clearTags();
	_tagLayout->setTags(_episode->categories);

	QUrl url(_episode->imageUrl);
	if (_core->imageDownloader()->isCached(url))
	{
		_core->imageDownloader()->getImage(url, &_background);
		_initBackground();
	}
	else
	{
		_core->imageDownloader()->getImage(url);
	}
}

void EpisodeDetailWidget::_setByline()
{
	QDateTime dt = parseDateTime(_episode->published);
	QDateTime now = QDateTime::currentDateTimeUtc();
	QString timeAgo = relativeTimeBetween(now, dt);

	QString timestamp = commonTimestamp(_episode->published);
	timestamp = QString(tr("%1 (%2)")).arg(timestamp).arg(timeAgo);

	QString bylineText("");
	if (_episode->author == "")
	{
		bylineText = QString(tr("Posted on %2"))
			.arg(timestamp);
	}
	else
	{
		bylineText = QString(tr("Posted by %1 on %2"))
			.arg(_episode->author).arg(timestamp);
	}

	ui.byline->setText(bylineText);
}

void EpisodeDetailWidget::_setMetadata()
{
	QString metaFormat("<span style='font-weight: bold'>%1:</span> %2<br />");

	QString metatext("");

	if (_episode->shareLink != "")
	{
		QString linkFormat("<a href='%1'>%1</a>");
		QString link = linkFormat.arg(_episode->shareLink);
		metatext += metaFormat.arg(tr("Link")).arg(link);
	}

	metatext += metaFormat
		.arg(tr("Published")).arg(_episode->published);

	ui.metadata->setText(metatext);
}

void EpisodeDetailWidget::episodeUpdated()
{
	_refresh();
}

void EpisodeDetailWidget::onImageDownloaded(QPixmap* px, QString url)
{
	if (!_episode || url != _episode->imageUrl)
		return;

	_background = *px;
	_initBackground();
}

void EpisodeDetailWidget::onLinkHovered(const QString& link)
{
	QToolTip::showText(QCursor::pos(), link);
}

void EpisodeDetailWidget::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);

	if (_background.isNull())
	{
		return;
	}


	float scaleBase = (width() > height() ? height() : width());
	float scaleVal = (scaleBase * 0.8f);
	QPixmap scaledBg = _background.scaled(QSize(scaleVal, scaleVal),
		Qt::KeepAspectRatio, Qt::SmoothTransformation);
	QSize scale = scaledBg.size();

	QPoint point(rect().bottomRight());
	point.setX(point.x() - scale.width());
	point.setY(point.y() - scale.height());

	QGraphicsScene scene;
	QGraphicsPixmapItem item;

	QPoint a1 = QPoint(scale.width(), scale.height());
	QPoint a2 = QPoint(0, 0);

	QLinearGradient alphaGradient(a1, a2);
	alphaGradient.setColorAt(0.0, QColor(0, 0, 0, _backgroundOpacity));
	alphaGradient.setColorAt(1.0, Qt::transparent);
	
	QGraphicsOpacityEffect effect;
	effect.setOpacityMask(alphaGradient);
	
	item.setPixmap(scaledBg);
	item.setGraphicsEffect(&effect);
	scene.addItem(&item);
	
	QImage res(scale, QImage::Format_ARGB32_Premultiplied);
	res.fill(Qt::transparent);

	QPainter resPtr(&res);
	scene.render(&resPtr, QRect(0, 0, scale.width(), scale.height()));

	QPainter bgPainter(this);
	bgPainter.drawImage(point, res);
}

void EpisodeDetailWidget::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
}
