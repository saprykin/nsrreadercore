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
		QByteArray	ba;
		QString		text, fullText;
		int		strPos;
		int		bytesRead;

		QTextCodec *codec = QTextCodec::codecForName (charset.toAscii ());

		if (page > 1) {
			ba.resize (NSR_CORE_TEXT_PAGE_SIZE + NSR_CORE_TEXT_PAGE_SIZE / 2);
			in.device()->seek ((page - 2) * NSR_CORE_TEXT_PAGE_SIZE + NSR_CORE_TEXT_PAGE_SIZE / 2);
		} else {
			ba.resize (NSR_CORE_TEXT_PAGE_SIZE);
			in.device()->seek (0);
		}

		if ((bytesRead = in.readRawData (ba.data (), ba.size ())) == -1) {
			data.close ();
			return rinfo;
		}

		data.close ();
		ba.truncate (bytesRead);

		fullText = (codec == NULL) ? QString (ba) : codec->toUnicode (ba);

		text = (codec == NULL) ? QString (ba.right (NSR_CORE_TEXT_PAGE_SIZE))
				       : codec->toUnicode (ba.right (NSR_CORE_TEXT_PAGE_SIZE));

		/* First and last characters may be broken as of multibyte charset */
		if (!text.isEmpty () && !text.at(0).isSpace ())
			text.remove (0, 1);

		if (!text.isEmpty () && !text.at(text.size () - 1).isSpace ())
			text.remove (text.size () - 1, 1);

		/* Complete last word */
		strPos = text.size () - 1;

		while (strPos >= 0 && !text.at(strPos).isSpace ())
			--strPos;

		while (strPos >= 0 && text.at(strPos).isSpace ())
			--strPos;

		if (strPos < 0)
			strPos = text.size () - 1;

		text = text.left (strPos + 1);

		fullText = fullText.left (fullText.lastIndexOf (text));

		/* Prepend semi-words at the start */
		strPos = fullText.size () - 1;

		while (strPos >= 0 && !fullText.at(strPos).isSpace ())
			--strPos;

		if (strPos < 0)
			strPos = fullText.size () + 1;

		text.prepend (fullText.right (fullText.size () - 1 - strPos));

		_text = text;
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
