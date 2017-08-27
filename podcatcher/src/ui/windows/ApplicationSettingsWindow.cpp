#include "ApplicationSettingsWindow.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "core/Core.h"
#include "core/Settings.h"

#include "FeedSettingsWindow.h"

ApplicationSettingsWindow::ApplicationSettingsWindow(Core* c,
	QWidget *parent) : QDialog(parent), _core(c)
{
	ui.setupUi(this);

	ui.saveDirectoryLineEdit->setText(_core->settings()->saveDirectory());
}

ApplicationSettingsWindow::~ApplicationSettingsWindow()
{
}

void ApplicationSettingsWindow::on_saveDirectoryButton_clicked()
{
	QString dir = 
		QFileDialog::getExistingDirectory(this, tr("Select Directory"),
		QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
		QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);

	if(dir != "")
		ui.saveDirectoryLineEdit->setText(dir);
}

void ApplicationSettingsWindow::on_defaultFeedButton_clicked()
{
	FeedSettingsWindow* fsw = new FeedSettingsWindow(_core, nullptr);
	fsw->setAttribute(Qt::WA_DeleteOnClose);
	fsw->setWindowModality(Qt::ApplicationModal);
	fsw->setWindowTitle(tr("Default Feed Settings"));
	fsw->show();
}

void ApplicationSettingsWindow::on_saveButtonBox_accepted()
{
	QString t = ui.saveDirectoryLineEdit->text();
	_core->settings()->setSaveDirectory(t);
	close();
}

void ApplicationSettingsWindow::on_saveButtonBox_clicked(QAbstractButton*)
{

}

void ApplicationSettingsWindow::on_saveButtonBox_rejected()
{
	close();
}
