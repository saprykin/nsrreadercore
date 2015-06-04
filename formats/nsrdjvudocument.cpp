#include "nsrdjvudocument.h"
#include "nsrpagecropper.h"
#include "text/nsrtextpage.h"

#include <djvu/DjVuImage.h>
#include <djvu/DjVuText.h>
#include <djvu/DjVuNavDir.h>
#include <djvu/DjVmNav.h>
#include <djvu/DjVmDir.h>
#include <djvu/DjVmDir0.h>
#include <djvu/DataPool.h>
#include <djvu/GBitmap.h>
#include <djvu/ByteStream.h>
#include <djvu/IFFByteStream.h>

#include <qmath.h>

#include <QString>
#include <QQueue>

#define NSR_CORE_DJVU_MIN_ZOOM	25.0

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

	for (GPosition pos = zone.children; pos; ++pos)
		if (zone.children[pos].ztype > detail)
			gather = true;

	if (gather) {
		const char *data = (const char *) (txt->textUTF8) + zone.text_start;
		int length = zone.text_length;

		if (length > 0 && data[length - 1] == zone_names[zinfo].separator)
			length -= 1;

		a = miniexp_substring (data, length);
		p = miniexp_cons (a, p);
	} else {
		for (GPosition pos = zone.children; pos; ++pos) {
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

static miniexp_t
outline_sub (const GP<DjVmNav>& nav, int& pos, int count)
{
	GP<DjVmNav::DjVuBookMark>	entry;
	minivar_t			p, q, s;

	while (count > 0 && pos < nav->getBookMarkCount ()) {
		nav->getBookMark (entry, pos++);
		q = outline_sub (nav, pos, entry->count);
		s = miniexp_string ((const char *) (entry->url));
		q = miniexp_cons (s, q);
		s = miniexp_string ((const char *) (entry->displayname));
		q = miniexp_cons (s, q);
		p = miniexp_cons (q, p);
		count--;
	}

	return miniexp_reverse (p);
}

/* OK, we are using color render mode and BGR24 pixel format */
NSRDjVuDocument::NSRDjVuDocument (const QString& file, QObject *parent) :
	NSRAbstractDocument (file, parent),
	_pagesCount (0),
	_imgData (NULL)
{
	GURL url = GURL::Filename::UTF8 (file.toUtf8().data ());

	_cache = DjVuFileCache::create ();
	_doc = DjVuDocument::create_wait (url, NULL, _cache);

	if (_doc != NULL)
		_pagesCount = _doc->get_pages_num ();
}

NSRDjVuDocument::~NSRDjVuDocument ()
{
	if (_imgData != NULL)
		delete [] _imgData;
}

int
NSRDjVuDocument::getPageCount () const
{
	return _pagesCount;
}

bool
NSRDjVuDocument::isValid () const
{
	return (_doc != NULL);
}

NSRRenderInfo
NSRDjVuDocument::renderPage (int page)
{
	NSRRenderInfo	rinfo;
	double		resFactor;
	int		rot;

	if (_doc == NULL || page > getPageCount () || page < 1)
		return rinfo;

	clearRenderedData ();

	GP<DjVuImage> img = _doc->get_page (page - 1, true, NULL);

	if (img == NULL)
		return rinfo;

	int width  = img->get_width ();
	int height = img->get_height ();

	if (isTextOnly ()) {
		NSRTextEntityList words = getPageText (page, QSize (width, height), "word");

		if (words.isEmpty ())
			words = getPageText (page, QSize (width, height), "line");

		NSRTextPage *textPage = new NSRTextPage (QSize (width, height),
							 getRotation (),
							 NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0,
							 words);

		qDeleteAll (words);
		textPage->correctTextOrder ();
		_text = textPage->text ();
		delete textPage;

		rinfo.setSuccessRender (true);

		return rinfo;
	}

	switch (getRotation ()) {
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0:
		rot = 0;
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_90:
		rot = 3;
		qSwap (width, height);
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_180:
		rot = 2;
		break;
	case NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270:
		rot = 1;
		qSwap (width, height);
		break;
	default:
		rot = 0;
		break;
	}

	if (img->get_info () != NULL)
		img->set_rotate (rot);

	resFactor = 72.0 / img->get_dpi ();

	double minZoom, maxZoom;

	/* Each pixel needs 3 bytes (RGB) of memory */
	double pageSize = width * height * 3 * resFactor;

	maxZoom = qMin (sqrt (NSR_CORE_DOCUMENT_MAX_HEAP) * sqrt (72 * 72 / pageSize * 4) / 72 * 100 + 0.5,
			getMaxZoom (QSize (width, height) * resFactor));

	if (pageSize > NSR_CORE_DOCUMENT_MAX_HEAP)
		minZoom = maxZoom;
	else
		minZoom = (maxZoom / 10) > NSR_CORE_DJVU_MIN_ZOOM ? NSR_CORE_DJVU_MIN_ZOOM
								  : maxZoom / 10;

	if (isZoomToWidth ()) {
		int zoomWidth = getPageWidth ();

		if (isAutoCrop ()) {
			NSRCropPads pads = getCropPads ();
			pads.setRotation ((unsigned int) getRotation ());

			zoomWidth = (int) (zoomWidth / (1 - (pads.getLeft () + pads.getRight ())) + 0.5);
		}

		double wZoom = ((double) zoomWidth / ((double) width * resFactor) * 100.0);
		setZoomSilent (wZoom);
	}

	if (getZoom () < minZoom)
		setZoomSilent (minZoom);

	setZoomSilent (validateMaxZoom (QSize (width, height) * resFactor, getZoom ()));

	GRect grect (0, 0,
		    (int) ((double) width * getZoom () / 100.0 * resFactor),
		    (int) ((double) height * getZoom () / 100.0 * resFactor));

	if (grect.xmax * grect.ymax * 3 > NSR_CORE_DOCUMENT_MAX_HEAP)
		return rinfo;

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

	rinfo.setMinZoom (minZoom);
	rinfo.setMaxZoom (maxZoom);
	rinfo.setSuccessRender (true);

	return rinfo;
}

NSR_CORE_IMAGE_DATATYPE
NSRDjVuDocument::getCurrentPage ()
{
	if (_imgData == NULL)
		return NSR_CORE_IMAGE_DATATYPE ();

	NSRCropPads pads = isAutoCrop () ? getCropPads () : NSRCropPads ();
	pads.setRotation ((unsigned int) getRotation ());

	int top    = (int) (_imgSize.height () * pads.getTop () + 0.5);
	int bottom = (int) (_imgSize.height () * pads.getBottom () + 0.5);
	int left   = (int) (_imgSize.width () * pads.getLeft () + 0.5);
	int right  = (int) (_imgSize.width () * pads.getRight () + 0.5);

#ifdef Q_OS_BLACKBERRY
	bb::ImageData imgData (bb::PixelFormat::RGBX,
			       _imgSize.width () - left - right,
			       _imgSize.height () - top - bottom);

	int rowSize = imgData.bytesPerLine ();
	unsigned char *image = imgData.pixels ();

	for (int i = top; i < _imgSize.height () - bottom; ++i)
		for (int j = left; j < _imgSize.width () - right; ++j) {
			if (isInvertedColors ()) {
				unsigned char meanVal = (unsigned char) (((unsigned int) 255 * 3 - *(_imgData + i * _imgSize.width () * 3 + j * 3 + 2) -
												   *(_imgData + i * _imgSize.width () * 3 + j * 3 + 2) -
												   *(_imgData + i * _imgSize.width () * 3 + j * 3)) / 3);

				*(image + rowSize * (i - top) + (j - left) * 4) = meanVal;
				*(image + rowSize * (i - top) + (j - left) * 4 + 1) = meanVal;
				*(image + rowSize * (i - top) + (j - left) * 4 + 2) = meanVal;
			} else {
				*(image + rowSize * (i - top) + (j - left) * 4) =
						*(_imgData + i * _imgSize.width () * 3 + j * 3 + 2);
				*(image + rowSize * (i - top) + (j - left) * 4 + 1) =
						*(_imgData + i * _imgSize.width () * 3 + j * 3 + 1);
				*(image + rowSize * (i - top) + (j - left) * 4 + 2) =
						*(_imgData + i * _imgSize.width () * 3 + j * 3);
			}
		}

	clearRenderedData ();

	return imgData;
#else
	Q_UNUSED (top);
	Q_UNUSED (bottom);
	Q_UNUSED (left);
	Q_UNUSED (right);
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

NSRTocEntry *
NSRDjVuDocument::getToc () const
{
	if (_doc == NULL)
		return NULL;

	GP<DjVmNav> nav = _doc->get_djvm_nav ();

	if (!nav)
		return NULL;

	int pos = 0;

	minivar_t result = outline_sub (nav, pos, nav->getBookMarkCount ());
	result = miniexp_cons (miniexp_symbol ("bookmarks"), result);

	NSRTocEntry *toc = NULL;

	if (miniexp_listp (result) && (miniexp_length (result) > 0) &&
	    miniexp_symbolp (miniexp_nth (0, result)) &&
	   (QString::fromUtf8 (miniexp_to_name (miniexp_nth (0, result))) == QLatin1String ("bookmarks"))) {
		toc = new NSRTocEntry (QString (), -1);
		addTocChildren (toc, result, 1);
	}

	return toc;
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

NSRTextEntityList
NSRDjVuDocument::getPageText (int page, const QSize& size, const QString& detail)
{
	NSRTextEntityList	ret;
	miniexp_t		ptext = miniexp_nil;

	if (_doc == NULL)
		return ret;

	GP<DjVuFile> file = _doc->get_djvu_file (page - 1);

	if (file != NULL) {
		GP<ByteStream> bs = file->get_text ();

		if (bs != NULL) {
			GP<DjVuText> text = DjVuText::create ();
			text->decode (bs);

			GP<DjVuTXT> txt = text->txt;

			if (txt != NULL) {
				DjVuTXT::ZoneType detailZone = DjVuTXT::CHARACTER;

				for (int i = 0; zone_names[i].name; i++)
					if (detail == QString (zone_names[i].name))
						detailZone = zone_names[i].ztype;

				ptext = pagetext_sub (txt, txt->page_zone, detailZone);
			}
		}
	}

	if (ptext == miniexp_nil)
		return ret;

	QQueue<miniexp_t> queue;
	queue.enqueue (ptext);

	while (!queue.isEmpty ()) {
		miniexp_t cur = queue.dequeue ();

		if (miniexp_listp (cur) && (miniexp_length (cur) > 0) &&
		    miniexp_symbolp (miniexp_nth (0, cur))) {
			int curLen    = miniexp_length (cur);
			QString sym = QString::fromUtf8 (miniexp_to_name (miniexp_nth (0, cur)));

			if (sym == detail) {
				if (curLen >= 6) {
					int xmin = miniexp_to_int (miniexp_nth (1, cur));
					int ymin = miniexp_to_int (miniexp_nth (2, cur));
					int xmax = miniexp_to_int (miniexp_nth (3, cur));
					int ymax = miniexp_to_int (miniexp_nth (4, cur));
					QRect rect (xmin, size.height () - ymax, xmax - xmin, ymax - ymin);

					ret.append (new NSRTextEntity (QString::fromUtf8 (miniexp_to_str (miniexp_nth (5, cur))),
								       new NSRNormalizedRect (rect, size.width (), size.height ())));
				}
			} else {
				for (int i = 5; i < curLen; ++i)
					queue.enqueue (miniexp_nth (i, cur));
			}
		}
	}

	return ret;
}

void
NSRDjVuDocument::addTocChildren (NSRTocEntry *parent, miniexp_t exp, int offset) const
{
	if (!miniexp_listp (exp))
		return;

	int l = miniexp_length (exp);

	for (int i = qMax (offset, 0); i < l; ++i) {
		miniexp_t cur = miniexp_nth (i, exp);

		if (miniexp_consp (cur) && (miniexp_length (cur) > 0) &&
		    miniexp_stringp (miniexp_nth (0, cur)) && miniexp_stringp (miniexp_nth (1, cur))) {
			QString title = QString::fromUtf8 (miniexp_to_str (miniexp_nth (0, cur)));
			QString dest  = QString::fromUtf8 (miniexp_to_str (miniexp_nth (1, cur)));

			NSRTocEntry *entry = new NSRTocEntry (title, -1);

			if (!dest.isEmpty ()) {
				if (dest.at (0) == QLatin1Char ('#')) {
					dest.remove (0, 1);
					bool isNumber = false;
					dest.toInt (&isNumber);

					if (isNumber) {
						/* It might be an actual page number, but could also be a page label,
						 * so resolve the number, and get the real page number */
						int pageNo = pageFromName (dest);

						if (pageNo != -1)
							entry->setPage (pageNo + 1);
						else
							entry->setPage (dest.toInt ());
					} else
						entry->setPage (pageFromName (dest));

					if (entry->getTitle().isEmpty ())
						entry->setTitle (dest);
				} else {
					entry->setUri (dest);
					entry->setExternal (true);

					if (entry->getTitle().isEmpty ())
						entry->setTitle (entry->getUri ());
				}
			}

			if (entry->getTitle().isEmpty ())
				delete entry;
			else {
				parent->appendChild (entry);

				if (miniexp_length (cur) > 2)
					addTocChildren (entry, cur, 2);
			}
		}
	}
}

int
NSRDjVuDocument::pageFromName (const QString& name) const
{
	if (_doc == NULL)
		return -1;

	int pageNo = _pageNamesCache.value (name, -1);

	if (pageNo != -1)
		return pageNo;

	QByteArray utfName = name.toUtf8 ();

	int docType = _doc->get_doc_type ();
	int fileNum = -1;

	if (docType == DjVuDocument::BUNDLED || docType == DjVuDocument::INDIRECT) {
		GP<DjVmDir> dir = _doc->get_djvm_dir ();
		fileNum = dir->get_files_num ();
	} else if (docType == DjVuDocument::OLD_BUNDLED) {
		GP<DjVmDir0> dir0 = _doc->get_djvm_dir0 ();
		fileNum = dir0->get_files_num ();
	} else
		fileNum = _doc->get_pages_num ();

	ddjvu_fileinfo_t info;

	for (int i = 0; i < fileNum; ++i) {
		if (!getPageFileInfo (i, &info))
			continue;

		if (info.type != 'P')
			continue;

		if ((utfName == info.id) || (utfName == info.name) || (utfName == info.title)) {
			_pageNamesCache.insert (name, info.pageno);
			return info.pageno;
		}
	}

	return -1;
}

bool
NSRDjVuDocument::getPageFileInfo (int fileno, ddjvu_fileinfo_t *info) const
{
	if (_doc == NULL || info == NULL)
		return false;

	ddjvu_fileinfo_t fileInfo;

	memset (info, 0, sizeof (*info));
	memset (&fileInfo, 0, sizeof (fileInfo));

	int type = _doc->get_doc_type ();

	if (type == DjVuDocument::BUNDLED || type == DjVuDocument::INDIRECT) {
		GP<DjVmDir> dir = _doc->get_djvm_dir ();
		GP<DjVmDir::File> file = dir->pos_to_file (fileno, &fileInfo.pageno);

		if (!file)
			return false;

		fileInfo.type = 'I';

		if (file->is_page ())
			fileInfo.type = 'P';
		else
			fileInfo.pageno = -1;

		if (file->is_thumbnails ())
			fileInfo.type = 'T';

		if (file->is_shared_anno ())
			fileInfo.type = 'S';

		fileInfo.size  = file->size;
		fileInfo.id    = file->get_load_name ();
		fileInfo.name  = file->get_save_name ();
		fileInfo.title = file->get_title ();

		memcpy (info, &fileInfo, sizeof (fileInfo));

		return true;
	} else if (type == DjVuDocument::OLD_BUNDLED) {
		GP<DjVmDir0> dir0  = _doc->get_djvm_dir0 ();
		GP<DjVuNavDir> nav = _doc->get_nav_dir ();

		GP<DjVmDir0::FileRec> frec = dir0->get_file (fileno);

		if (!frec)
			return false;

		fileInfo.size = frec->size;
		fileInfo.id   = (const char *) frec->name;
		fileInfo.name = fileInfo.title = fileInfo.id;

		if (!nav)
			return false;
		else if (nav->name_to_page (frec->name) >= 0)
			fileInfo.type = 'P';
		else
			fileInfo.type = 'I';

		memcpy (info, &fileInfo, sizeof (fileInfo));

		return true;
	} else {
		if (fileno < 0 || fileno >= _doc->get_pages_num ())
			return false;

		fileInfo.type   = 'P';
		fileInfo.pageno = fileno;
		fileInfo.size   = -1;

		GP<DjVuNavDir> nav = _doc->get_nav_dir ();

		fileInfo.id = (nav) ? (const char *) nav->page_to_name (fileno) : 0;
		fileInfo.name = fileInfo.title = fileInfo.id;

		GP<DjVuFile> file = _doc->get_djvu_file (fileno, true);
		GP<DataPool> pool;

		if (file)
			pool = file->get_init_data_pool ();

		if (pool)
			fileInfo.size = pool->get_length ();

		memcpy (info, &fileInfo, sizeof (fileInfo));

		return true;
	}
}
