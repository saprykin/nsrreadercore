#include "nsrnormalizedrect.h"

#include <math.h>
#include <float.h>

NSRNormalizedRect::NSRNormalizedRect () :
	_left   (0.0),
	_top    (0.0),
	_right  (0.0),
	_bottom (0.0)
{
}

NSRNormalizedRect::NSRNormalizedRect (double left, double top, double right, double bottom) :
	_left   (left),
	_top    (top),
	_right  (right),
	_bottom (bottom)
{
	if (_left > _right)
		qSwap (_left, _right);

	if (_bottom > _top)
		qSwap (_bottom, _top);
}

NSRNormalizedRect::NSRNormalizedRect (const QRect& rect, double xScale, double yScale)
{
	if (fabs (xScale) < DBL_EPSILON) {
		_left  = 0.0;
		_right = 0.0;
	} else {
		_left  = ((double) rect.left   () / xScale);
		_right = ((double) rect.right  () / xScale);
	}

	if (fabs (yScale) < DBL_EPSILON) {
		_top    = 0.0;
		_bottom = 0.0;
	} else {
		_top    = ((double) rect.top    () / yScale);
		_bottom = ((double) rect.bottom () / yScale);
	}
}

NSRNormalizedRect::NSRNormalizedRect (const NSRNormalizedRect& rect) :
	_left   (rect._left),
	_top    (rect._top),
	_right  (rect._right),
	_bottom (rect._bottom)
{
}

NSRNormalizedRect
NSRNormalizedRect::fromQRectF (const QRectF& rect)
{
	QRectF nrect = rect.normalized ();

	return NSRNormalizedRect  (nrect.left (), nrect.top (), nrect.right (), nrect.bottom ());
}

NSRNormalizedRect&
NSRNormalizedRect::operator= (const NSRNormalizedRect& rect)
{
	_left   = rect._left;
	_right  = rect._right;
	_top    = rect._top;
	_bottom = rect._bottom;

	return *this;
}

NSRNormalizedRect
NSRNormalizedRect::operator| (const NSRNormalizedRect& rect) const
{
	NSRNormalizedRect ret;

	ret._left   = qMin (_left,   rect._left);
	ret._top    = qMin (_top,    rect._top);
	ret._bottom = qMax (_bottom, rect._bottom);
	ret._right  = qMax (_right,  rect._right);

	return ret;
}

NSRNormalizedRect&
NSRNormalizedRect::operator|= (const NSRNormalizedRect& rect)
{
	_left   = qMin (_left,   rect._left);
	_top    = qMin (_top,    rect._top);
	_bottom = qMax (_bottom, rect._bottom);
	_right  = qMax (_right,  rect._right);

	return *this;
}

NSRNormalizedRect
NSRNormalizedRect::operator& (const NSRNormalizedRect& rect) const
{
	if (isNull () || rect.isNull ())
		return NSRNormalizedRect ();

	NSRNormalizedRect ret;

	ret._left   = qMax (_left,   rect._left);
	ret._top    = qMax (_top,    rect._top);
	ret._bottom = qMin (_bottom, rect._bottom);
	ret._right  = qMin (_right,  rect._right);

	return ret;
}

bool
NSRNormalizedRect::operator== (const NSRNormalizedRect& rect) const
{
	return (isNull () && rect.isNull ()) ||
		(fabs (_left   - rect._left)   < DBL_EPSILON &&
		 fabs (_right  - rect._right)  < DBL_EPSILON &&
		 fabs (_top    - rect._top)    < DBL_EPSILON &&
		 fabs (_bottom - rect._bottom) < DBL_EPSILON);
}

bool
NSRNormalizedRect::isNull () const
{
	return _left == 0 && _top == 0 && _right == 0 && _bottom == 0;
}

bool
NSRNormalizedRect::contains (double x, double y) const
{
	return x >= _left && x <= _right && y >= _top && y <= _bottom;
}

bool
NSRNormalizedRect::intersects (const NSRNormalizedRect& rect) const
{
	return (rect._left <= _right)  && (rect._right  >= _left) &&
	       (rect._top  <= _bottom) && (rect._bottom >= _top);
}

bool
NSRNormalizedRect::intersects (const NSRNormalizedRect *rect) const
{
    return (rect->_left <= _right)  && (rect->_right  >= _left) &&
	   (rect->_top  <= _bottom) && (rect->_bottom >= _top);
}

bool
NSRNormalizedRect::intersects (double left, double top, double right, double bottom) const
{
	return (left <= _right) && (right >= _left) && (top <= _bottom) && (bottom >= _top);
}

QRect
NSRNormalizedRect::geometry (int xScale, int yScale) const
{
	int l = (int) (_left   * xScale);
	int t = (int) (_top    * yScale);
	int r = (int) (_right  * xScale);
	int b = (int) (_bottom * yScale);

	return QRect (l, t, r - l + 1, b - t + 1);
}

QRect
NSRNormalizedRect::roundedGeometry (int xScale, int yScale) const
{
	int l = (int) (_left   * xScale + 0.5);
	int t = (int) (_top    * yScale + 0.5);
	int r = (int) (_right  * xScale + 0.5);
	int b = (int) (_bottom * yScale + 0.5);

	return QRect (l, t, r - l + 1, b - t + 1);
}

NSRNormalizedPoint
NSRNormalizedRect::center () const
{
	return NSRNormalizedPoint ((_left + _right) / 2.0,
				   (_top + _bottom) / 2.0);
}

void
NSRNormalizedRect::transform (const QTransform& matrix)
{
	QRectF rect (_left, _top, _right - _left, _bottom - _top);
	rect = matrix.mapRect (rect);

	_left   = rect.left   ();
	_top    = rect.top    ();
	_right  = rect.right  ();
	_bottom = rect.bottom ();
}

double
NSRNormalizedRect::distanceSqr (double x, double y, double xScale, double yScale) const
{
	double distX = 0;

	if (x < _left)
		distX = _left - x;
	else if (x > _right)
		distX = x - _right;

	double distY = 0;

	if (_top > y)
		distY = _top - y;
	else if (_bottom < y)
		distY = y - _bottom;

	return distX * distX * xScale * xScale + distY * distY * yScale * yScale;
}
