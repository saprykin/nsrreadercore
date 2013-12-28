#ifndef NSRRENDERREQUEST_H_
#define NSRRENDERREQUEST_H_

#include "nsrabstractdocument.h"

#include <QObject>
#include <QSize>
#include <QPointF>

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
		NSR_RENDER_REASON_CROP_TO_WIDTH	= 6
	};

	NSRRenderRequest (QObject *parent = 0);
	NSRRenderRequest (int number, QObject *parent = 0);
	NSRRenderRequest (int number, NSRRenderReason reason, QObject *parent = 0);
	NSRRenderRequest (const NSRRenderRequest& page);
	virtual ~NSRRenderRequest ();
	NSRRenderRequest& operator= (const NSRRenderRequest& page);

	NSRRenderRequest::NSRRenderReason getRenderReason () const;
	int getNumber () const;
	double getZoom () const;
	QSize getSize () const;
	NSR_CORE_IMAGE_DATATYPE getImage () const;
	QString getText () const;
	QPointF getLastPosition () const;
	QPointF getLastTextPosition () const;
	bool isValid () const;
	bool isImageValid () const;
	bool isEmpty () const;
	bool isCropped () const;
	bool isCached () const;

	void setRenderReason (NSRRenderRequest::NSRRenderReason reason);
	void setNumber (int number);
	void setZoom (double zoom);
	void setImage (NSR_CORE_IMAGE_DATATYPE img);
	void setText (const QString &text);
	void setLastPosition (const QPointF& pos);
	void setLastTextPosition (const QPointF& pos);
	void setCropped (bool cropped);
	void setCached (bool cached);

private:
	NSRRenderReason		_reason;
	NSR_CORE_IMAGE_DATATYPE	_image;
	QString			_text;
	QPointF			_lastPos;
	QPointF			_lastTextPos;
	double			_zoom;
	int			_number;
	bool			_cropped;
	bool			_cached;
};

#endif /* NSRRENDERREQUEST_H_ */
