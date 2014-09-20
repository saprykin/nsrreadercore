#ifndef NSRABSTRACTDOCUMENT_H
#define NSRABSTRACTDOCUMENT_H

#include "nsrreadercore_global.h"

#include <QObject>
#include <QtGui/QPixmap>

#ifdef Q_OS_BLACKBERRY
#  include <bb/ImageData>
#  include <bb/PixelFormat>
#else
#  include <QtGui/QImage>
#endif

#define NSR_CORE_DOCUMENT_MAX_HEAP	(0x6000000 * 0.50)

#ifdef Q_OS_BLACKBERRY
#  define NSR_CORE_IMAGE_DATATYPE	bb::ImageData
#else
#  define NSR_CORE_IMAGE_DATATYPE	QImage
#endif

class NSRREADERCORE_SHARED NSRAbstractDocument : public QObject
{
	Q_OBJECT
	Q_ENUMS (NSRDocumentError)
	Q_ENUMS (NSRDocumentStyle)
public:
	enum NSRDocumentError {
		NSR_DOCUMENT_ERROR_NO		= 0,
		NSR_DOCUMENT_ERROR_PASSWD	= 1,
		NSR_DOCUMENT_ERROR_TOO_LARGE	= 2,
		NSR_DOCUMENT_ERROR_UNKNOWN	= 3
	};

	enum NSRDocumentStyle {
		NSR_DOCUMENT_STYLE_GRAPHIC	= 1,
		NSR_DOCUMENT_STYLE_TEXT		= 2
	};

	enum NSRDocumentRotation {
		NSR_DOCUMENT_ROTATION_0		= 0,
		NSR_DOCUMENT_ROTATION_90	= 90,
		NSR_DOCUMENT_ROTATION_180	= 180,
		NSR_DOCUMENT_ROTATION_270	= 270
	};

	explicit NSRAbstractDocument (const QString& file, QObject *parent = 0);
	virtual ~NSRAbstractDocument ();
	inline QString getDocumentPath () const {return _docPath;}
	virtual int getNumberOfPages () const			= 0;
	virtual void renderPage (int page)			= 0;
	virtual NSR_CORE_IMAGE_DATATYPE getCurrentPage ()	= 0;
	virtual bool isValid ()	const				= 0;
	double getZoom () const {return _zoom;}
	void setZoom (double zoom);
	virtual double getMaxZoom ()				= 0;
	virtual double getMinZoom ()				= 0;
	virtual void setScreenWidth (int screenWidth) {_screenWidth = screenWidth;}
	int getScreenWidth () const {return _screenWidth;}
	void setZoomToWidth (bool toWidth) {_zoomToWidth = toWidth;}
	bool isZoomToWidth () const {return _zoomToWidth;}
	virtual void rotateLeft ();
	virtual void rotateRight ();
	virtual void setRotation (NSRAbstractDocument::NSRDocumentRotation rotation);
	virtual NSRAbstractDocument::NSRDocumentRotation getRotation () const {return _rotation;}
	virtual QString getText () {return QString ();}
	virtual void setTextOnly (bool textOnly) {_textOnly = textOnly;}
	virtual bool isTextOnly () const {return _textOnly;}
	void setInvertedColors (bool isInverted) {_invertedColors = isInverted;}
	bool isInvertedColors () const {return _invertedColors;}
	void setAutoCrop (bool isAutoCrop) {_autoCrop = isAutoCrop;}
	bool isAutoCrop () const {return _autoCrop;}
	NSRDocumentError getLastError () const {return _lastError;}
	virtual void setPassword (const QString& passwd) {_password = passwd;}
	virtual QString getPassword () const {return _password;}
	virtual void setEncoding (const QString& enc);
	virtual QString getEncoding () const {return _encoding;}
	virtual bool isEncodingUsed () const {return false;}
	void setMaximumPageSize (const QSize& size) {_maxPageSize = size;}
	QSize getMaximumPageSize () const {return _maxPageSize;}
	virtual bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const = 0;
	virtual NSRAbstractDocument::NSRDocumentStyle getPrefferedDocumentStyle () const = 0;

protected:
	void setZoomSilent (double zoom) {_zoom = zoom;}
	void setLastError (NSRDocumentError err) {_lastError = err;}
	QString processText (const QString& text);
	double validateMaxZoom (const QSize& pageSize, double zoom) const;

private:
	QString			_docPath;
	QString			_password;
	QString			_encoding;
	QSize			_maxPageSize;
	double			_zoom;
	int			_screenWidth;
	bool			_zoomToWidth;
	bool			_textOnly;
	bool			_invertedColors;
	bool			_autoCrop;
	NSRDocumentError	_lastError;
	NSRDocumentRotation	_rotation;
};

#endif /* NSRABSTRACTDOCUMENT_H */
