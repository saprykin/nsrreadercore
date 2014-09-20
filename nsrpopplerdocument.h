#ifndef NSRPOPPLERDOCUMENT_H
#define NSRPOPPLERDOCUMENT_H

#include <QMutex>

#include "nsrabstractdocument.h"

#include "poppler/poppler/GlobalParams.h"
#include "poppler/poppler/PDFDoc.h"
#include "poppler/poppler/SplashOutputDev.h"
#include "poppler/poppler/TextOutputDev.h"
#include "poppler/poppler/OutputDev.h"
#include "poppler/splash/SplashTypes.h"
#include "poppler/splash/SplashBitmap.h"

class NSRPopplerDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	NSRPopplerDocument (const QString& file, QObject *parent = 0);
	virtual ~NSRPopplerDocument ();
	int getNumberOfPages () const;
	void renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	double getMaxZoom ();
	double getMinZoom ();
	QString getText ();
	void setPassword (const QString &passwd);
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
	void createInternalDoc (QString passwd = QString());

	static QMutex	_mutex;
	static int	_refcount;

	QSize		_cachedPageSize;
	QString		_text;
	PDFDoc		*_doc;
	Catalog		*_catalog;
	Page		*_page;
	SplashOutputDev *_dev;
	double		_cachedMinZoom;
	double		_cachedMaxZoom;
	int		_dpix;
	int		_dpiy;
};

#endif /* NSRPOPPLERDOCUMENT_H */
