#include "nsrrenderrequest.h"

#include <QVariant>

NSRRenderRequest::NSRRenderRequest () :
	_encoding ("UTF-8"),
	_reason (NSR_RENDER_REASON_NONE),
	_type (NSR_RENDER_TYPE_PAGE),
	_rotation (NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0),
	_zoom (0.0),
	_screenWidth (0.0),
	_number (0),
	_autoCrop (false),
	_invertColors (false),
	_textOnly (false),
	_zoomToWidth (false)
{
}

NSRRenderRequest::NSRRenderRequest (int number) :
	_encoding ("UTF-8"),
	_reason (NSR_RENDER_REASON_NONE),
	_type (NSR_RENDER_TYPE_PAGE),
	_rotation (NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0),
	_zoom (0.0),
	_screenWidth (0.0),
	_number (number),
	_autoCrop (false),
	_invertColors (false),
	_textOnly (false),
	_zoomToWidth (false)
{
}

NSRRenderRequest::NSRRenderRequest (int number, NSRRenderReason reason) :
	_encoding ("UTF-8"),
	_reason (reason),
	_type (NSR_RENDER_TYPE_PAGE),
	_rotation (NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0),
	_zoom (0.0),
	_screenWidth (0.0),
	_number (number),
	_autoCrop (false),
	_invertColors (false),
	_textOnly (false),
	_zoomToWidth (false)
{
}

NSRRenderRequest::~NSRRenderRequest ()
{
}
