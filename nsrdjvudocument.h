#ifndef NSRDJVUDOCUMENT_H
#define NSRDJVUDOCUMENT_H

#include "nsrabstractdocument.h"

#include <djvu/DjVuFileCache.h>
#include <djvu/DjVuDocument.h>
#include <djvu/ddjvuapi.h>
#include <djvu/miniexp.h>

enum NSRDjVuErrorType {
	NSR_DJVU_ERROR_NONE	= 0,
	NSR_DJVU_ERROR_FILENAME	= 1,
	NSR_DJVU_ERROR_OTHER	= 2
};

class NSRDjVuDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	NSRDjVuDocument (const QString& file, QObject *parent = 0);
	virtual ~NSRDjVuDocument ();
	int getNumberOfPages () const;
	void renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	double getMaxZoom ();
	double getMinZoom ();
	QString getText ();
	void setZoom (double zoom);
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;
	inline NSRAbstractDocument::NSRDocumentStyle getPrefferedDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC;
	}
	inline void setEncoding (const QString& encoding) {
		Q_UNUSED (encoding);
	}
	inline QString getEncoding () const {
		return QString ("UTF-8");
	}

private:
	QSize getPageSize (int page);
	void clearRenderedData ();

	GP<DjVuFileCache>	_cache;
	GP<DjVuDocument>	_doc;
	QSize			_cachedPageSize;
	QSize			_imgSize;
	QString			_text;
	double			_cachedMinZoom;
	double			_cachedMaxZoom;
	int			_cachedResolution;
	int			_pageCount;
	char			*_imgData;
};

#endif /* NSRDJVUDOCUMENT_H */
