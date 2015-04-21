#include "nsrtextentity.h"

NSRTextEntity::NSRTextEntity (const QString &text, NSRNormalizedRect *area) :
	_text (text),
	_area (area)
{
}

NSRTextEntity::~NSRTextEntity ()
{
	delete _area;
}

NSRNormalizedRect
NSRTextEntity::getTransformedArea (const QTransform& matrix) const
{
	NSRNormalizedRect transformedArea = *_area;
	transformedArea.transform (matrix);

	return transformedArea;
}
