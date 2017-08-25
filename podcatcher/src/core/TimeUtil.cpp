#include "core/TimeUtil.h"

#include <QTextStream>

QDateTime parseDateTime(const QString& datetime)
{
	QDateTime t =
		QDateTime::fromString(datetime, "ddd, dd MMM yyyy HH:mm:ss t");

	if (!t.isValid())
	{
		QStringList parts = datetime.split(" ");
		QString zonepart = parts.takeLast();
		QString newstamp = parts.join(" ");

		t = QDateTime::fromString(newstamp, "ddd, dd MMM yyyy HH:mm:ss");

		if (t.isValid())
		{
			int totalOffset = 0;

			QTextStream stream(&zonepart);
			stream.seek(0);
			QString symbol = stream.read(1);

			//If offset/TZ is unrecognised, leave it as UTC
			if (symbol == "-" || symbol == "+")
			{
				int op = 1;

				if (symbol == "-")
					op = -1;

				int hours = stream.read(2).toInt();
				int minutes = stream.read(2).toInt();

				totalOffset = (((hours * 60 * 60) + (minutes * 60)) * op);
			}

			t.setTimeSpec(Qt::OffsetFromUTC);
			t.setOffsetFromUtc(totalOffset);
		}
	}

	return t;
}

QString formatTime(const QString& original, const QString& format)
{
	//format ex: "ddd dd MMM HH:mm:ss"
	QDateTime dt = parseDateTime(original);
	return dt.toLocalTime().toString(format);
}

QString relativeTimeBetween(const QDateTime& now, const QDateTime& then)
{
	QString timeAgo("");
	qint64 secs = (now.toSecsSinceEpoch() - then.toSecsSinceEpoch());
	qint64 mins = secs / 60;
	qint64 hours = mins / 60;
	qint64 days = hours / 24;

	if (days > 0)
	{
		if (days > 7)
		{
			timeAgo = QString("%1w %2d ago")
				.arg(days / 7).arg(days % 7);
		}
		else
		{
			hours -= (days * 24);
			timeAgo = QString("%1d %2h ago")
				.arg(days).arg((hours));
		}
	}
	else
	{
		mins -= (hours * 60);
		timeAgo = QString("%1h %2m ago").arg(hours).arg(mins);
	}

	return timeAgo;
}

QString commonTimestamp(const QString& datetime)
{
	QDateTime dt = parseDateTime(datetime);
	QDateTime now = QDateTime::currentDateTimeUtc();
	dt.setOffsetFromUtc(0);

	QString dateFormat(QObject::tr("ddd dd MMM HH:mm"));
	if (now.toString("yyyy").toInt() > dt.toString("yyyy").toInt())
		dateFormat = QString(QObject::tr("ddd dd MMM yyyy"));

	return formatTime(datetime, dateFormat);
}