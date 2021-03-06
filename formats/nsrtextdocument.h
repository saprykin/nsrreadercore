#ifndef __NSRTEXTDOCUMENT_H__
#define __NSRTEXTDOCUMENT_H__

/**
 * @file nsrtextdocument.h
 * @author Alexander Saprykin
 * @brief Text file handler
 */

#include "nsrabstractdocument.h"

#include <QHash>

/**
 * @class NSRTextDocument nsrtextdocument.h
 * @brief Class for text file handler
 */
class NSRTextDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	/**
	 * @brief Constructor with parameters
	 * @param file Path to file.
	 * @param parent Parent object.
	 */
	NSRTextDocument (const QString& file, QObject *parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRTextDocument ();

	/* Reimplemented from NSRAbstractDocument */
	int getPageCount () const;
	NSRRenderInfo renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid ()	const;

	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;

	NSRAbstractDocument::NSRDocumentStyle getPreferredDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT;
	}

	bool hasDynamicPages () const {
		return true;
	}

	NSRTocEntry * getToc () const {
		return NULL;
	}

	double getZoom () const {
		return 100.0;
	}

	void setZoom (double zoom) {
		Q_UNUSED (zoom);
	}

	int getPageWidth () const {
		return 0;
	}

	void setPageWidth (int pageWidth) {
		Q_UNUSED (pageWidth);
	}

	virtual void setZoomToWidth (bool toWidth) {
		Q_UNUSED (toWidth);
	}

	bool isZoomToWidth () const {
		return false;
	}

	NSRAbstractDocument::NSRDocumentRotation getRotation () const {
		return NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0;
	}

	void setRotation (NSRAbstractDocument::NSRDocumentRotation rotation) {
		Q_UNUSED (rotation);
	}

	QString getText ();

	bool isTextOnly () const {
		return true;
	}

	void setTextOnly (bool textOnly) {
		Q_UNUSED (textOnly);
	}

	bool isInvertedColors () const {
		return false;
	}

	void setInvertedColors (bool isInverted) {
		Q_UNUSED (isInverted);
	}

	bool isAutoCrop () const {
		return false;
	}

	void setAutoCrop (bool isAutoCrop) {
		Q_UNUSED (isAutoCrop);
	}

	QString getPassword () const {
		return QString ();
	}

	void setPassword (const QString& passwd) {
		Q_UNUSED (passwd);
	}

	bool isEncodingUsed () const {
		return true;
	}

private:
	/**
	 * @brief Detects text charset
	 * @return Text charset.
	 */
	QString detectCharset ();

	/**
	 * @brief Rebuilds internal hash
	 * @param encoding Text encoding to rebuild hash with.
	 */
	void rebuildHash (const QString& encoding);

	QHash<int, QString>	_textData;		/**< Hashed data		*/
	QString			_text;			/**< Current page text		*/
	QString			_hashBuildEncoding;	/**< Hashed data encoding	*/
	QString			_autodetectedEncoding;	/**< Autodetected charset	*/
	int			_pageCount;		/**< Page count			*/
	bool			_wasEncodingDetected;	/**< Charset autodetection flag	*/
};

#endif /* __NSRTEXTDOCUMENT_H__ */
