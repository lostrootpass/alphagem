#include "AddFeedWindow.h"

AddFeedWindow::AddFeedWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.urlInput, &QLineEdit::returnPressed, this, &AddFeedWindow::on_addFeedButton_clicked);
}

AddFeedWindow::~AddFeedWindow()
{
}

void AddFeedWindow::on_addFeedButton_clicked()
{
	QString s = ui.urlInput->text();
	emit feedAdded(s);
	close();
}

void AddFeedWindow::on_cancelButton_clicked()
{
	close();
}
