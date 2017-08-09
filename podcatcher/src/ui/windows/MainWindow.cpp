#include "MainWindow.h"
#include "AddFeedWindow.h"
#include "AboutWindow.h"

#include "src/core/FeedParser.h"
#include "src/core/Feed.h"

#include <QProgressBar>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), _feedParser(nullptr)
{
	ui.setupUi(this);

	_progressBar = new QProgressBar();
	statusBar()->addPermanentWidget(_progressBar);
}

MainWindow::~MainWindow()
{
	if (_feedParser)
		delete _feedParser;
}

void MainWindow::on_actionAdd_Feed_triggered()
{
	AddFeedWindow* feedWindow = new AddFeedWindow();
	feedWindow->setAttribute(Qt::WA_DeleteOnClose);

	connect(feedWindow, &AddFeedWindow::feedAdded, this, &MainWindow::onFeedAdded);

	feedWindow->show();
}

void MainWindow::on_actionQuit_triggered()
{
	QApplication::quit();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
	QApplication::aboutQt();
}

void MainWindow::on_actionAbout_triggered()
{
	AboutWindow* window = new AboutWindow();
	window->setAttribute(Qt::WA_DeleteOnClose);

	window->show();
}

void MainWindow::onFeedAdded(QString& feed)
{
	if (!_feedParser)
	{
		_feedParser = new FeedParser(nullptr);

		connect(_feedParser, &FeedParser::downloadFailed, this, &MainWindow::onStatusBarUpdate);
		connect(_feedParser, &FeedParser::updateProgress, _progressBar, &QProgressBar::setValue);
		connect(_feedParser, &FeedParser::feedRetrieved, this, &MainWindow::onFeedRetrieved);
	}



	_feedParser->parseFromRemoteFile(feed);
}

void MainWindow::onStatusBarUpdate(QString& text)
{
	statusBar()->showMessage(text);
}

void MainWindow::onFeedRetrieved(Feed* feed)
{
	ui.feedNameLabel->setText(feed->title);

	for (QString& s : feed->episodes)
	{
		ui.listWidget->addItem(s);
	}
}
