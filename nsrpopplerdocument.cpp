#include "nsrpopplerdocument.h"
#include "nsrpagecropper.h"

#include "poppler/poppler/ErrorCodes.h"

#include <qmath.h>

#include <QFile>

#define NSR_CORE_PDF_MIN_ZOOM	25.0

int NSRPopplerDocument::_refcount = 0;
QMutex NSRPopplerDocument::_mutex;

NSRPopplerDocument::NSRPopplerDocument (const QString& file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_doc (NULL),
	_catalog (NULL),
	_page (NULL),
	_dev (NULL)
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
NSRPopplerDocument::getPagesCount () const
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

NSRRenderInfo
NSRPopplerDocument::renderPage (int page)
{
	NSRRenderInfo	rinfo;
	double		dpix, dpiy;

	if (_doc == NULL || page > getPagesCount () || page < 1)
		return rinfo;

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

		rinfo.setSuccessRender (true);

		return rinfo;
	}

	bool isLandscape = (qAbs (_page->getRotate ()) % 180) == 90;

	double pageWidth = (((getRotation () % 180) == 90 && !isLandscape) ||
			    ((getRotation () % 180) == 0 && isLandscape)) ?
				_page->getCropHeight () : _page->getCropWidth ();

	double minZoom, maxZoom;

	/* Each pixel needs 4 bytes (RGBA) of memory */
	double pageSize = _page->getCropWidth () * _page->getCropHeight () * 4;

	maxZoom = qMin (sqrt (NSR_CORE_DOCUMENT_MAX_HEAP) * sqrt (72 * 72 / pageSize) / 72 * 100 + 0.5,
			getMaxZoom (QSize (_page->getCropWidth (), _page->getCropHeight ())));

	if (pageSize > NSR_CORE_DOCUMENT_MAX_HEAP)
		minZoom = maxZoom;
	else
		minZoom = (maxZoom / 10) > NSR_CORE_PDF_MIN_ZOOM ? NSR_CORE_PDF_MIN_ZOOM
								 : maxZoom / 10;

	if (isZoomToWidth ()) {
		int zoomWidth = getPageWidth ();

		if (isAutoCrop ()) {
			NSRCropPads pads = getCropPads ();
			pads.setRotation ((unsigned int) getRotation ());

			zoomWidth = (int) (zoomWidth / (1 - (pads.getLeft () + pads.getRight ())) + 0.5);
		}

		double wZoom = ((double) zoomWidth / pageWidth * 100.0);
		setZoomSilent (wZoom);
	}

	if (getZoom () < minZoom)
		setZoomSilent (minZoom);

	setZoomSilent (validateMaxZoom (QSize (_page->getCropWidth (), _page->getCropHeight ()), getZoom ()));

	_dev->startPage (0, NULL, _doc->getXRef ());

	dpix = 72.0 * getZoom () / 100.0;
	dpiy = 72.0 * getZoom () / 100.0;

	_page->display (_dev, dpix, dpiy, (int) getRotation (), gFalse, gFalse, gTrue, NULL, NULL, NULL, NULL);

	rinfo.setMinZoom (minZoom);
	rinfo.setMaxZoom (maxZoom);
	rinfo.setSuccessRender (true);

	return rinfo;
}

NSR_CORE_IMAGE_DATATYPE
NSRPopplerDocument::getCurrentPage ()
{
	if (_dev == NULL || (_dev->getBitmapHeight () == 1 && _dev->getBitmapWidth () == 1))
		return NSR_CORE_IMAGE_DATATYPE ();

	SplashBitmap *bitmap = _dev->getBitmap ();
	int bw = bitmap->getWidth ();
	int bh = bitmap->getHeight ();

	char *dataPtr = (char *) _dev->getBitmap()->getDataPtr ();

	int rowBytes = bw * 3;
	while (rowBytes % 4)
		rowBytes += 1;

	NSRCropPads pads = isAutoCrop () ? getCropPads () : NSRCropPads ();
	pads.setRotation ((unsigned int) getRotation ());

	int top    = (int) (bh * pads.getTop () + 0.5);
	int bottom = (int) (bh * pads.getBottom () + 0.5);
	int left   = (int) (bw * pads.getLeft () + 0.5);
	int right  = (int) (bw * pads.getRight () + 0.5);

#ifdef Q_OS_BLACKBERRY
	bb::ImageData imgData (bb::PixelFormat::RGBX,
			       bw - left - right,
			       bh - top - bottom);

	unsigned char *addr = (unsigned char *) imgData.pixels ();
	int stride = imgData.bytesPerLine ();

	for (int i = top; i < bh - bottom; ++i) {
		unsigned char *inAddr = (unsigned char *) (dataPtr + i * rowBytes);

		for (int j = left; j < bw - right; ++j) {
			if (isInvertedColors ()) {
				unsigned char meanVal = (unsigned char) (((unsigned int) 255 * 3 - inAddr[j * 3 + 0] -
												   inAddr[j * 3 + 1] -
												   inAddr[j * 3 + 2]) / 3);

				addr[(j - left) * 4 + 0] = meanVal;
				addr[(j - left) * 4 + 1] = meanVal;
				addr[(j - left) * 4 + 2] = meanVal;
			} else {
				addr[(j - left) * 4 + 0] = inAddr[j * 3 + 0];
				addr[(j - left) * 4 + 1] = inAddr[j * 3 + 1];
				addr[(j - left) * 4 + 2] = inAddr[j * 3 + 2];
			}
		}

		addr += stride;
	}

	_dev->startPage (0, NULL, _doc->getXRef ());

	return imgData;
#else
	return NSR_CORE_IMAGE_DATATYPE ();
#endif
}

QString
NSRPopplerDocument::getText ()
{
	if (_text.isEmpty ())
		return NSRAbstractDocument::getText ();
	else {
		QString ret = _text;
		_text.clear ();

		return ret;
	}
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
		delete _dev;
		_dev = NULL;
		_doc = NULL;

		return;
	}

	_catalog = _doc->getCatalog ();
	_page = _catalog->getPage (1);
	_dev->startDoc (_doc);
}
