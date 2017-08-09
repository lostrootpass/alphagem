#include "AboutWindow.h"

AboutWindow::AboutWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

AboutWindow::~AboutWindow()
{
}

void AboutWindow::on_closeButton_clicked()
{
	close();
}
