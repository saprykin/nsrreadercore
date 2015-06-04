#ifndef __NSRDJVUDOCUMENT_H__
#define __NSRDJVUDOCUMENT_H__

/**
 * @file nsrdjvudocument.h
 * @author Alexander Saprykin
 * @brief DjVu file handler
 */

#include "nsrabstractdocument.h"
#include "text/nsrtextentity.h"

#include <djvu/DjVuFileCache.h>
#include <djvu/DjVuDocument.h>
#include <djvu/ddjvuapi.h>
#include <djvu/miniexp.h>

#include <QHash>

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
	int getPageCount () const;
	NSRRenderInfo renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	QString getText ();
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;
	NSRAbstractDocument::NSRDocumentStyle getPreferredDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC;
	}

	bool hasDynamicPages () const {
		return false;
	}

	NSRTocEntry * getToc () const;

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
	/** Clears rendered data */
	void clearRenderedData ();

	/**
	 * @brief Gets page text with given detail level
	 * @param page Page number, from 1.
	 * @param detail Detail level.
	 * @return List of text enities from the page.
	 *
	 * The caller takes ownership for the words within returned list.
	 *
	 * Detail level can be selected from one of the following:
	 * -page;
	 * -column;
	 * -region;
	 * -para;
	 * -line;
	 * -word;
	 * -char.
	 */
	NSRTextEntityList getPageText (int page, const QSize& size, const QString& detail);

	/**
	 * @brief Appends TOC entries to parent item
	 * @param parent Parent TOC item.
	 * @param exp S-expression with toc entries.
	 * @param offset Offset in @a exp expression.
	 * @since 1.5.2
	 */
	void addTocChildren (NSRTocEntry *parent, miniexp_t exp, int offset) const;

	/**
	 * @brief Converts page name into actual number
	 * @param name Page name.
	 * @return Page number (from 0) in case of success, -1 otherwise.
	 * @since 1.5.2
	 */
	int pageFromName (const QString& name) const;

	/**
	 * @brief Gets information about internal page file
	 * @param fileno Page file number, from 0.
	 * @param[out] info Output page file information.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.2
	 */
	bool getPageFileInfo (int fileno, ddjvu_fileinfo_t *info) const;

	mutable QHash<QString, int>	_pageNamesCache;	/**< Cache with page names	*/
	GP<DjVuFileCache>		_cache;			/**< File cache			*/
	GP<DjVuDocument>		_doc;			/**< DjVu file handler		*/
	QSize				_imgSize;		/**< Rendered image size	*/
	QString				_text;			/**< Page text			*/
	int				_pagesCount;		/**< Pages count		*/
	char *				_imgData;		/**< Rendered image data	*/
};

#endif /* __NSRDJVUDOCUMENT_H__ */
