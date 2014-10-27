#ifndef __NSRABSTRACTDOCUMENT_H__
#define __NSRABSTRACTDOCUMENT_H__

/**
 * @file nsrabstractdocument.h
 * @author Alexander Saprykin
 * @brief Base class for supported file type
 */

#include "nsrcroppads.h"
#include "nsrrenderinfo.h"
#include "nsrreadercore_global.h"

#include <QObject>
#include <QtGui/QPixmap>

#ifdef Q_OS_BLACKBERRY
#  include <bb/ImageData>
#  include <bb/PixelFormat>
#else
#  include <QImage>
#endif

/** Maximum memroy available for files rendering, bytes */
#define NSR_CORE_DOCUMENT_MAX_HEAP	(0x6000000)

/** @def NSR_CORE_IMAGE_DATATYPE Rendered page image type */
#ifdef Q_OS_BLACKBERRY
#  define NSR_CORE_IMAGE_DATATYPE	bb::ImageData
#else
#  define NSR_CORE_IMAGE_DATATYPE	QImage
#endif

/**
 * @class NSRAbstractDocument nsrabstractdocument.h
 * @brief Base class for supported file type
 *
 * Implement it to add support for any file type.
 */
class NSRREADERCORE_SHARED NSRAbstractDocument : public QObject
{
	Q_OBJECT
	Q_ENUMS (NSRDocumentError)
	Q_ENUMS (NSRDocumentStyle)
	Q_ENUMS (NSRDocumentRotation)
public:
	/** File errors */
	enum NSRDocumentError {
		NSR_DOCUMENT_ERROR_NO		= 0,	/**< No errors		*/
		NSR_DOCUMENT_ERROR_PASSWD	= 1,	/**< Invalid password	*/
		NSR_DOCUMENT_ERROR_TOO_LARGE	= 2,	/**< File is too large	*/
		NSR_DOCUMENT_ERROR_UNKNOWN	= 3	/**< Unknown error	*/
	};

	/** Representing style */
	enum NSRDocumentStyle {
		NSR_DOCUMENT_STYLE_GRAPHIC	= 1,	/**< Graphic (image)	*/
		NSR_DOCUMENT_STYLE_TEXT		= 2	/**< Text only		*/
	};

	/** Rotation angle (clockwise) */
	enum NSRDocumentRotation {
		NSR_DOCUMENT_ROTATION_0		= 0,	/**< No rotation	*/
		NSR_DOCUMENT_ROTATION_90	= 90,	/**< 90 degrees		*/
		NSR_DOCUMENT_ROTATION_180	= 180,	/**< 180 degrees	*/
		NSR_DOCUMENT_ROTATION_270	= 270	/**< 270 degrees	*/
	};

	/**
	 * @brief Constructor with parameters
	 * @param file Path to file.
	 * @param parent Parent object.
	 */
	explicit NSRAbstractDocument (const QString& file, QObject *parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRAbstractDocument ();

	/**
	 * @brief Gets pages count in file
	 * @return Pages count in file.
	 */
	virtual int getPagesCount () const			= 0;

	/**
	 * @brief Renders page internally
	 * @param page Page number to render, starts from 1.
	 */
	virtual NSRRenderInfo renderPage (int page)		= 0;

	/**
	 * @brief Gets last rendered page image
	 * @return Last rendered page image in case of success,
	 * empty object otherwise.
	 */
	virtual NSR_CORE_IMAGE_DATATYPE getCurrentPage ()	= 0;

	/**
	 * @brief Checks whether the file is valid
	 * @return True if file is valid and opened successfully, false
	 * otherwise.
	 */
	virtual bool isValid ()	const				= 0;

	/**
	 * @brief Checks whether file supports given representing style
	 * @param style Style to check.
	 * @return True is file supports style, false otherwise.
	 */
	virtual bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const	= 0;

	/**
	 * @brief Gets preferred representing style
	 * @return Preferred representing style.
	 */
	virtual NSRAbstractDocument::NSRDocumentStyle getPreferredDocumentStyle () const		= 0;

	/**
	 * @brief Gets path to file
	 * @return Path to file.
	 */
	inline QString getDocumentPath () const {
		return _docPath;
	}

	/**
	 * @brief Gets current zoom
	 * @return Current zoom, in %.
	 */
	virtual double getZoom () const {
		return _zoom;
	}

	/**
	 * @brief Sets current zoom
	 * @param zoom Current zoom, in %.
	 * @note Zoom to width option will be reseted.
	 */
	virtual void setZoom (double zoom);

	/**
	 * @brief Gets page width
	 * @return Page width for rendering.
	 * @note Used only if zoom to width is enabled and as crop
	 * limiter when zoom to width is disabled.
	 */
	virtual int getPageWidth () const {
		return _pageWidth;
	}

	/**
	 * @brief Sets page width
	 * @param pageWidth Page width for rendering.
	 * @note Used only if zoom to width is enabled and as crop
	 * limiter when zoom to width is disabled.
	 */
	virtual void setPageWidth (int pageWidth) {
		_pageWidth = pageWidth;
	}

	/**
	 * @brief Sets zoom to width option
	 * @param toWidth Whether to render page to it's width.
	 * @note Use setPageWidth() to set page width.
	 */
	virtual void setZoomToWidth (bool toWidth) {
		_zoomToWidth = toWidth;
	}

	/**
	 * @brief Checks whether zoom to width option is enabled.
	 * @return True is zoom to width option is enabled, false otherwise.
	 */
	virtual bool isZoomToWidth () const {
		return _zoomToWidth;
	}

	/**
	 * @brief Rotates page left by 90 degrees
	 */
	void rotateLeft ();

	/**
	 * @brief Rotates page right by 90 degrees
	 */
	void rotateRight ();

	/**
	 * @brief Gets rotation angle
	 * @return Rotation angle.
	 * @note Only 0, 90, 180, 270 degrees angles are supported.
	 */
	virtual NSRAbstractDocument::NSRDocumentRotation getRotation () const {
		return _rotation;
	}

	/**
	 * @brief Sets rotation angle
	 * @param rotation Rotation angle.
	 * @note Only 0, 90, 180, 270 degrees angles are supported.
	 */
	virtual void setRotation (NSRAbstractDocument::NSRDocumentRotation rotation);

	/**
	 * @brief Gets page text (if available).
	 * @return Page text.
	 */
	virtual QString getText () {
		return QString ();
	}

	/**
	 * @brief Checks whether only text data is retrieved on rendering
	 * @return True if only text data is retrieved from file on page
	 * rendering.
	 */
	virtual bool isTextOnly () const {
		return _textOnly;
	}

	/**
	 * @brief Sets text only rendering mode
	 * @param textOnly Text only rendering mode.
	 */
	virtual void setTextOnly (bool textOnly) {
		_textOnly = textOnly;
	}

	/**
	 * @brief Checks whether inverted colors rendering mode is enabled
	 * @return True is inverted colors rendering mode is enabled, false
	 * otherwise.
	 */
	virtual bool isInvertedColors () const {
		return _invertedColors;
	}

	/**
	 * @brief Sets inverted colors rendering mode
	 * @param isInverted Inverted colors rendering mode.
	 */
	virtual void setInvertedColors (bool isInverted) {
		_invertedColors = isInverted;
	}

	/**
	 * @brief Checks whether autocrop option is enabled
	 * @return True if autocrop option is enabled, false otherwise.
	 */
	virtual bool isAutoCrop () const {
		return _autoCrop;
	}

	/**
	 * @brief Sets autocrop option
	 * @param isAutoCrop Autocrop option.
	 */
	virtual void setAutoCrop (bool isAutoCrop) {
		_autoCrop = isAutoCrop;
	}

	/**
	 * @brief Gets page crop pads
	 * @return Page crop pads.
	 * @since 1.4.2
	 *
	 * Only has meaning if autocrop feature is enabled.
	 */
	virtual NSRCropPads getCropPads () const {
		return _cropPads;
	}

	/**
	 * @brief Sets page crop pads
	 * @param pads Page crop pads.
	 * @since 1.4.2
	 *
	 * Only has meaning if autocrop feature is enabled.
	 */
	virtual void setCropPads (const NSRCropPads& pads) {
		_cropPads = pads;
	}

	/**
	 * @brief Gets file password
	 * @return File password.
	 */
	virtual QString getPassword () const {
		return _password;
	}

	/**
	 * @brief Sets file password
	 * @param passwd File password.
	 */
	virtual void setPassword (const QString& passwd) {
		_password = passwd;
	}

	/**
	 * @brief Checks whether text encoding is used to decode
	 * text data
	 * @return True if encoding is used, false otherwise.
	 */
	virtual bool isEncodingUsed () const {
		return false;
	}

	/**
	 * @brief Gets text encoding
	 * @return Text encoding.
	 */
	virtual QString getEncoding () const {
		return _encoding;
	}

	/**
	 * @brief Sets text encoding
	 * @param enc Text encoding.
	 * @sa QTextCodec
	 */
	virtual void setEncoding (const QString& enc);

	/**
	 * @brief Gets last occurred error
	 * @return Last occurred error.
	 */
	NSRDocumentError getLastError () const {
		return _lastError;
	}

protected:
	/**
	 * @brief Sets zoom without reseting zoom to width flag
	 * @param zoom Zoom value, in %.
	 */
	void setZoomSilent (double zoom) {
		_zoom = zoom;
	}

	/**
	 * @brief Sets last occurred error
	 * @param err Last occurred error.
	 */
	void setLastError (NSRDocumentError err) {
		_lastError = err;
	}

	/**
	 * @brief Processes text to more appealing style
	 * @param text Text to process.
	 * @return Processed text.
	 *
	 * Removes empty paragraphs, prolog spaces, etc.
	 */
	QString processText (const QString& text);

	/**
	 * @brief Validates page zoom for maximum supported
	 * @param pageSize Page size.
	 * @param zoom Zoom value, in %.
	 * @return Corrected (if need) zoom value, in %.
	 */
	double validateMaxZoom (const QSize& pageSize, double zoom) const;

private:
	NSRCropPads		_cropPads;		/**< Page crop pads		*/
	QString			_docPath;		/**< File path			*/
	QString			_password;		/**< File password		*/
	QString			_encoding;		/**< Text encoding		*/
	double			_zoom;			/**< Current zoom, %		*/
	int			_pageWidth;		/**< Page width			*/
	bool			_zoomToWidth;		/**< Zoom to width		*/
	bool			_textOnly;		/**< Text only mode		*/
	bool			_invertedColors;	/**< Inverted colors mode	*/
	bool			_autoCrop;		/**< Autocrop			*/
	NSRDocumentError	_lastError;		/**< Last error			*/
	NSRDocumentRotation	_rotation;		/**< Rotation angle		*/
};

#endif /* __NSRABSTRACTDOCUMENT_H__ */
