#include "nsrrenderrequest.h"

NSRRenderRequest::NSRRenderRequest (QObject *parent) :
	QObject (parent),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0),
	_number (0),
	_cropped (false),
	_cached (false)
{
}

NSRRenderRequest::NSRRenderRequest (int number, QObject *parent) :
	QObject (parent),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0),
	_number (number),
	_cropped (false),
	_cached (false)
{
}

NSRRenderRequest::NSRRenderRequest (int number, NSRRenderReason reason, QObject *parent) :
		QObject (parent),
		_reason (reason),
		_zoom (0),
		_number (number),
		_cropped (false),
		_cached (false)
{
}

NSRRenderRequest::NSRRenderRequest (const NSRRenderRequest& page) :
	QObject (page.parent ())
{
	_reason		= page._reason;
	_image		= page._image;
	_zoom		= page._zoom;
	_number		= page._number;
	_text		= page._text;
	_lastPos	= page._lastPos;
	_lastTextPos	= page._lastTextPos;
	_cropped	= page._cropped;
	_cached		= page._cached;
}

NSRRenderRequest::~NSRRenderRequest ()
{
}

NSRRenderRequest&
NSRRenderRequest::operator = (const NSRRenderRequest& page)
{
	if (this != &page) {
		_reason		= page._reason;
		_image		= page._image;
		_zoom		= page._zoom;
		_number		= page._number;
		_text		= page._text;
		_lastPos	= page._lastPos;
		_lastTextPos	= page._lastTextPos;
		_cropped	= page._cropped;
		_cached		= page._cached;
	}

	return *this;
}

NSRRenderRequest::NSRRenderReason
NSRRenderRequest::getRenderReason () const
{
	return _reason;
}

int
NSRRenderRequest::getNumber () const
{
	return _number;
}

double
NSRRenderRequest::getZoom () const
{
	return _zoom;
}

QSize
NSRRenderRequest::getSize () const
{
	return QSize (_image.width (), _image.height ());
}

NSR_CORE_IMAGE_DATATYPE
NSRRenderRequest::getImage () const
{
	return _image;
}

QString
NSRRenderRequest::getText () const
{
	return _text;
}

QPointF
NSRRenderRequest::getLastPosition () const
{
	return _lastPos;
}

QPointF
NSRRenderRequest::getLastTextPosition () const
{
	return _lastTextPos;
}

bool
NSRRenderRequest::isValid () const
{
	return _number > 0;
}

bool
NSRRenderRequest::isImageValid () const
{
#ifdef Q_OS_BLACKBERRY
	return _number > 0 && _image.isValid ();
#else
	return _number > 0 && !_image.isNull ();
#endif
}

bool

NSRRenderRequest::isEmpty () const
{
#ifdef Q_OS_BLACKBERRY
	return _text.isEmpty () && !_image.isValid ();
#else
	return _text.isEmpty () && _image.isNull ();
#endif
}

bool
NSRRenderRequest::isCropped () const
{
	return _cropped;
}

bool
NSRRenderRequest::isCached () const
{
	return _cached;
}

void
NSRRenderRequest::setRenderReason (NSRRenderRequest::NSRRenderReason reason)
{
	_reason = reason;
}

void
NSRRenderRequest::setNumber (int number)
{
	_number = number;
}

void
NSRRenderRequest::setZoom (double zoom)
{
	if (zoom < 0)
		zoom = 0;

	_zoom = zoom;
}

void
NSRRenderRequest::setImage (NSR_CORE_IMAGE_DATATYPE img)
{
	_image = img;
}

void
NSRRenderRequest::setText (const QString& text)
{
	_text = text;
}

void
NSRRenderRequest::setLastPosition (const QPointF& pos)
{
	_lastPos = pos;
}

void
NSRRenderRequest::setLastTextPosition (const QPointF& pos)
{
	_lastTextPos = pos;
}

void
NSRRenderRequest::setCropped (bool cropped)
{
	_cropped = cropped;
}

void
NSRRenderRequest::setCached (bool cached)
{
	_cached = cached;
}
