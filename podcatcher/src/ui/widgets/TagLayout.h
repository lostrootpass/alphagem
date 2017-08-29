#pragma once

#include <QLabel>
#include <QVector>

#include "FlowLayout.h"

class TagLabel;

class TagLayout : public FlowLayout
{
	Q_OBJECT

public:
	TagLayout(QWidget *parent = Q_NULLPTR);
	~TagLayout();

	void clearTags();

	void setTags(const QVector<QString>& tags);
};
