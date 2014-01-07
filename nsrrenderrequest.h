#ifndef NSRRENDERREQUEST_H_
#define NSRRENDERREQUEST_H_

#include <QObject>
#include <QString>

#include <qmath.h>

class NSRRenderRequest : public QObject
{
	Q_OBJECT
	Q_ENUMS (NSRRenderReason)
public:
	enum NSRRenderReason {
		NSR_RENDER_REASON_NONE		= 0,
		NSR_RENDER_REASON_NAVIGATION	= 1,
		NSR_RENDER_REASON_SETTINGS	= 2,
		NSR_RENDER_REASON_ZOOM		= 3,
		NSR_RENDER_REASON_ZOOM_TO_WIDTH	= 4,
		NSR_RENDER_REASON_ROTATION	= 5,
		NSR_RENDER_REASON_CROP_TO_WIDTH	= 6,
		NSR_RENDER_REASON_PRELOAD	= 7
	};

	NSRRenderRequest (QObject *parent = 0);
	NSRRenderRequest (int number, QObject *parent = 0);
	NSRRenderRequest (int number, NSRRenderReason reason, QObject *parent = 0);
	NSRRenderRequest (const NSRRenderRequest& req);
	virtual ~NSRRenderRequest ();

	virtual NSRRenderRequest& operator= (const NSRRenderRequest& req);

	inline QString getEncoding () const {
		return _encoding;
	}

	inline NSRRenderRequest::NSRRenderReason getRenderReason () const {
		return _reason;
	}

	inline int getNumber () const {
		return _number;
	}

	inline double getZoom () const {
		return _zoom;
	}

	inline double getRotation () const {
		return _rotation;
	}

	inline double getScreenWidth () const {
		return _screenWidth;
	}

	inline bool isValid () const {
		return _number > 0;
	}

	inline bool isAutoCrop () const {
		return _autoCrop;
	}

	inline bool isInvertColors () const {
		return _invertColors;
	}

	inline bool isTextOnly () const {
		return _textOnly;
	}

	inline bool isZoomToWidth () const {
		return _zoomToWidth;
	}

	inline void setEncoding (const QString& encoding) {
		_encoding = encoding;
	}

	inline void setRenderReason (NSRRenderRequest::NSRRenderReason reason) {
		_reason = reason;
	}

	inline void setNumber (int number) {
		_number = number;
	}

	inline void setZoom (double zoom) {
		_zoom = qBound (0.0, zoom, zoom);
	}

	inline void setRotation (double rotation) {
		_rotation = rotation;
	}

	inline void setScreenWidth (double screenWidth) {
		_screenWidth = screenWidth;
	}

	inline void setAutoCrop (bool autoCrop) {
		_autoCrop = autoCrop;
	}

	inline void setInvertColors (bool invertColors) {
		_invertColors = invertColors;
	}

	inline void setTextOnly (bool textOnly) {
		_textOnly = textOnly;
	}

	inline void setZoomToWidth (bool zoomToWidth) {
		_zoomToWidth = zoomToWidth;
	}

private:

	QString			_encoding;
	NSRRenderReason		_reason;
	double			_zoom;
	double			_rotation;
	double			_screenWidth;
	int			_number;
	bool			_autoCrop;
	bool			_invertColors;
	bool			_textOnly;
	bool			_zoomToWidth;
};

#endif /* NSRRENDERREQUEST_H_ */
