#pragma once

#include <QWidget>
#include "ui_AboutWindow.h"

class AboutWindow : public QWidget
{
	Q_OBJECT

public:
	AboutWindow(QWidget *parent = Q_NULLPTR);
	~AboutWindow();

private:
	Ui::AboutWindow ui;

private slots:
	void on_closeButton_clicked();
};
