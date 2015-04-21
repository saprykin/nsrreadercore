#include "nsrtinytextentity.h"

NSRTinyTextEntity::NSRTinyTextEntity (const QString& text, const NSRNormalizedRect& rect) :
	_area (rect)
{
	Q_ASSERT_X (!text.isEmpty (), "NSRTinyTextEntity", "Empty string");
	Q_ASSERT_X (sizeof (_d) == sizeof (QChar *), "NSRTinyTextEntity",
		    "Internal storage is wider than QChar *, fix it!");

	_length = text.length ();

	switch (_length) {
#if QT_POINTER_SIZE >= 8
	case 4:
		d.qc[3] = text.at(3).unicode ();
		/* Fall through */
	case 3:
		d.qc[2] = text.at(2).unicode ();
		/* Fall through */
#endif
	case 2:
		_d.qc[1] = text.at(1).unicode ();
		/* Fall through */
	case 1:
		_d.qc[0] = text.at(0).unicode ();
		break;
	default:
		_d.data = new QChar[_length];
		std::memcpy (_d.data, text.constData (), _length * sizeof (QChar));
	}
}

NSRTinyTextEntity::~NSRTinyTextEntity ()
{
	if (_length > MaxStaticChars)
		delete [] _d.data;
}

QString
NSRTinyTextEntity::getText () const
{
	return (_length <= MaxStaticChars) ? QString::fromRawData ((const QChar *) & _d.qc[0], _length)
					   : QString::fromRawData (_d.data, _length);
}

NSRNormalizedRect
NSRTinyTextEntity::getTransformedArea (const QTransform& matrix) const
{
	NSRNormalizedRect transformedArea = _area;
	transformedArea.transform (matrix);

	return transformedArea;
}
