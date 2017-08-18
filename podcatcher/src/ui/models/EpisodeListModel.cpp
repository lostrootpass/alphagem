#include "EpisodeListModel.h"

#include "ui/widgets/EpisodeDetailWidget.h"
#include "MainWindow.h"

EpisodeListModel::EpisodeListModel(QListView& view, FeedCache& cache, int feed, QObject* parent) 
	: QAbstractListModel(parent), _view(&view), _feedCache(&cache), _feedIndex(feed)
{
}

EpisodeListModel::~EpisodeListModel()
{
}

int EpisodeListModel::rowCount(const QModelIndex&) const
{
	return _epCount();
}

QVariant EpisodeListModel::data(const QModelIndex &, int ) const
{
	return QVariant();
}

Episode& EpisodeListModel::getEpisode(const QModelIndex& index) const
{
	return _feedCache->episodes(_feedIndex)[_epCount() - 1 - index.row()];
}

void EpisodeListModel::markAsPlayed(const QModelIndex& index)
{
	getEpisode(index).listened = true;

	emit dataChanged(index, index);
}

void EpisodeListModel::refreshIndex(const QModelIndex& index)
{
	if(!_view->indexWidget(index))
		_view->setIndexWidget(index, _getWidget(index));
}

void EpisodeListModel::setFeedIndex(int newIndex)
{
	if (_feedIndex == newIndex)
		return;

	_feedIndex = newIndex;

	emit layoutChanged();
}

EpisodeDetailWidget* EpisodeListModel::_getWidget(const QModelIndex& index) const
{
	EpisodeDetailWidget* e = new EpisodeDetailWidget(*this, index, nullptr);
	MainWindow* w = qobject_cast<MainWindow*>(_view->window());
	connect(e, &EpisodeDetailWidget::play, w, &MainWindow::onEpisodeSelected);
	connect(e, &EpisodeDetailWidget::download, w, &MainWindow::onDownloadStarted);

	//Item view will take ownership of the widget for us.
	return e;
}
