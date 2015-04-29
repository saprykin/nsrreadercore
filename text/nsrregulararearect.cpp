#include "nsrregulararearect.h"

NSRRegularAreaRect::NSRRegularAreaRect () :
	NSRRegularArea< NSRNormalizedRect, QRect > ()
{
}

NSRRegularAreaRect::NSRRegularAreaRect (const NSRRegularAreaRect& rar) :
	NSRRegularArea< NSRNormalizedRect, QRect > (rar)
{
}

NSRRegularAreaRect::~NSRRegularAreaRect ()
{
}

NSRRegularAreaRect&
NSRRegularAreaRect::operator= (const NSRRegularAreaRect& rar)
{
	NSRRegularArea< NSRNormalizedRect, QRect >::operator= (rar);
	return *this;
}
