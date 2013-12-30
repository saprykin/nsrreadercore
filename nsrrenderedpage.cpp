#include "nsrrenderedpage.h"

#include <QVariant>

NSRRenderedPage::NSRRenderedPage (QObject *parent) :
	NSRRenderRequest (parent),
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
	_cached		= page._cached;
}

NSRRenderedPage::NSRRenderedPage (const NSRRenderRequest& req) :
	NSRRenderRequest (req),
	_cached (false)
{
	loadFromProperties (req);
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
		_cached		= page._cached;
	}

	loadFromProperties (page);

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

void
NSRRenderedPage::loadFromProperties (const QObject& obj)
{
	QList<QByteArray> props = obj.dynamicPropertyNames ();

	if (props.contains ("nsr-cache-text"))
		_text = obj.property("nsr-cache-text").toString ();

	if (props.contains ("nsr-cache-last-text-position"))
		_lastTextPos = obj.property("nsr-cache-last-text-position").toPointF ();
}
