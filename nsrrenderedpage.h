#ifndef NSRRENDEREDPAGE_H_
#define NSRRENDEREDPAGE_H_

#include "nsrrenderrequest.h"
#include "nsrabstractdocument.h"

#include <QObject>
#include <QSize>
#include <QPointF>

class NSRRenderedPage: public NSRRenderRequest
{
	Q_OBJECT
public:
	NSRRenderedPage (QObject *parent = 0);
	NSRRenderedPage (const NSRRenderedPage& page);
	NSRRenderedPage (const NSRRenderRequest& req);
	virtual ~NSRRenderedPage ();

	NSRRenderedPage& operator= (const NSRRenderedPage& page);

	inline QSize getSize () const {
		return QSize (_image.width (), _image.height ());
	}

	inline NSR_CORE_IMAGE_DATATYPE getImage () const {
		return _image;
	}

	inline QString getText () const {
		return _text;
	}

	inline QPointF getLastPosition () const {
		return _lastPos;
	}

	inline QPointF getLastTextPosition () const {
		return _lastTextPos;
	}

	inline bool isCached () const {
		return _cached;
	}

	bool isImageValid () const;
	bool isEmpty () const;

	inline void setImage (NSR_CORE_IMAGE_DATATYPE image) {
		_image = image;
	}

	inline void setText (const QString &text) {
		_text = text;
	}

	inline void setLastPosition (const QPointF& pos) {
		_lastPos = pos;
	}

	inline void setLastTextPosition (const QPointF& pos) {
		_lastTextPos = pos;
	}

	inline void setCached (bool cached) {
		_cached = cached;
	}

private:
	void loadFromProperties (const QObject& obj);

	NSR_CORE_IMAGE_DATATYPE	_image;
	QString			_text;
	QPointF			_lastPos;
	QPointF			_lastTextPos;
	bool			_cached;
};

#endif /* NSRRENDEREDPAGE_H_ */
