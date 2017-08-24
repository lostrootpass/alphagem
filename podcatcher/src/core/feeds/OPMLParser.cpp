#include "OPMLParser.h"

#include <QFile>
#include <QTime>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "Feed.h"

OPMLParser::OPMLParser(QObject *parent)
	: QObject(parent)
{
}

OPMLParser::~OPMLParser()
{
}

void OPMLParser::save(const QString& fileName, const QVector<Feed*>& feeds)
{
	QString timestamp = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

	QFile file(fileName);
	file.open(QIODevice::WriteOnly);
	
	QXmlStreamWriter xml(&file);
	xml.setAutoFormatting(true);
	xml.writeStartDocument();
	
		xml.writeStartElement("opml");
		xml.writeAttribute("version", "2.0");
	
		xml.writeStartElement("head");
			xml.writeTextElement("title", tr("Exported Feed List"));
			xml.writeTextElement("dateCreated", timestamp);
			xml.writeTextElement("dateModified", timestamp);
		xml.writeEndElement();


		xml.writeStartElement("body");
			for (const Feed* f : feeds)
			{
				xml.writeEmptyElement("outline");
				xml.writeAttribute("text", f->title);
				xml.writeAttribute("type", "rss");
				xml.writeAttribute("xmlUrl", f->feedUrl);
				xml.writeAttribute("htmlUrl", f->link);
			}
		xml.writeEndElement();


		xml.writeEndElement();
	xml.writeEndDocument();

	file.close();
}

void OPMLParser::parse(const QString& filename)
{
	QFile file(filename);
	if (!file.exists())
		return;

	file.open(QIODevice::ReadOnly);
	QXmlStreamReader xml;
	xml.addData(file.readAll());
	file.close();

	while (!xml.atEnd())
	{
		xml.readNext();

		if (xml.isStartElement() && xml.name() == "outline")
		{
			QStringRef url = xml.attributes().value("xmlUrl");
			if (url != "")
			{
				_urls.push_back(url.toString());
			}
		}
	}
}
