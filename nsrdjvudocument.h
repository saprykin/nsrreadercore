#ifndef __NSRDJVUDOCUMENT_H__
#define __NSRDJVUDOCUMENT_H__

/**
 * @file nsrdjvudocument.h
 * @author Alexander Saprykin
 * @brief DjVu file handler
 */

#include "nsrabstractdocument.h"

#include <djvu/DjVuFileCache.h>
#include <djvu/DjVuDocument.h>
#include <djvu/ddjvuapi.h>
#include <djvu/miniexp.h>

/**
 * @class NSRDjVuDocument nsrdjvudocument.h
 * @brief Class for DjVu file handler
 */
class NSRDjVuDocument : public NSRAbstractDocument
{
	Q_OBJECT
	Q_ENUMS (NSRDjVuErrorType)
public:
	/** DjVu errors */
	enum NSRDjVuErrorType {
		NSR_DJVU_ERROR_NONE	= 0,	/**< No errors		*/
		NSR_DJVU_ERROR_FILENAME	= 1,	/**< Filename error	*/
		NSR_DJVU_ERROR_OTHER	= 2	/**< Unknown error	*/
	};

	/**
	 * @brief Constructor with parameters
	 * @param file Path to file.
	 * @param parent Parent object.
	 */
	NSRDjVuDocument (const QString& file, QObject *parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRDjVuDocument ();

	/* Reimplemented from NSRAbstractDocument */
	int getPagesCount () const;
	void renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	double getMaxZoom ();
	double getMinZoom ();
	QString getText ();
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;
	NSRAbstractDocument::NSRDocumentStyle getPreferredDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC;
	}

	QString getPassword () const {
		return QString ();
	}

	void setPassword (const QString& passwd) {
		Q_UNUSED (passwd);
	}

	void setEncoding (const QString& encoding) {
		Q_UNUSED (encoding);
	}

	QString getEncoding () const {
		return QString ("UTF-8");
	}

private:
	/**
	 * @brief Gets page size
	 * @param page Page number.
	 * @return Page size, in px.
	 */
	QSize getPageSize (int page);

	/** Clears rendered data */
	void clearRenderedData ();

	GP<DjVuFileCache>	_cache;			/**< File cache			*/
	GP<DjVuDocument>	_doc;			/**< DjVu file handler		*/
	QSize			_cachedPageSize;	/**< Cached page size		*/
	QSize			_imgSize;		/**< Rendered image size	*/
	QString			_text;			/**< Page text			*/
	double			_cachedMinZoom;		/**< Cached min zoom		*/
	double			_cachedMaxZoom;		/**< Cached max zoom		*/
	int			_cachedResolution;	/**< Cached page resolution	*/
	int			_pagesCount;		/**< Pages count		*/
	char			*_imgData;		/**< Rendered image data	*/
};

#endif /* __NSRDJVUDOCUMENT_H__ */
