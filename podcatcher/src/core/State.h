#pragma once

#include <QObject>

class Core;

class State : public QObject
{
	Q_OBJECT

public:
	State(Core* core, QObject *parent = nullptr);
	~State();

	void loadFromDisk();

	void saveToDisk();

public slots:
	void onAboutToQuit();

private:
	Core* _core;
};
