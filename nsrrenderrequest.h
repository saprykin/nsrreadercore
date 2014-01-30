#ifndef NSRRENDERREQUEST_H_
#define NSRRENDERREQUEST_H_

/**
 * @file nsrrenderrequest.h
 * @author Alexander Saprykin
 * @brief Rendering request
 */

#include <QObject>
#include <QString>

#include <qmath.h>

/**
 * @class NSRRenderRequest nsrrenderrequest.h
 * @brief Rendering request class
 *
 * Rendering request is an input data for NSR rendering engine. It
 * describes how engine should render page from the file. Rendering
 * request can be of different types, i.e. ordinary page or thumbnail.
 * Engine processes requests through queue one by one.
 * Rendering reason defines the source of the request. Different reasons
 * affect rendering in different threads.
 * By default #NSR_RENDER_TYPE_PAGE is used.
 * Thumbnails are always rendered with both image and text, with autocrop
 * and inverted colors disabled, zoomed to specified screen width.
 */
class NSRRenderRequest : public QObject
{
	Q_OBJECT
	Q_ENUMS (NSRRenderReason)
	Q_ENUMS (NSRRenderType)
public:
	/** Rendering reason */
	enum NSRRenderReason {
		NSR_RENDER_REASON_NONE		= 0,	/**< Empty reason				*/
		NSR_RENDER_REASON_NAVIGATION	= 1,	/**< Navigation or page selection		*/
		NSR_RENDER_REASON_SETTINGS	= 2,	/**< Some settings have been changed		*/
		NSR_RENDER_REASON_ZOOM		= 3,	/**< Zoom operation				*/
		NSR_RENDER_REASON_ZOOM_TO_WIDTH	= 4,	/**< Zoom to screen width			*/
		NSR_RENDER_REASON_ROTATION	= 5,	/**< Page rotation				*/
		NSR_RENDER_REASON_CROP_TO_WIDTH	= 6,	/**< Crop blank edges when zooming to width	*/
		NSR_RENDER_REASON_PRELOAD	= 7,	/**< Preload page ahead				*/
		NSR_RENDER_REASON_THUMBNAIL	= 8	/**< Thumbnail rendering			*/
	};

	/** Request type */
	enum NSRRenderType {
		NSR_RENDER_TYPE_PAGE		= 1,	/**< Render as common page			*/
		NSR_RENDER_TYPE_THUMBNAIL	= 2	/**< Render as thumbnail, both image and text	*/
	};

	/**
	 * @brief Default constructor
	 * @param parent Parent object.
	 */
	NSRRenderRequest (QObject *parent = 0);

	/**
	 * @brief Constructor with page number
	 * @param number Page number for request.
	 * @param parent Parent object.
	 * @since 1.4.0
	 */
	NSRRenderRequest (int number, QObject *parent = 0);

	/**
	 * @brief Constructor with page number and render reason
	 * @param number Page number for request.
	 * @param reason Rendering reason.
	 * @param parent Parent object.
	 * @since 1.4.0
	 */
	NSRRenderRequest (int number, NSRRenderReason reason, QObject *parent = 0);

	/**
	 * @brief Copy constructor
	 * @param req #NSRRenderRequest to copy.
	 * @since 1.4.0
	 */
	NSRRenderRequest (const NSRRenderRequest& req);

	/**
	 * @brief Default destructor
	 */
	virtual ~NSRRenderRequest ();

	/**
	 * @brief Equal operator overloading
	 * @param req #NSRRenderRequest to copy data from.
	 * @since 1.4.0
	 */
	virtual NSRRenderRequest& operator= (const NSRRenderRequest& req);

	/**
	 * @brief Gets text encoding to use when extracting the text
	 * @return Text encoding.
	 * @since 1.4.0
	 */
	inline QString getEncoding () const {
		return _encoding;
	}

	/**
	 * @brief Gets rendering reason
	 * @return Rendering reason.
	 * @since 1.4.0
	 */
	inline NSRRenderRequest::NSRRenderReason getRenderReason () const {
		return _reason;
	}

	/**
	 * @brief Gets rendering type
	 * @return Rendering type.
	 * @since 1.4.0
	 */
	inline NSRRenderRequest::NSRRenderType getRenderType () const {
		return _type;
	}

	/**
	 * @brief Gets requested page number
	 * @return Requested page number.
	 * @since 1.4.0
	 */
	inline int getNumber () const {
		return _number;
	}

	/**
	 * @brief Gets requested zoom, in %
	 * @return Requested zoom.
	 * @since 1.4.0
	 */
	inline double getZoom () const {
		return _zoom;
	}

	/**
	 * @brief Gets rotation angle, clockwise
	 * @return Rotation angle.
	 * @since 1.4.0
	 */
	inline double getRotation () const {
		return _rotation;
	}

	/**
	 * @brief Gets screen width used to zoom to width
	 * @return Screen width for zoom to width.
	 * @since 1.4.0
	 */
	inline double getScreenWidth () const {
		return _screenWidth;
	}

	/**
	 * @brief Checks if request is valid
	 * @return True if request is valid (page number > 0), false otherwise.
	 * @since 1.4.0
	 */
	inline bool isValid () const {
		return _number > 0;
	}

	/**
	 * @brief Checks autocrop flag
	 * @return True if autocrop is enabled, false otherwise.
	 * @since 1.4.0
	 */
	inline bool isAutoCrop () const {
		return _autoCrop;
	}

	/**
	 * @brief Checks inverted colors flag
	 * @return True if inverted colors are enabled, false otherwise.
	 * @since 1.4.0
	 */
	inline bool isInvertColors () const {
		return _invertColors;
	}

	/**
	 * @brief Checks text only flag
	 * @return True if text only rendering requested, false otherwise.
	 * @since 1.4.0
	 */
	inline bool isTextOnly () const {
		return _textOnly;
	}

	/**
	 * @brief Checks zoom to width flag
	 * @return True if zoom to screen width is enabled, false otherwise.
	 * @since 1.4.0
	 * @sa getScreenWidth(), setScreenWidth()
	 */
	inline bool isZoomToWidth () const {
		return _zoomToWidth;
	}

	/**
	 * @brief Sets encoding used for text extracting
	 * @param encoding Text encoding.
	 * @since 1.4.0
	 * @note Not all formats support text data and encoding selection.
	 */
	inline void setEncoding (const QString& encoding) {
		_encoding = encoding;
	}

	/**
	 * @brief Sets rendering reason
	 * @param reason Rendering reason.
	 * @since 1.4.0
	 */
	inline void setRenderReason (NSRRenderRequest::NSRRenderReason reason) {
		_reason = reason;
	}

	/**
	 * @brief Sets rendering type
	 * @param type Rendering type.
	 * @since 1.4.0
	 */
	inline void setRenderType (NSRRenderRequest::NSRRenderType type) {
		_type = type;
	}

	/**
	 * @brief Sets requested page number
	 * @param number Requested page number.
	 * @since 1.4.0
	 */
	inline void setNumber (int number) {
		_number = number;
	}

	/**
	 * @brief Sets requested zoom, in %
	 * @param zoom Requested zoom.
	 * @since 1.4.0
	 */
	inline void setZoom (double zoom) {
		_zoom = qBound (0.0, zoom, zoom);
	}

	/**
	 * @brief Sets page rotation angle, clockwise
	 * @param rotation Rotation angle.
	 * @since 1.4.0
	 */
	inline void setRotation (double rotation) {
		_rotation = rotation;
	}

	/**
	 * @brief Sets screen width for zoom to width
	 * @param screenWidth Screen width to zoom page to.
	 * @since 1.4.0
	 * @sa setZoomToWidth(), isZoomToWidth()
	 *
	 * For ordinary page requests this value used only if zoom to width
	 * is enabled. For thumbnails it used as a width.
	 */
	inline void setScreenWidth (double screenWidth) {
		_screenWidth = screenWidth;
	}

	/**
	 * @brief Sets autocrop flag
	 * @param autoCrop Autocrop flag to set.
	 * @since 1.4.0
	 */
	inline void setAutoCrop (bool autoCrop) {
		_autoCrop = autoCrop;
	}

	/**
	 * @brief Sets inverted colors flag
	 * @param invertColors Inverted colors flag to set.
	 * @since 1.4.0
	 */
	inline void setInvertColors (bool invertColors) {
		_invertColors = invertColors;
	}

	/**
	 * @brief Sets text only flag
	 * @param textOnly Text only flag to set.
	 * @since 1.4.0
	 */
	inline void setTextOnly (bool textOnly) {
		_textOnly = textOnly;
	}

	/**
	 * @brief Sets zoom to width flag
	 * @param zoomToWidth Zoom to width flag.
	 * @since 1.4.0
	 * @sa setScreenWidth()
	 *
	 * Use setScreenWidth() to set screen width to zoom to.
	 */
	inline void setZoomToWidth (bool zoomToWidth) {
		_zoomToWidth = zoomToWidth;
	}

private:
	QString			_encoding;	/**< Text encoding		*/
	NSRRenderReason		_reason;	/**< Rendering reason		*/
	NSRRenderType		_type;		/**< Rendering type		*/
	double			_zoom;		/**< Zoom, %			*/
	double			_rotation;	/**< Rotation angle, clockwise	*/
	double			_screenWidth;	/**< Screen width		*/
	int			_number;	/**< Page number		*/
	bool			_autoCrop;	/**< Autocrop flag		*/
	bool			_invertColors;	/**< Inverted colors flag	*/
	bool			_textOnly;	/**< Text only flag		*/
	bool			_zoomToWidth;	/**< Zoom to width flag		*/
};

#endif /* NSRRENDERREQUEST_H_ */
