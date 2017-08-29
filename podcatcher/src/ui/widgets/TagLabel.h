#pragma once

#include <QLabel>
#include <QPixmap>
#include "ui_TagLabel.h"

class TagLabel : public QLabel
{
	Q_OBJECT

public:
	TagLabel(const QString& text, QWidget *parent = Q_NULLPTR);
	~TagLabel();

private:
	Ui::TagLabel ui;
	QPixmap _icon;

protected:
	void paintEvent(QPaintEvent *event) override;
};
