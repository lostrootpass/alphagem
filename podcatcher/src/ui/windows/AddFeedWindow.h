#pragma once

#include <QWidget>
#include "ui_AddFeedWindow.h"

class AddFeedWindow : public QWidget
{
	Q_OBJECT

public:
	AddFeedWindow(QWidget *parent = Q_NULLPTR);
	~AddFeedWindow();

signals:
	void feedAdded(QString&);

private:
	Ui::AddFeedWindow ui;

private slots:
	/* Auto generated slots */
	void on_addFeedButton_clicked();
	void on_cancelButton_clicked();
};
