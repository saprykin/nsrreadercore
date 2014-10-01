#include "nsrrenderedpage.h"

#include <QVariant>

NSRRenderedPage::NSRRenderedPage () :
	NSRRenderRequest (),
	_renderedZoom (-1.0),
	_cached (false)
{
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
