#include "nsrpopplerdocument.h"
#include "nsrpagecropper.h"

#include "ErrorCodes.h"

#include <qmath.h>

#include <QFile>

#define NSR_CORE_PDF_MIN_ZOOM	25.0

int NSRPopplerDocument::_refcount = 0;
QMutex NSRPopplerDocument::_mutex;

NSRPopplerDocument::NSRPopplerDocument (const QString& file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_cachedPageSize (QSize (0, 0)),
	_doc (NULL),
	_catalog (NULL),
	_page (NULL),
	_dev (NULL),
	_cachedMinZoom (NSR_CORE_PDF_MIN_ZOOM),
	_cachedMaxZoom (100.0),
	_dpix (72),
	_dpiy (72),
	_readyForLoad (false)
{
	_mutex.lock ();

	if (_refcount == 0)
		globalParams = new GlobalParams ();

	++_refcount;
	_mutex.unlock ();

	if (!QFile::exists (file))
		return;

	createInternalDoc ();
}

NSRPopplerDocument::~NSRPopplerDocument ()
{
	if (_readyForLoad)
		_dev->startPage (0, NULL);

	if (_dev != NULL)
		delete _dev;

	if (_doc != NULL)
		delete _doc;

	_mutex.lock ();
	--_refcount;

	if (_refcount == 0)
		delete globalParams;

	_mutex.unlock ();
}


int
NSRPopplerDocument::getNumberOfPages () const
{
	if (_doc == NULL)
		return 0;

	return _doc->getNumPages ();
}

bool
NSRPopplerDocument::isValid () const
{
	return (_doc != NULL && _doc->isOk ());
}

void
NSRPopplerDocument::renderPage (int page)
{
	double dpix, dpiy;

	if (_doc == NULL || page > getNumberOfPages () || page < 1)
		return;

	_page = _catalog->getPage (page);

	if (isTextOnly ()) {
		PDFRectangle	*rect;
		GooString	*text;
		TextOutputDev	*dev;

		dev = new TextOutputDev (0, gFalse, 0, gFalse, gFalse);

		_doc->displayPageSlice (dev, _page->getNum (), 72, 72, 0, gFalse, gTrue, gFalse, -1, -1, -1, -1);

		rect = _page->getCropBox ();
		text = dev->getText (rect->x1, rect->y1, rect->x2, rect->y2);
		_text = processText (QString::fromUtf8 (text->getCString ()));

		delete text;
		delete dev;
		_readyForLoad = true;

		return;
	}

	double pageWidth = (getRotation () == 90 || getRotation () == 270) ? _page->getCropHeight ()
									   : _page->getCropWidth ();

	if (isZoomToWidth ()) {
		double wZoom = ((double) getScreenWidth () / pageWidth * 100.0);
		setZoomSilent (wZoom);
	}

	if (getZoom () < getMinZoom ())
		setZoomSilent (getMinZoom ());

	setZoomSilent (validateMaxZoom (QSize (_page->getCropWidth (), _page->getCropHeight ()), getZoom ()));

	if (_readyForLoad)
		_dev->startPage (0, NULL);

	dpix = _dpix * getZoom () / 100.0;
	dpiy = _dpiy * getZoom () / 100.0;

	_page->display (_dev, dpix, dpiy, getRotation (), gFalse, gFalse, gTrue, NULL, NULL, NULL, NULL);

	_readyForLoad = true;
}

double
NSRPopplerDocument::getMaxZoom ()
{
	if (_page == NULL)
		return 0;

	if (QSize (_page->getCropWidth (), _page->getCropHeight ()) == _cachedPageSize)
		return _cachedMaxZoom;

	/* Each pixel needs 4 bytes (RGBA) of memory */
	double pageSize = _page->getCropWidth () * _page->getCropHeight () * 4;
	_cachedPageSize = QSize (_page->getCropWidth (), _page->getCropHeight ());
	_cachedMaxZoom = (sqrt (NSR_CORE_DOCUMENT_MAX_HEAP * 72 * 72 / pageSize ) / 72 * 100 + 0.5);
	_cachedMaxZoom = validateMaxZoom (_cachedPageSize, _cachedMaxZoom);

	return _cachedMaxZoom;
}

double
NSRPopplerDocument::getMinZoom ()
{
	if (_page == NULL)
		return 0;

	if (QSize (_page->getCropWidth (), _page->getCropHeight ()) == _cachedPageSize)
		return _cachedMinZoom;

	/* Each pixel needs 4 bytes (RGBA) of memory */
	double pageSize = _page->getCropWidth () * _page->getCropHeight () * 4;

	if (pageSize > NSR_CORE_DOCUMENT_MAX_HEAP)
		_cachedMinZoom = getMaxZoom ();
	else
		_cachedMinZoom = (getMaxZoom () / 10) > NSR_CORE_PDF_MIN_ZOOM ? NSR_CORE_PDF_MIN_ZOOM
									      : getMaxZoom () / 10;

	_cachedPageSize = QSize (_page->getCropWidth (), _page->getCropHeight ());

	return _cachedMinZoom;
}

NSR_CORE_IMAGE_DATATYPE
NSRPopplerDocument::getCurrentPage ()
{
	if (!_readyForLoad)
		return NSR_CORE_IMAGE_DATATYPE ();

	SplashBitmap *bitmap = _dev->getBitmap ();
	int bw = bitmap->getWidth ();
	int bh = bitmap->getHeight ();

	char *dataPtr = (char *) _dev->getBitmap()->getDataPtr ();

	int rowBytes = bw * 3;
	while (rowBytes % 4)
		rowBytes += 1;

	NSRCropPads pads;

	if (isAutoCrop ())
		pads = NSRPageCropper::findCropPads ((unsigned char *) bitmap->getDataPtr (),
						     NSRPageCropper::NSR_PIXEL_ORDER_RGB,
						     bw, bh, rowBytes);

#ifdef Q_OS_BLACKBERRY
	bb::ImageData imgData (bb::PixelFormat::RGBX,
			       bw - pads.getLeft () - pads.getRight (),
			       bh - pads.getTop () - pads.getBottom ());

	unsigned char *addr = (unsigned char *) imgData.pixels ();
	int stride = imgData.bytesPerLine ();

	for (int i = pads.getTop (); i < bh - pads.getBottom (); ++i) {
		unsigned char *inAddr = (unsigned char *) (dataPtr + i * rowBytes);

		for (int j = pads.getLeft (); j < bw - pads.getRight (); ++j) {
			if (isInvertedColors ()) {
				unsigned char meanVal = (unsigned char) (((unsigned int) 255 * 3 - inAddr[j * 3 + 0] -
												   inAddr[j * 3 + 1] -
												   inAddr[j * 3 + 2]) / 3);

				addr[(j - pads.getLeft ()) * 4 + 0] = meanVal;
				addr[(j - pads.getLeft ()) * 4 + 1] = meanVal;
				addr[(j - pads.getLeft ()) * 4 + 2] = meanVal;
			} else {
				addr[(j - pads.getLeft ()) * 4 + 0] = inAddr[j * 3 + 0];
				addr[(j - pads.getLeft ()) * 4 + 1] = inAddr[j * 3 + 1];
				addr[(j - pads.getLeft ()) * 4 + 2] = inAddr[j * 3 + 2];
			}
		}

		addr += stride;
	}

	_dev->startPage (0, NULL);
	_readyForLoad = false;

	return imgData;
#else
	_readyForLoad = false;
	return NSR_CORE_IMAGE_DATATYPE ();
#endif
}

QString
NSRPopplerDocument::getText()
{
	if (!_readyForLoad)
		return NSRAbstractDocument::getText ();

	_readyForLoad = false;

	if (_text.isEmpty ())
		return NSRAbstractDocument::getText ();
	else
		return _text;
}

void
NSRPopplerDocument::setPassword (const QString &passwd)
{
	if (_doc != NULL)
		return;

	NSRAbstractDocument::setPassword (passwd);
	createInternalDoc (passwd);
}

bool
NSRPopplerDocument::isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const
{
	return (style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC ||
		style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
}

void
NSRPopplerDocument::createInternalDoc (QString passwd)
{
	SplashColor	bgColor;
	GooString	*fileName;
	GooString	*passwdStr;

	if (_doc != NULL)
		return;

	bgColor[0] = 255;
	bgColor[1] = 255;
	bgColor[2] = 255;
	_dev = new SplashOutputDev (splashModeRGB8, 4, gFalse, bgColor);

	fileName = new GooString (getDocumentPath().toUtf8().data ());

	if (!passwd.isEmpty ())
		passwdStr = new GooString (passwd.toUtf8().data ());
	else
		passwdStr = NULL;

	_doc = new PDFDoc (fileName, passwdStr);

	if (!_doc->isOk ()) {
		if (_doc->getErrorCode () == errEncrypted)
			setLastError (NSR_DOCUMENT_ERROR_PASSWD);
		else
			setLastError (NSR_DOCUMENT_ERROR_UNKNOWN);

		delete _doc;
		_doc = NULL;

		return;
	}

	_catalog = _doc->getCatalog ();
	_page = _catalog->getPage (1);
	_dev->startDoc (_doc);
}
