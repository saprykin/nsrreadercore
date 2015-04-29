#include "nsrabstractdocument.h"

#include <QTextCodec>

#ifdef Q_OS_BLACKBERRY
#  define NSR_CORE_MAX_PAGE_WIDTH	4000
#  define NSR_CORE_MAX_PAGE_HEIGHT	4000
#else
#  define NSR_CORE_MAX_PAGE_WIDTH	2000
#  define NSR_CORE_MAX_PAGE_HEIGHT	2000
#endif

NSRAbstractDocument::NSRAbstractDocument (const QString& file, QObject *parent) :
	QObject (parent),
	_docPath (file),
	_encoding ("UTF-8"),
	_zoom (100.0),
	_pageWidth (360),
	_zoomToWidth (false),
	_textOnly (false),
	_invertedColors (false),
	_autoCrop (false),
	_lastError (NSR_DOCUMENT_ERROR_NO),
	_rotation (NSR_DOCUMENT_ROTATION_0)
{
}

NSRAbstractDocument::~NSRAbstractDocument ()
{
}

void
NSRAbstractDocument::rotateLeft ()
{
	_rotation = (_rotation == NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0) ?
		    NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270 :
		    (NSRAbstractDocument::NSRDocumentRotation) (((int) _rotation) - 90);
}

void
NSRAbstractDocument::rotateRight ()
{
	_rotation = (_rotation == NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270) ?
		    NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0 :
		    (NSRAbstractDocument::NSRDocumentRotation) (((int) _rotation) + 90);
}

void
NSRAbstractDocument::setRotation (NSRAbstractDocument::NSRDocumentRotation rotation)
{
	switch (rotation) {
		case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0:
		case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_90:
		case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_180:
		case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270:
			_rotation = rotation;
			break;
		default:
			_rotation = NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0;
			break;
	}
}

void
NSRAbstractDocument::setEncoding (const QString &enc)
{
	/* Empty encoding means we will use autodetection */
	if (!enc.isEmpty () && QTextCodec::codecForName (enc.toAscii ()) == NULL)
		return;
	else
		_encoding = enc;
}

void
NSRAbstractDocument::setZoom (double zoom)
{
	_zoom = zoom;
	_zoomToWidth = false;
}

double
NSRAbstractDocument::validateMaxZoom (const QSize& pageSize, double zoom) const
{
	if (pageSize.width () * zoom / 100.0 <= NSR_CORE_MAX_PAGE_WIDTH &&
	    pageSize.height () * zoom / 100.0 <= NSR_CORE_MAX_PAGE_HEIGHT)
		return zoom;

	return getMaxZoom (pageSize);
}

double NSRAbstractDocument::getMaxZoom (const QSize &pageSize) const
{
	double scale = qMin (NSR_CORE_MAX_PAGE_WIDTH / (double) pageSize.width (),
			     NSR_CORE_MAX_PAGE_HEIGHT / (double) pageSize.height ());

	return scale * 100.0;
}
