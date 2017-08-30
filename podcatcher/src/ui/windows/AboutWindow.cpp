#include "AboutWindow.h"

#include "core/Version.h"

AboutWindow::AboutWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.versionLabel->setText(VERSION_STR);
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::on_closeButton_clicked()
{
	close();
}
