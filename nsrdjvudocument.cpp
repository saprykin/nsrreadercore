#include "nsrdjvudocument.h"
#include "nsrpagecropper.h"

#include <djvu/DjVuImage.h>
#include <djvu/DjVuText.h>
#include <djvu/GBitmap.h>
#include <djvu/ByteStream.h>
#include <djvu/IFFByteStream.h>

#include <qmath.h>

#include <QString>

#define NSR_CORE_DJVU_MIN_ZOOM	25.0
#define NSR_CORE_DJVU_MAX_ZOOM	600.0

/* DjVu stuff for text extraction */
#define S(n,s) (n = miniexp_symbol(s))

static struct zone_names_s {
	const char *name;
	DjVuTXT::ZoneType ztype;
	char separator;
} zone_names[] = {
		 {"page",	DjVuTXT::PAGE,		0},
		 {"column",	DjVuTXT::COLUMN,	DjVuTXT::end_of_column},
		 {"region",	DjVuTXT::REGION,	DjVuTXT::end_of_region},
		 {"para",	DjVuTXT::PARAGRAPH,	DjVuTXT::end_of_paragraph},
		 {"line",	DjVuTXT::LINE,		DjVuTXT::end_of_line},
		 {"word",	DjVuTXT::WORD,		' '},
		 {"char",	DjVuTXT::CHARACTER,	0},
		 { 0, (DjVuTXT::ZoneType) 0,		0}
};

static bool
miniexp_get_int (miniexp_t &r, int &x)
{
	if (! miniexp_numberp (miniexp_car (r)))
		return false;
	x = miniexp_to_int (miniexp_car (r));
	r = miniexp_cdr (r);
	return true;
}

static bool
miniexp_get_rect_from_points (miniexp_t &r, QRect &rect)
{
	int x1, y1, x2, y2;

	if (!(miniexp_get_int (r, x1) && miniexp_get_int (r, y1) &&
	      miniexp_get_int (r, x2) && miniexp_get_int (r, y2)))
		return false;

	if (x2 < x1 || y2 < y1)
		return false;

	rect.setCoords (x1, y1, x2, y2);
	return true;
}

static void
flatten_hiddentext_sub (miniexp_t p, minivar_t &d)
{
	QRect		rect;
	miniexp_t	type = miniexp_car (p);
	miniexp_t	r = miniexp_cdr (p);

	if (miniexp_symbolp (type) && miniexp_get_rect_from_points (r, rect)) {
		if (miniexp_stringp (miniexp_car (r)))
			d = miniexp_cons (p, d);
		else {
			while (miniexp_consp (r)) {
				flatten_hiddentext_sub (miniexp_car (r), d);
				r = miniexp_cdr (r);
			}

			d = miniexp_cons (type, d);
		}
	}
}

/*
 * Output list contains
 * - terminals of the hidden text tree
 *   (keyword x1 y1 x2 y2 string)
 * - or a keyword symbol indicating the need for a separator
 *   page,column,region,para,line,word
 */
static miniexp_t
flatten_hiddentext (miniexp_t p)
{
	minivar_t d;

	flatten_hiddentext_sub (p, d);
	d = miniexp_reverse (d);

	return d;
}

static miniexp_t
pagetext_sub (const GP<DjVuTXT> &txt, DjVuTXT::Zone &zone, DjVuTXT::ZoneType detail)
{
	int zinfo;

	for (zinfo = 0; zone_names[zinfo].name; zinfo++)
		if (zone.ztype == zone_names[zinfo].ztype)
			break;

	minivar_t p;
	minivar_t a;

	bool gather = zone.children.isempty ();

	for (GPosition pos=zone.children; pos; ++pos)
		if (zone.children[pos].ztype > detail)
			gather = true;

	if (gather) {
		const char *data = (const char *) (txt->textUTF8) + zone.text_start;
		int length = zone.text_length;

		if (length > 0 && data[length-1] == zone_names[zinfo].separator)
			length -= 1;

		a = miniexp_substring (data, length);
		p = miniexp_cons (a, p);
	} else {
		for (GPosition pos=zone.children; pos; ++pos) {
			a = pagetext_sub (txt, zone.children[pos], detail);
			p = miniexp_cons (a, p);
		}
	}

	p = miniexp_reverse (p);
	const char *s = zone_names[zinfo].name;

	if (s) {
		p = miniexp_cons (miniexp_number (zone.rect.ymax), p);
		p = miniexp_cons (miniexp_number (zone.rect.xmax), p);
		p = miniexp_cons (miniexp_number (zone.rect.ymin), p);
		p = miniexp_cons (miniexp_number (zone.rect.xmin), p);
		p = miniexp_cons (miniexp_symbol (s), p);
		return p;
	}

	return miniexp_nil;
}

static void
fmt_convert_bitmap (GBitmap *bm, char *buffer, int rowsize)
{
	int h = bm->rows ();
	int m = bm->get_grays ();
	const GPixel wh = GPixel::WHITE;

	unsigned char g[256][4];

	for (int i = 0; i < m; i++) {
		g[i][0] = wh.b - (i * wh.b + (m - 1) / 2) / (m - 1);
		g[i][1] = wh.g - (i * wh.g + (m - 1) / 2) / (m - 1);
		g[i][2] = wh.r - (i * wh.r + (m - 1) / 2) / (m - 1);
		g[i][3] = (5 * g[i][2] + 9 * g[i][1] + 2 * g[i][0]) >> 4;
	}

	for (int i = m; i < 256; i++)
		g[i][0] = g[i][1] = g[i][2] = g[i][3] = 0;

	for (int r = h - 1; r >= 0; r--, buffer += rowsize) {
		int w = bm->columns ();
		unsigned char *p = (*bm)[r];
		char *pbuf = buffer;

		while (--w >= 0) {
			pbuf[0] = g[*p][0];
			pbuf[1] = g[*p][1];
			pbuf[2] = g[*p][2];
			pbuf += 3;
			p += 1;
		}
	}
}

static void
fmt_convert_pixmap (GPixmap *pm, char *buffer, int rowsize)
{
	int w = pm->columns ();
	int h = pm->rows ();

	for (int r = h - 1; r >= 0; r--, buffer += rowsize)
		memcpy (buffer, (*pm)[r], w * 3);
}

/*
 * OK, we are using color render mode and BGR24 pixel format
 */
NSRDjVuDocument::NSRDjVuDocument (const QString& file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_cachedPageSize (QSize (0, 0)),
	_cachedMinZoom (NSR_CORE_DJVU_MIN_ZOOM),
	_cachedMaxZoom (100.0),
	_cachedResolution (72),
	_pageCount (0),
	_imgData (NULL)
{
	GURL url = GURL::Filename::UTF8 (file.toUtf8().data ());

	_cache = DjVuFileCache::create ();
	_doc = DjVuDocument::create_wait (url, NULL, _cache);

	if (_doc != NULL)
		_pageCount = _doc->get_pages_num ();
}

NSRDjVuDocument::~NSRDjVuDocument ()
{
	if (_imgData != NULL)
		delete [] _imgData;
}

int
NSRDjVuDocument::getPagesCount () const
{
	return _pageCount;
}

bool
NSRDjVuDocument::isValid () const
{
	return (_doc != NULL);
}

void
NSRDjVuDocument::renderPage (int page)
{
	int	rot;
	int	tmp;
	double	resFactor;

	if (_doc == NULL || page > getPagesCount () || page < 1)
		return;

	clearRenderedData ();

	if (isTextOnly ()) {
		QString		ans;
		miniexp_t	seps[7];
		miniexp_t	ptext;
		int		separator;

		S(seps[0], zone_names[0].name);
		S(seps[1], zone_names[1].name);
		S(seps[2], zone_names[2].name);
		S(seps[3], zone_names[3].name);
		S(seps[4], zone_names[4].name);
		S(seps[5], zone_names[5].name);
		S(seps[6], zone_names[6].name);

		GP<DjVuFile> file = _doc->get_djvu_file (page - 1);

		if (file != NULL) {
			GP<ByteStream> bs = file->get_text ();

			if (bs != NULL) {
				GP<DjVuText> text = DjVuText::create ();
				text->decode (bs);

				GP<DjVuTXT> txt = text->txt;

				if (txt != NULL)
					ptext = pagetext_sub (txt, txt->page_zone, DjVuTXT::CHARACTER);
			}
		}

		ptext = flatten_hiddentext (ptext);
		separator = 6;

		while (miniexp_consp (ptext)) {
			QRect		rect;
			miniexp_t	r = miniexp_car (ptext);
			miniexp_t	type = r;

			ptext = miniexp_cdr (ptext);

			if (miniexp_consp (r)) {
				type = miniexp_car(r);
				r = miniexp_cdr(r);

				if (miniexp_symbolp (type) && miniexp_get_rect_from_points (r, rect)) {
					if (!ans.isEmpty ()) {
						if (separator == 0)
							ans += "\n\f";
						else if (separator <= 4)
							ans += "\n";
						else if (separator <= 5)
							ans += " ";
					}
					separator = 6;
					ans += QString::fromUtf8 (miniexp_to_str (miniexp_car (r)));
				}
			}

			for (int s = separator - 1; s >= 0; s--)
				if (type == seps[s])
					separator = s;
		}

		_text = processText (ans);
		return;
	}

	GP<DjVuImage> img = _doc->get_page (page - 1, true, NULL);

	if (img == NULL)
		return;

	int width = img->get_width ();
	int height = img->get_height ();

	switch (getRotation ()) {
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0:
		rot = 0;
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_90:
		rot = 3;
		tmp = width;
		width = height;
		height = tmp;
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_180:
		rot = 2;
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270:
		rot = 1;
		tmp = width;
		width = height;
		height = tmp;
		break;
	default:
		rot = 0;
		break;
	}

	if (img->get_info () != NULL)
		img->set_rotate (rot);

	_cachedPageSize = QSize (width, height);
	_cachedResolution = img->get_dpi ();

	resFactor = 72.0 / _cachedResolution;

	if (isZoomToWidth ()) {
		double wZoom = ((double) getPageWidth () / ((double) width * resFactor) * 100.0);
		setZoomSilent (wZoom);
	}

	if (getZoom () < getMinZoom ())
		setZoomSilent (getMinZoom ());

	setZoomSilent (validateMaxZoom (_cachedPageSize * resFactor, getZoom ()));

	GRect grect (0, 0,
		    (int) ((double) width * getZoom () / 100.0 * resFactor),
		    (int) ((double) height * getZoom () / 100.0 * resFactor));

	if (grect.xmax * grect.ymax * 3 > NSR_CORE_DOCUMENT_MAX_HEAP)
		return;

	_imgData = new char[grect.xmax * grect.ymax * 3];
	int rowSize = grect.xmax * 3;

	GP<GPixmap> pm;
	GP<GBitmap> bm;

	pm = img->get_pixmap (grect, grect, 2.2, GPixel::WHITE);

	if (pm == NULL)
		bm = img->get_bitmap (grect, grect);

	if (pm != NULL)
		fmt_convert_pixmap (pm, _imgData, rowSize);
	else if (bm != NULL)
		fmt_convert_bitmap (bm, _imgData, rowSize);
	else
		memset (_imgData, 0xFF, rowSize * grect.ymax);

	_imgSize = QSize (grect.xmax, grect.ymax);
}

double
NSRDjVuDocument::getMaxZoom ()
{
	if (_doc == NULL)
		return 0;

	if (_cachedPageSize == QSize (0, 0))
		return NSR_CORE_DJVU_MAX_ZOOM;

	/* Each pixel needs 3 bytes (RGB) of memory */
	double resFactor = 72.0 / _cachedResolution;
	double pageSize = _cachedPageSize.width () * _cachedPageSize.height () * 3 * resFactor / 4;
	_cachedMaxZoom = (sqrt (NSR_CORE_DOCUMENT_MAX_HEAP * 72 * 72 / pageSize) / 72 * 100 + 0.5);
	_cachedMaxZoom = validateMaxZoom (_cachedPageSize * resFactor, _cachedMaxZoom);

	if (_cachedMaxZoom > NSR_CORE_DJVU_MAX_ZOOM)
		_cachedMaxZoom = NSR_CORE_DJVU_MAX_ZOOM;

	return _cachedMaxZoom;
}

double
NSRDjVuDocument::getMinZoom ()
{
	if (_cachedPageSize == QSize (0, 0))
		return NSR_CORE_DJVU_MIN_ZOOM;

	/* Each pixel needs 3 bytes (RGB) of memory */
	double pageSize = _cachedPageSize.width () * _cachedPageSize.height ()
			  * 3 * 72 / _cachedResolution;

	if (pageSize > NSR_CORE_DOCUMENT_MAX_HEAP)
		_cachedMinZoom = getMaxZoom ();
	else
		_cachedMinZoom = (getMaxZoom () / 10) > NSR_CORE_DJVU_MIN_ZOOM ? NSR_CORE_DJVU_MIN_ZOOM
									       : getMaxZoom () / 10;

	return _cachedMinZoom;
}

NSR_CORE_IMAGE_DATATYPE
NSRDjVuDocument::getCurrentPage ()
{
	if (_imgData == NULL)
		return NSR_CORE_IMAGE_DATATYPE ();

	NSRCropPads pads;

	if (isAutoCrop ())
		pads = NSRPageCropper::findCropPads ((unsigned char *) _imgData,
						     NSRPageCropper::NSR_PIXEL_ORDER_BGR,
						     _imgSize.width (),
						     _imgSize.height (),
						     _imgSize.width () * 3,
						     !isZoomToWidth () ? getPageWidth () : 0);

#ifdef Q_OS_BLACKBERRY
	bb::ImageData imgData (bb::PixelFormat::RGBX,
			       _imgSize.width () - pads.getLeft () - pads.getRight (),
			       _imgSize.height () - pads.getTop () - pads.getBottom ());

	int rowSize = imgData.bytesPerLine ();
	unsigned char *image = imgData.pixels ();

	for (int i = pads.getTop (); i < _imgSize.height() - pads.getBottom (); ++i)
		for (int j = pads.getLeft (); j < _imgSize.width() - pads.getRight (); ++j) {
			if (isInvertedColors ()) {
				unsigned char meanVal = (unsigned char) (((unsigned int) 255 * 3 - *(_imgData + i * _imgSize.width () * 3 + j * 3 + 2) -
												   *(_imgData + i * _imgSize.width () * 3 + j * 3 + 2) -
												   *(_imgData + i * _imgSize.width () * 3 + j * 3)) / 3);

				*(image + rowSize * (i - pads.getTop ()) + (j - pads.getLeft ()) * 4) = meanVal;
				*(image + rowSize * (i - pads.getTop ()) + (j - pads.getLeft ()) * 4 + 1) = meanVal;
				*(image + rowSize * (i - pads.getTop ()) + (j - pads.getLeft ()) * 4 + 2) = meanVal;
			} else {
				*(image + rowSize * (i - pads.getTop ()) + (j - pads.getLeft ()) * 4) =
						*(_imgData + i * _imgSize.width () * 3 + j * 3 + 2);
				*(image + rowSize * (i - pads.getTop ()) + (j - pads.getLeft ()) * 4 + 1) =
						*(_imgData + i * _imgSize.width () * 3 + j * 3 + 1);
				*(image + rowSize * (i - pads.getTop ()) + (j - pads.getLeft ()) * 4 + 2) =
						*(_imgData + i * _imgSize.width () * 3 + j * 3);
			}
		}

	clearRenderedData ();

	return imgData;
#else
	return NSR_CORE_IMAGE_DATATYPE ();
#endif
}

QString
NSRDjVuDocument::getText ()
{
	if (_text.isEmpty ())
		return NSRAbstractDocument::getText ();
	else {
		QString ret = _text;
		_text.clear ();

		return ret;
	}
}

bool
NSRDjVuDocument::isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const
{
	return (style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC ||
		style == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);
}

QSize
NSRDjVuDocument::getPageSize (int page)
{
	int h = 0, w = 0;

	if (page < 1)
		return QSize (0, 0);

	GP<DjVuFile> file = _doc->get_djvu_file (page - 1);

	if (file == NULL)
		return QSize (0, 0);

	const GP<ByteStream> pbs (file->get_djvu_bytestream (false, false));
	const GP<IFFByteStream> iff (IFFByteStream::create (pbs));

	GUTF8String chkid;

	if (!iff->get_chunk (chkid))
		return QSize (0, 0);

	if (chkid == "FORM:DJVU") {
		while (iff->get_chunk (chkid) && chkid != "INFO")
			iff->close_chunk ();

		if (chkid == "INFO") {
			GP<ByteStream> gbs = iff->get_bytestream ();
			GP<DjVuInfo> dinfo = DjVuInfo::create ();

			dinfo->decode (*gbs);
			int rot = dinfo->orientation;

			w = (rot&1) ? dinfo->height : dinfo->width;
			h = (rot&1) ? dinfo->width : dinfo->height;
		}
	} else if (chkid == "FORM:BM44" || chkid == "FORM:PM44") {
		while (iff->get_chunk (chkid) && chkid != "BM44" && chkid != "PM44")
			iff->close_chunk ();

		if (chkid == "BM44" || chkid == "PM44") {
			GP<ByteStream> gbs = iff->get_bytestream ();

			if (gbs->read8 () == 0) {
				gbs->read24 ();
				unsigned char xhi = gbs->read8 ();
				unsigned char xlo = gbs->read8 ();
				unsigned char yhi = gbs->read8 ();
				unsigned char ylo = gbs->read8 ();

				w = (xhi << 8) + xlo;
				h = (yhi << 8) + ylo;
			}
		}
	}

	return QSize (w * 72 / _cachedResolution, h * 72 / _cachedResolution);
}

void
NSRDjVuDocument::clearRenderedData ()
{
	if (_imgData != NULL) {
		delete [] _imgData;
		_imgData = NULL;
		_imgSize = QSize (0, 0);
	}
}
