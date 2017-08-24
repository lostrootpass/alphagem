#pragma once

#include <QObject>
#include <QVector>

struct Feed;

class OPMLParser : public QObject
{
	Q_OBJECT

public:
	OPMLParser(QObject *parent);
	~OPMLParser();

	void save(const QString& fileName, const QVector<Feed*>& feeds);

	void parse(const QString& filename);

	inline const QVector<QString>& urls() const 
	{
		return _urls;
	}

private:
	QVector<QString> _urls;
};
