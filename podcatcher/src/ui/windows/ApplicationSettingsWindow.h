#pragma once

#include <QDialog>
#include "ui_ApplicationSettingsWindow.h"

class Core;

class ApplicationSettingsWindow : public QDialog
{
	Q_OBJECT

public:
	ApplicationSettingsWindow(Core* c, QWidget *parent = Q_NULLPTR);
	~ApplicationSettingsWindow();

private:
	Ui::ApplicationSettingsWindow ui;

	Core* _core;

private slots:
	void on_saveDirectoryButton_clicked();

	void on_defaultFeedButton_clicked();

	/* Save/Cancel buttons */
	void on_saveButtonBox_accepted();
	void on_saveButtonBox_clicked(QAbstractButton*);
	void on_saveButtonBox_rejected();
};
