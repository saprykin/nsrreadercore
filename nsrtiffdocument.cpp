#include "nsrtiffdocument.h"

#include <qmath.h>

#define NSR_CORE_TIFF_MIN_ZOOM	25.0

NSRTIFFDocument::NSRTIFFDocument (const QString& file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_tiff (NULL),
	_pagesCount (0),
	_cachedPage (0)
{
	if ((_tiff = TIFFOpen (file.toUtf8().data (), "r")) == NULL)
		return;

	_pagesCount = 1;

	while (TIFFReadDirectory (_tiff))
		++_pagesCount;
}

NSRTIFFDocument::~NSRTIFFDocument ()
{
	if (_tiff != NULL) {
		TIFFClose (_tiff);
		_tiff = NULL;
	}

	if (!_origImage.isNull ())
		_origImage = QImage ();

	if (!_image.isNull ())
		_image = QImage ();
}

int
NSRTIFFDocument::getPagesCount () const
{
	if (_tiff == NULL)
		return 0;

	return _pagesCount > 0 ? _pagesCount : 1;
}

bool
NSRTIFFDocument::isValid () const
{
	return (_tiff != NULL);
}

NSRRenderInfo
NSRTIFFDocument::renderPage (int page)
{
	NSRRenderInfo	rinfo;
	uint32		w = 0, h = 0;
	size_t		npixels;
	char *		imgBuf;
	QImage *	img;

	if (_tiff == NULL || page > getPagesCount () || page < 1)
		return rinfo;

	if (_pagesCount > 0 && TIFFSetDirectory (_tiff, page - 1) == 0)
		return rinfo;

	_image = QImage ();

	TIFFGetField (_tiff, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField (_tiff, TIFFTAG_IMAGELENGTH, &h);
	npixels = w * h;

	if (npixels * sizeof (uint32) > NSR_CORE_DOCUMENT_MAX_HEAP)
		return rinfo;

	double pageWidth = (getRotation () == NSRAbstractDocument::NSR_DOCUMENT_ROTATION_90 ||
			    getRotation () == NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270) ? h : w;

	double minZoom, maxZoom;

	/* Each pixel needs 4 bytes (RGBA) of memory */
	double pageSize = npixels * 4;

	maxZoom = qMin (sqrt (NSR_CORE_DOCUMENT_MAX_HEAP / (double) pageSize) * 100.0,
			getMaxZoom (QSize (w, h)));

	if (pageSize > NSR_CORE_DOCUMENT_MAX_HEAP)
		minZoom = maxZoom;
	else
		minZoom = (maxZoom / 10) > NSR_CORE_TIFF_MIN_ZOOM ? NSR_CORE_TIFF_MIN_ZOOM
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

	setZoomSilent (validateMaxZoom (QSize (w, h), getZoom ()));

	rinfo.setMinZoom (minZoom);
	rinfo.setMaxZoom (maxZoom);

	if (_cachedPage == page && !_origImage.isNull ()) {
		double scale = getZoom () / 100.0;
		QTransform trans;

		trans.scale (scale, scale);
		trans.rotate ((int) getRotation ());
		_image = _origImage.transformed (trans);

		rinfo.setSuccessRender (true);

		return rinfo;
	}

	_origImage = QImage ();
	imgBuf = new char[npixels * sizeof (uint32)];
	img = new QImage ((const uchar*) imgBuf, w, h, w * sizeof (uint32), QImage::Format_ARGB32);

	if (TIFFReadRGBAImageOriented (_tiff, w, h, (uint32 *) img->bits (), ORIENTATION_TOPLEFT, 0) == 0) {
		delete img;
		delete imgBuf;
	} else {
		uint32 orientationTag = 0;

		if (TIFFGetField (_tiff, TIFFTAG_ORIENTATION, &orientationTag) != 0) {
			switch (orientationTag) {
			case ORIENTATION_LEFTTOP:
				rotateRightMirrorHorizontal (&img, &imgBuf);
				break;
			case ORIENTATION_RIGHTTOP:
				rotateRightMirrorVertical (&img, &imgBuf);
				break;
			case ORIENTATION_RIGHTBOT:
				rotateRightMirrorHorizontal (&img, &imgBuf);
				break;
			case ORIENTATION_LEFTBOT:
				rotateRightMirrorVertical (&img, &imgBuf);
				break;
			default:
				break;
			}
		};

		if (orientationTag < ORIENTATION_LEFTTOP) {
			/* Convert from ABGR to ARGB pixel format */
			uint32 *dataPtr = reinterpret_cast<uint32 *> (img->bits ());

			for (uint32 row = 0; row < h; ++row)
				for (uint32 col = 0; col < w; ++col) {
					uint32 pxl  = *(dataPtr + row * w + col);
					*(dataPtr + row * w + col) = ((pxl & 0x000000FF) << 16) |
								     (pxl & 0xFF000000) |
								     ((pxl & 0x00FF0000) >> 16) |
								     (pxl & 0x0000FF00);
				}
		}

		double scale = getZoom () / 100.0;
		QTransform trans;

		trans.scale (scale, scale);
		trans.rotate ((int) getRotation ());

		if (_origImage.byteCount () > NSR_CORE_DOCUMENT_MAX_HEAP / (2 + scale * scale)) {
			_image = img->transformed (trans);
			_cachedPage = 0;

			delete img;
			delete imgBuf;
		} else {
			_origImage = img->copy ();

			delete img;
			delete imgBuf;

			_image = _origImage.transformed (trans);
			_cachedPage = page;
		}
	}

	rinfo.setSuccessRender (true);

	return rinfo;
}

NSR_CORE_IMAGE_DATATYPE
NSRTIFFDocument::getCurrentPage ()
{
	if (_image.isNull ())
		return NSR_CORE_IMAGE_DATATYPE ();

#ifdef Q_OS_BLACKBERRY
	NSRCropPads pads = isAutoCrop () ? getCropPads () : NSRCropPads ();
	pads.setRotation ((unsigned int) getRotation ());

	int top    = (int) (_image.height () * pads.getTop () + 0.5);
	int bottom = (int) (_image.height () * pads.getBottom () + 0.5);
	int left   = (int) (_image.width () * pads.getLeft () + 0.5);
	int right  = (int) (_image.width () * pads.getRight () + 0.5);

	bb::ImageData imgData (bb::PixelFormat::RGBX,
			       _image.width () - left - right,
			       _image.height () - top - bottom);

	unsigned char *addr = (unsigned char *) imgData.pixels ();
	int stride = imgData.bytesPerLine ();
	int bw = _image.width ();
	int bh = _image.height ();
	int rowBytes = _image.bytesPerLine ();
	unsigned char *dataPtr = _image.bits ();

	for (int i = top; i < bh - bottom; ++i) {
		unsigned char *inAddr = (unsigned char *) (dataPtr + i * rowBytes);

		for (int j = left; j < bw - right; ++j) {
			if (isInvertedColors ()) {
				unsigned char meanVal = (unsigned char) (((unsigned int) 255 * 3 - inAddr[j * 4 + 2] -
												   inAddr[j * 4 + 1] -
												   inAddr[j * 4 + 0]) / 3);

				addr[(j - left) * 4 + 0] = meanVal;
				addr[(j - left) * 4 + 1] = meanVal;
				addr[(j - left) * 4 + 2] = meanVal;
			} else {
				addr[(j - left) * 4 + 0] = inAddr[j * 4 + 2];
				addr[(j - left) * 4 + 1] = inAddr[j * 4 + 1];
				addr[(j - left) * 4 + 2] = inAddr[j * 4 + 0];
			}
		}

		addr += stride;
	}

	_image = QImage ();
	return imgData;
#else
	return NSR_CORE_IMAGE_DATATYPE ();
#endif
}

bool
NSRTIFFDocument::isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const
{
	return (style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC);
}

/* The following code was taken from the Qt TIFF handler plugin with some modifications */
void
NSRTIFFDocument::rotateRightMirrorHorizontal (QImage** const image, char **buf)
{
	const int height = (*image)->height ();
	const int width = (*image)->width ();

	char *newBuf = new char[height * width * sizeof (uint32)];
	QImage *generated = new QImage ((const uchar *) newBuf, height, width, height * sizeof (uint32), (*image)->format ());
	const uint32 *pixel = reinterpret_cast<const uint32 *> ((*image)->bits ());
	uint32 *const generatedBits = reinterpret_cast<uint32 *> (generated->bits ());

	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			int idx = col * height + row;
			/* Also convert from ABGR to ARGB */
			generatedBits[idx] = ((*pixel & 0x000000FF) << 16) |
					     (*pixel & 0xFF000000) |
					     ((*pixel & 0x00FF0000) >> 16) |
					     (*pixel & 0x0000FF00);
			++pixel;
		}
	}

	delete *image;
	delete *buf;

	*image = generated;
	*buf = newBuf;
}

void
NSRTIFFDocument::rotateRightMirrorVertical (QImage** const image, char **buf)
{
	const int height = (*image)->height ();
	const int width = (*image)->width ();

	char *newBuf = new char[height * width * sizeof (uint32)];
	QImage *generated = new QImage ((const uchar *) newBuf, height, width, height * sizeof (uint32), (*image)->format ());
	const int lastCol = width - 1;
	const int lastRow = height - 1;

	const uint32 *pixel = reinterpret_cast<const uint32 *> ((*image)->bits ());
	uint32 *const generatedBits = reinterpret_cast<uint32 *> (generated->bits ());

	for (int row = 0; row < height; ++row) {
		for (int col = 0; col < width; ++col) {
			int idx = (lastCol - col) * height + (lastRow - row);
			/* Also convert from ABGR to ARGB */
			generatedBits[idx] = ((*pixel & 0x000000FF) << 16) |
					     (*pixel & 0xFF000000) |
					     ((*pixel & 0x00FF0000) >> 16) |
					     (*pixel & 0x0000FF00);
			++pixel;
		}
	}

	delete *image;
	delete *buf;

	*image = generated;
	*buf = newBuf;
}
