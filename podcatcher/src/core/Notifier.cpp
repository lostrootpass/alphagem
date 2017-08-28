#include "Notifier.h"

#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>

#include "Core.h"
#include "ImageDownloader.h"
#include "Settings.h"

#include "core/feeds/Feed.h"
#include "core/feeds/FeedCache.h"

Notifier::Notifier(Core* core)
	: QObject(core), _core(core)
{
	_icon = new QSystemTrayIcon(QApplication::windowIcon(), this);
	_icon->show();

	QMenu* menu = new QMenu();
	menu->addAction(tr("Quit"), QApplication::instance(), &QApplication::quit);
	_icon->setContextMenu(menu);
}

Notifier::~Notifier()
{
	delete _icon;
}

bool Notifier::canNotify(NotificationType type, Feed* feed) const
{
	Settings* settings = _core->settings();

	switch (type)
	{
	case NotificationType::EpisodeChanged:
		return settings->notifyOnNextEpisode();
		break;
	case NotificationType::EpisodeDownloaded:
	{
		return settings->feed(feed).showDownloadNotification;
	}
	break;
	case NotificationType::EpisodeReleased:
	{
		return settings->feed(feed).showReleaseNotification;
	}
	break;
	default:
		return false;
		break;
	}
}

void Notifier::episodeDownloaded(Episode& e)
{
	const Feed* f = _core->feedCache()->feedForEpisode(&e);
	if (!_core->settings()->feed(f).showDownloadNotification)
		return;

	QIcon icon;
	_iconForFeed(&icon, f);

	QString title(tr("Download complete"));
	QString desc = QString("%1 - %2").arg(f->title).arg(e.title);

	_icon->showMessage(title, desc, icon);
}

void Notifier::episodeReleased(const Episode* e)
{
	const Feed* f = _core->feedCache()->feedForEpisode(e);
	if (!_core->settings()->feed(f).showReleaseNotification)
		return;

	_queuedNotifications.insert(NotificationType::EpisodeReleased, e);
}

void Notifier::episodeStarted(Episode* e)
{
	if (!_core->settings()->notifyOnNextEpisode())
		return;

	const Feed* f = _core->feedCache()->feedForEpisode(e);

	QIcon icon;
	_iconForFeed(&icon, f);

	_icon->showMessage(f->title, e->title, icon);
}

bool Notifier::_iconForFeed(QIcon* icon, const Feed* f)
{
	ImageDownloader* img = _core->imageDownloader();
	if (img->isCached(f->imageUrl))
	{
		QPixmap p;

		img->loadPixmap(QUrl(f->imageUrl), &p);
		p = p.scaled(QSize(96, 96), Qt::KeepAspectRatio,
			Qt::SmoothTransformation);

		icon->addPixmap(p);

		return true;
	}
	else
	{
		img->getImage(QUrl(f->imageUrl));
		*icon = QIcon::fromTheme("media-album-cover");

		return false;
	}
}

void Notifier::flushNotifications()
{
	QList<const Episode*> eps;

	eps = _queuedNotifications.values(NotificationType::EpisodeReleased);
	if (eps.size())
	{
		if (eps.size() == 1)
		{
			const Feed* f = _core->feedCache()->feedForEpisode(eps[0]);

			QIcon icon;
			_iconForFeed(&icon, f);

			QString title(tr("New episode available"));
			QString desc = QString("%1 - %2").arg(f->title).arg(eps[0]->title);

			_icon->showMessage(title, desc, icon);
		}
		else
		{
			const Feed* f = _core->feedCache()->feedForEpisode(eps[0]);

			QIcon icon;
			_iconForFeed(&icon, f);

			QString desc = QString(tr("%1 new episodes available"))
				.arg(eps.size());
			
			_icon->showMessage(tr("AlphaGem"), desc, icon);
		}
	}

	_queuedNotifications.clear();
}
