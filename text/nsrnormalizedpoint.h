#ifndef __NSRNORMALIZEDPOINT_H__
#define __NSRNORMALIZEDPOINT_H__

/**
 * @file nsrnormalizedpoint.h
 * @author Alexander Saprykin
 * @brief Normalized point
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include <QTransform>

/**
 * @class NSRNormalizedPoint nsrnormalizedpoint.h
 * @brief Point with normalized coordinates
 * @since 1.5.1
 *
 * NormalizedPoint is a helper class which stores the coordinates
 * of a normalized point. Normalized means that the coordinates are
 * between 0 and 1 so that it is page size independent.
 *
 * Example:
 *    The normalized point is (0.5, 0.3)
 *
 *    If you want to draw it on a 800x600 page, just multiply the x coordinate (0.5) with
 *    the page width (800) and the y coordinate (0.3) with the page height (600), so
 *    the point will be drawn on the page at (400, 180).
 *
 *    That allows you to zoom the page by just multiplying the normalized points with the
 *    zoomed page size.
 */
class NSRNormalizedPoint
{
public:
	/** Creates a new empty normalized point */
	NSRNormalizedPoint ();

	/**
	 * @brief Creates a new normalized point with the normalized coordinates
	 * @param x Coordinate of x-axis.
	 * @param y Coordinate of y-axis.
	 */
	NSRNormalizedPoint (double x, double y);

	/**
	 * @brief Creates a new normalized point with the coordinates and scale
	 * factors
	 * @param x Coordinate of x-axis.
	 * @param y Coordinate of y-axis.
	 * @param xScale Scale factor to normalize @a x.
	 * @param yScale Scale factor to normalize @a y.
	 */
	NSRNormalizedPoint (int x, int y, int xScale, int yScale);

	/**
	 * @brief Copy operator
	 * @param p Point to copy.
	 * @return Reference to resulting point.
	 * @since 1.5.1
	 */
	NSRNormalizedPoint& operator= (const NSRNormalizedPoint& p);

	/**
	 * @brief Substratcs point one point from another one
	 * @param p Point to substract.
	 * @return Vector between two points.
	 * @since 1.5.1
	 */
	NSRNormalizedPoint operator- (const NSRNormalizedPoint& p) const;

	/**
	 * @brief Gets x-axis coordinate
	 * @return x-axis coordinate.
	 * @since 1.5.1
	 */
	inline double getX () const {
		return _x;
	}

	/**
	 * @brief Gets y-axis coordinate
	 * @return y-axis coordinate.
	 * @since 1.5.1
	 */
	inline double getY () const {
		return _y;
	}

	/**
	 * @brief Sets x-axis coordinate
	 * @param x x-axis coordinate.
	 * @since 1.5.1
	 */
	inline void setX (double x) {
		_x = x;
	}

	/**
	 * @brief Sets y-axis coordinate
	 * @param y y-axis coordinate.
	 * @since 1.5.1
	 */
	inline void setY (double y) {
		_y = y;
	}

	/**
	 * @brief Transforms the normalized point
	 * @param matrix Transformation matrix.
	 * @since 1.5.1
	 */
	void transform (const QTransform &matrix);

	/**
	 * @brief Calculates squared distance to given point
	 * @param x Coordinate of x-axis.
	 * @param y Coordinate of y-axis.
	 * @param xScale Scale for x-axis.
	 * @param yScale Scale for y-axis.
	 * @return Squared distance to point.
	 * @since 1.5.1
	 */
	double distanceSqr (double x, double y, double xScale, double yScale) const;

	/**
	 * @brief Calculates distance of the point to the line segment
	 * @param x Coordinate of x-axis.
	 * @param y Coordinate of y-axis.
	 * @param xScale Scale for x-axis.
	 * @param yScale Scale for y-axis.
	 * @param start Start point of the line segment.
	 * @param end End point of the line segment.
	 * @return Distance from the point to line segment.
	 * @since 1.5.1
	 */
	static double distanceSqr (double			x,
				   double			y,
				   double			xScale,
				   double			yScale,
				   const NSRNormalizedPoint&	start,
				   const NSRNormalizedPoint&	end);

private:
	double _x;	/**< The normalized x coordinate	*/
	double _y;	/**< The normalized y coordinate	*/
};

#endif /* __NSRNORMALIZEDPOINT_H__ */
