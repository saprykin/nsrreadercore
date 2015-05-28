#include "nsrtextdocument.h"
#include "nsrcharsetdetector.h"

#include <qmath.h>

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QTextCodec>
#include <QTextStream>

#define NSR_CORE_TEXT_DETECT_CHARS	1024
#define NSR_CORE_TEXT_LINES_IN_PAGE	25

NSRTextDocument::NSRTextDocument (const QString &file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_pageCount (0),
	_wasEncodingDetected (false)
{
}

NSRTextDocument::~NSRTextDocument ()
{
}

int
NSRTextDocument::getPageCount () const
{
	return _pageCount;
}

NSRRenderInfo
NSRTextDocument::renderPage (int page)
{
	NSRRenderInfo rinfo;

	if (page < 1)
		return rinfo;

	if (!isValid ())
		return rinfo;

	_text = QString ();

	QString charset = getEncoding ();

	if (charset.isEmpty () && !_wasEncodingDetected) {
		_autodetectedEncoding = detectCharset ();
		_wasEncodingDetected  = true;
	}

	if (charset.isEmpty ())
		charset = _autodetectedEncoding;

	if (charset != _hashBuildEncoding)
		rebuildHash (charset);

	if (page > getPageCount ())
		return rinfo;

	_text = _textData.value (page, QString ());

	rinfo.setSuccessRender (true);

	return rinfo;
}

NSR_CORE_IMAGE_DATATYPE
NSRTextDocument::getCurrentPage ()
{
	return NSR_CORE_IMAGE_DATATYPE ();
}

bool
NSRTextDocument::isValid () const
{
	QFileInfo info (getDocumentPath ());

	return info.exists ();
}

QString
NSRTextDocument::getText ()
{
	QString ret = _text;
	_text.clear ();

	return ret;
}

bool
NSRTextDocument::isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const
{
	return (style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
}

QString
NSRTextDocument::detectCharset ()
{
	if (!QFile::exists (getDocumentPath ()))
		return QString ();

	NSRCharsetDetector	detector;
	QFile			data (getDocumentPath ());

	if (!data.open (QFile::ReadOnly))
		return QString ();

	QDataStream	in (&data);
	QByteArray	ba;
	int		bytesRead;

	ba.resize (NSR_CORE_TEXT_DETECT_CHARS);

	while (!detector.isCharsetDetected () && !in.atEnd ()) {
		if ((bytesRead = in.readRawData (ba.data (), NSR_CORE_TEXT_DETECT_CHARS)) == -1) {
			detector.finishData ();
			return detector.getCharset ();
		}

		detector.addData (ba, bytesRead);

		if (detector.isCharsetDetected ())
			return detector.getCharset ();
	}

	/* Reached end of the file */
	if (!detector.isCharsetDetected ())
		detector.finishData ();

	/* The best try */
	return detector.getCharset ();
}

void
NSRTextDocument::rebuildHash (const QString& encoding)
{
	_textData.clear ();

	QFile data (getDocumentPath ());

	if (!data.open (QFile::ReadOnly))
		return;

	QTextStream stream (&data);
	stream.setCodec (encoding.toAscii().data ());

	int pageCounter  = 1;
	int totalCounter = 0;

	while (!stream.atEnd () && totalCounter <= NSR_CORE_DOCUMENT_MAX_HEAP) {
		QString pageText;
		int lineCounter = 0;

		while (lineCounter != NSR_CORE_TEXT_LINES_IN_PAGE && !stream.atEnd ()) {
			QString line = stream.readLine () + QString ("\n");

			pageText     += line;
			totalCounter += line.toAscii().length ();

			++lineCounter;

			if (totalCounter > NSR_CORE_DOCUMENT_MAX_HEAP)
				break;
		}

		_textData.insert (pageCounter++, pageText);
	}

	_pageCount         = pageCounter - 1;
	_hashBuildEncoding = encoding;
}
