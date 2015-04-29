#include "nsrnormalizedpoint.h"

#include <float.h>

NSRNormalizedPoint::NSRNormalizedPoint () :
	_x (0.0),
	_y (0.0)
{
}

NSRNormalizedPoint::NSRNormalizedPoint (double x, double y) :
	_x (x),
	_y (y)
{
}

NSRNormalizedPoint::NSRNormalizedPoint (int x, int y, int xScale, int yScale)
{
	_x = (xScale == 0) ? 0.0 : (double) x / (double) xScale;
	_y = (yScale == 0) ? 0.0 : (double) y / (double) yScale;

}

NSRNormalizedPoint&
NSRNormalizedPoint::operator= (const NSRNormalizedPoint& p)
{
	_x = p._x;
	_y = p._y;
	return *this;
}

NSRNormalizedPoint
NSRNormalizedPoint::operator- (const NSRNormalizedPoint& p) const
{
	return NSRNormalizedPoint (_x - p._x, _y - p._y);
}

void
NSRNormalizedPoint::transform (const QTransform& matrix)
{
	qreal tmpX = (qreal) _x;
	qreal tmpY = (qreal) _y;

	matrix.map (tmpX, tmpY, &tmpX, &tmpY);

	_x = tmpX;
	_y = tmpY;
}

double
NSRNormalizedPoint::distanceSqr (double x, double y, double xScale, double yScale) const
{
	double xs = (this->_x - x) * xScale;
	double ys = (this->_y - y) * yScale;

	return xs * xs + ys * ys;
}

double
NSRNormalizedPoint::distanceSqr (double				x,
				 double				y,
				 double				xScale,
				 double				yScale,
				 const NSRNormalizedPoint&	start,
				 const NSRNormalizedPoint&	end)
{
	NSRNormalizedPoint point (x, y);
	NSRNormalizedPoint lineSegment (end - start);
	double thisDistance;
	const double lengthSqr = lineSegment._x * lineSegment._x +
				 lineSegment._y * lineSegment._y;

	/* If the length of the current segment is null, we can just
	 * measure the distance to either end point */
	if (lengthSqr < DBL_EPSILON)
		thisDistance = end.distanceSqr (x, y, xScale, yScale);
	else {
		/* Vector from the start point of the current line segment to the measurement point */
		NSRNormalizedPoint a = point - start;

		/* Vector from the same start point to the end point of the current line segment */
		NSRNormalizedPoint b = end - start;

		/* We're using a * b (dot product) := |a| * |b| * cos (phi) and the knowledge
		 * that cos (phi) is adjacent side / hypotenuse (hypotenuse = |b|)
		 * therefore, t becomes the length of the vector that represents the projection of
		 * the point p onto the current line segment
		 * (hint: if this is still unclear, draw it!) */

		float t = (a._x * b._x + a._y * b._y) / lengthSqr;

		if (t < 0)
			/* Projection falls outside the line segment on the side of start point */
			thisDistance = point.distanceSqr (start._x, start._y, xScale, yScale);
		else if (t > 1)
			/* Projection falls outside the line segment on the side of the current point */
			thisDistance = point.distanceSqr (end._x, end._y, xScale, yScale);
		else {
			/* Projection is within [start, *i], determine the length of the perpendicular
			 * distance from the projection to the actual point */
			NSRNormalizedPoint direction = end - start;
			NSRNormalizedPoint projection = start - NSRNormalizedPoint (-t * direction._x, -t * direction._y);
			thisDistance = projection.distanceSqr (x, y, xScale, yScale);
		}
	}

	return thisDistance;
}
