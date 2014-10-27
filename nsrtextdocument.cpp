#include "nsrtextdocument.h"
#include "nsrcharsetdetector.h"

#include <qmath.h>

#include <QFile>
#include <QFileInfo>
#include <QDataStream>
#include <QTextCodec>

#define NSR_CORE_TEXT_PAGE_SIZE		5120
#define NSR_CORE_TEXT_DETECT_CHARS	1024

NSRTextDocument::NSRTextDocument (const QString &file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_pagesCount (0),
	_wasEncodingDetected (false)
{
	QFileInfo info (file);

	_pagesCount = (int) (ceil ((double) info.size () / NSR_CORE_TEXT_PAGE_SIZE) + 0.5);
}

NSRTextDocument::~NSRTextDocument ()
{
}

int
NSRTextDocument::getPagesCount () const
{
	return _pagesCount;
}

NSRRenderInfo
NSRTextDocument::renderPage (int page)
{
	NSRRenderInfo rinfo;

	if (page < 1 || page > getPagesCount ())
		return rinfo;

	if (!isValid ())
		return rinfo;

	_text = QString ();

	QString charset = getEncoding ();

	if (charset.isEmpty () && !_wasEncodingDetected) {
		_autodetectedEncoding = detectCharset ();
		_wasEncodingDetected = true;
	}

	if (charset.isEmpty ())
		charset = _autodetectedEncoding;

	QFile data (getDocumentPath ());

	if (data.open (QFile::ReadOnly)) {
		QDataStream	in (&data);
		QByteArray	ba, bn;
		int		strPos;
		int		bytesRead;

		ba.resize (NSR_CORE_TEXT_PAGE_SIZE + NSR_CORE_TEXT_PAGE_SIZE / 2);
		in.device()->seek ((page - 1) * NSR_CORE_TEXT_PAGE_SIZE);

		if ((bytesRead = in.readRawData (ba.data (), NSR_CORE_TEXT_PAGE_SIZE + NSR_CORE_TEXT_PAGE_SIZE / 2)) == -1) {
			data.close ();
			return rinfo;
		}

		ba.truncate (bytesRead);
		bn = ba.left (NSR_CORE_TEXT_PAGE_SIZE);

		/* Complete last word */
		if (ba.size () > NSR_CORE_TEXT_PAGE_SIZE) {
			strPos = NSR_CORE_TEXT_PAGE_SIZE;
			while (strPos < ba.size () && !QChar(ba.at (strPos)).isSpace ())
				bn.append (ba.at (strPos++));
		}

		QTextCodec *codec = QTextCodec::codecForName (charset.toAscii ());

		_text = codec == NULL ? QString (bn) : codec->toUnicode (bn);

		if (!_text.isEmpty () && page > 1) {
			/* Remove previous semi-full words and spaces */
			strPos = -1;
			for (int i = 0; i < _text.size () / 2; ++i)
				if (_text.at(i).isSpace ()) {
					while (_text.at(++i).isSpace () && i < _text.size () / 2);
					strPos = i;
					break;
				}

			if (strPos != -1)
				_text = _text.right (_text.size () - strPos);
		}

		data.close ();
	}

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

	return info.exists();
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

QString NSRTextDocument::detectCharset ()
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
