#include "nsrtextselection.h"

NSRTextSelection::NSRTextSelection (const NSRNormalizedPoint& a, const NSRNormalizedPoint& b)
{
	_cur[0] = a;
	_cur[1] = b;
}

NSRTextSelection::~NSRTextSelection ()
{
}

NSRNormalizedPoint
NSRTextSelection::getStart () const
{
	return _cur[0];
}

NSRNormalizedPoint
NSRTextSelection::getEnd () const
{
	return _cur[1];
}

void
NSRTextSelection::setEnd (const NSRNormalizedPoint& p)
{
	_cur[1] = p;
}
