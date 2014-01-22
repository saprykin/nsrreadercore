#include "nsrrenderedpage.h"

#include <QVariant>

NSRRenderedPage::NSRRenderedPage (QObject *parent) :
	NSRRenderRequest (parent),
	_renderedZoom (-1.0),
	_cached (false)
{
}

NSRRenderedPage::NSRRenderedPage (const NSRRenderedPage& page) :
	NSRRenderRequest (page)
{
	_image		= page._image;
	_text		= page._text;
	_lastPos	= page._lastPos;
	_lastTextPos	= page._lastTextPos;
	_renderedZoom	= page._renderedZoom;
	_cached		= page._cached;
}

NSRRenderedPage::NSRRenderedPage (const NSRRenderRequest& req) :
	NSRRenderRequest (req),
	_renderedZoom (-1.0),
	_cached (false)
{
}

NSRRenderedPage::~NSRRenderedPage ()
{
}

NSRRenderedPage&
NSRRenderedPage::operator = (const NSRRenderedPage& page)
{
	if (this != &page) {
		NSRRenderRequest::operator= (page);
		_image		= page._image;
		_text		= page._text;
		_lastPos	= page._lastPos;
		_lastTextPos	= page._lastTextPos;
		_renderedZoom	= page._renderedZoom;
		_cached		= page._cached;
	}

	return *this;
}

bool
NSRRenderedPage::isImageValid () const
{
#ifdef Q_OS_BLACKBERRY
	return getNumber () > 0 && _image.isValid ();
#else
	return getNumber () > 0 && !_image.isNull ();
#endif
}

bool
NSRRenderedPage::isEmpty () const
{
#ifdef Q_OS_BLACKBERRY
	return _text.isEmpty () && !_image.isValid ();
#else
	return _text.isEmpty () && _image.isNull ();
#endif
}
