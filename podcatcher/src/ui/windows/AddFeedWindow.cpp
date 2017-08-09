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
	emit feedAdded(ui.urlInput->text());
	close();
}

void AddFeedWindow::on_cancelButton_clicked()
{
	close();
}
