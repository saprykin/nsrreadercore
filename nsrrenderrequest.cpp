#include "nsrrenderrequest.h"

#include <QVariant>

NSRRenderRequest::NSRRenderRequest (QObject *parent) :
	QObject (parent),
	_encoding ("UTF-8"),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0.0),
	_rotation (0.0),
	_screenWidth (0.0),
	_number (0),
	_autoCrop (false),
	_invertColors (false),
	_textOnly (false),
	_zoomToWidth (false)
{
}

NSRRenderRequest::NSRRenderRequest (int number, QObject *parent) :
	QObject (parent),
	_encoding ("UTF-8"),
	_reason (NSR_RENDER_REASON_NONE),
	_zoom (0.0),
	_rotation (0.0),
	_screenWidth (0.0),
	_number (number),
	_autoCrop (false),
	_invertColors (false),
	_textOnly (false),
	_zoomToWidth (false)
{
}

NSRRenderRequest::NSRRenderRequest (int number, NSRRenderReason reason, QObject *parent) :
	QObject (parent),
	_encoding ("UTF-8"),
	_reason (reason),
	_zoom (0.0),
	_rotation (0.0),
	_screenWidth (0.0),
	_number (number),
	_autoCrop (false),
	_invertColors (false),
	_textOnly (false),
	_zoomToWidth (false)
{
}

NSRRenderRequest::NSRRenderRequest (const NSRRenderRequest& req) :
	QObject (req.parent ())
{
	_encoding	= req._encoding;
	_reason		= req._reason;
	_zoom		= req._zoom;
	_rotation	= req._rotation;
	_screenWidth	= req._screenWidth;
	_number		= req._number;
	_autoCrop	= req._autoCrop;
	_invertColors	= req._invertColors;
	_textOnly	= req._textOnly;
	_zoomToWidth	= req._zoomToWidth;
}

NSRRenderRequest::~NSRRenderRequest ()
{
}

NSRRenderRequest&
NSRRenderRequest::operator = (const NSRRenderRequest& req)
{
	if (this != &req) {
		_encoding	= req._encoding;
		_reason		= req._reason;
		_zoom		= req._zoom;
		_rotation	= req._rotation;
		_screenWidth	= req._screenWidth;
		_number		= req._number;
		_autoCrop	= req._autoCrop;
		_invertColors	= req._invertColors;
		_textOnly	= req._textOnly;
		_zoomToWidth	= req._zoomToWidth;
	}

	return *this;
}
