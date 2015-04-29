#ifndef __NSRNORMALIZEDRECT_H__
#define __NSRNORMALIZEDRECT_H__

/**
 * @file nsrnormalizedrect.h
 * @author Alexander Saprykin
 * @brief Normalized rectangle
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrnormalizedpoint.h"

#include <QTransform>
#include <QRectF>

/**
 * @class NSRNormalizedRect nsrnormalizedrect.h
 * @brief Normalized rectangle
 * @since 1.5.1
 *
 * NormalizedRect is a helper class which stores the coordinates
 * of a normalized rect, which is a rectangle of several #NSRNormalizedPoint.
 */
class NSRNormalizedRect
{
public:
	/** Creates a null normalized rectangle */
	NSRNormalizedRect ();

	/**
	 * @brief Creates a normalized rectangle with the normalized coordinates
	 * @param left Left border.
	 * @param top Top border.
	 * @param right Right border.
	 * @param bottom Bottom border.
	 *
	 * If you need the x, y, width and height coordinates use the
	 * following formulas:
	 *
	 * @li x = left
	 * @li y = top
	 * @li width = right - left
	 * @li height = bottom - top
	 */
	NSRNormalizedRect (double left, double top, double right, double bottom);

	/**
	 * @brief Creates a normalized rectangle with the scaling factors
	 * @param rect Given normalized rectangle.
	 * @param xScale Scale for x-axis.
	 * @param yScale Scale for y-axis.
	 */
	NSRNormalizedRect (const QRect& rect, double xScale, double yScale);

	/**
	 * @brief Copy constructor
	 * @param rect Rectangle to copy.
	 */
	NSRNormalizedRect (const NSRNormalizedRect& rect);

	/**
	 * @brief Builds a normalized rectangle from @a QRectF
	 * @param rect Rectangle to build from.
	 * @return Normalized rectangle.
	 * @since 1.5.1
	 */
	static NSRNormalizedRect fromQRectF (const QRectF& rect);

	/**
	 * @brief Assignment operator
	 * @param rect Rectangle to copy.
	 * @return Reference to resulting rectangle.
	 * @since 1.5.1
	 */
	NSRNormalizedRect& operator= (const NSRNormalizedRect& rect);

	/**
	 * @brief Combines normalized rectangle with the another normalized rectangle
	 * @param rect Rectangle to combine.
	 * @return Combined normalized rectangle.
	 * @since 1.5.1
	 */
	NSRNormalizedRect operator| (const NSRNormalizedRect& rect) const;

	/**
	 * @brief Combines normalized rectangle with the another normalized rectangle
	 * to itself
	 * @param rect Rectangle to combine.
	 * @return Combined normalized rectangle.
	 * @since 1.5.1
	 */
	NSRNormalizedRect& operator|= (const NSRNormalizedRect& rect);

	/**
	 * @brief Intersects normalized rectangle with the specified one
	 * @param rect Rectangle to intersect with.
	 * @return Intersection of rectangles.
	 * @since 1.5.1
	 */
	NSRNormalizedRect operator& (const NSRNormalizedRect& rect) const;

	/**
	 * @brief Checks whether the normalized rectangle is equal to the another
	 * normalized rectangle
	 * @param rect Rectangle to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	bool operator== (const NSRNormalizedRect& rect) const;

	/**
	 * @brief Gets left border
	 * @return Left border.
	 * @since 1.5.1
	 */
	inline double getLeft () const {
		return _left;
	}

	/**
	 * @brief Gets top border
	 * @return Top border.
	 * @since 1.5.1
	 */
	inline double getTop () const {
		return _top;
	}

	/**
	 * @brief Gets right border
	 * @return Right border.
	 * @since 1.5.1
	 */
	inline double getRight () const {
		return _right;
	}

	/**
	 * @brief Gets bottom border
	 * @return Bottom border.
	 * @since 1.5.1
	 */
	inline double getBottom () const {
		return _bottom;
	}

	/**
	 * @brief Sets left border
	 * @param left Left border.
	 * @since 1.5.1
	 */
	inline void setLeft (double left) {
		_left = left;
	}

	/**
	 * @brief Sets top border
	 * @param top Top border.
	 * @since 1.5.1
	 */
	inline void setTop (double top) {
		_top = top;
	}

	/**
	 * @brief Sets right border
	 * @param right Right border.
	 * @since 1.5.1
	 */
	inline void setRight (double right) {
		_right = right;
	}

	/**
	 * @brief Sets bottom border
	 * @param bottom Bottom border.
	 * @since 1.5.1
	 */
	inline void setBottom (double bottom) {
		_bottom = bottom;
	}

	/**
	 * @brief Checks whether this normalized rectangle is a null.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	bool isNull () const;

	/**
	 * @brief Checks whether the normalized rectangle contains the normalized coordinates
	 * @param x Coordinate of x-axis.
	 * @param y Coordinate of y-axis.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	bool contains (double x, double y) const;

	/**
	 * @brief Checks whether the normalized rectangle intersects the another normalized
	 * rectangle
	 * @param rect Rectangle to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	bool intersects (const NSRNormalizedRect& rect) const;

	/**
	 * @brief Checks whether the normalized rectangle intersects the another normalized
	 * rectangle
	 * @param rect Rectangle to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	bool intersects (const NSRNormalizedRect *rect) const;

	/**
	 * @brief Checks whether the normalized rectangle intersects the another normalized
	 * rectangle
	 * @param left Rectangle's left border.
	 * @param top Rectangle's top border.
	 * @param right Rectangle's right border.
	 * @param bottom Rectangle's bottom border.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	bool intersects (double left, double top, double right, double bottom) const;

	/**
	 * @brief Returns the rectangle that accrues when the normalized rectangle is multiplyed
	 * with the scaling factors
	 * @param xScale Scale for x-axis.
	 * @param yScale Scale for y-axis.
	 * @return Multiplyed rectangle.
	 * @since 1.5.1
	 */
	QRect geometry (int xScale, int yScale) const;

	/**
	 * @brief Returns the rectangle that accrues when the normalized rectangle is multiplyed
	 * with the scaling factors
	 * @param xScale Scale for x-axis.
	 * @param yScale Scale for y-axis.
	 * @return Multiplyed rectangle.
	 * @since 1.5.1
	 *
	 * The output is rounded before typecasting to int.
	 */
	QRect roundedGeometry (int xScale, int yScale) const;

	/**
	 * @brief Gets the center of the rectangle.
	 * @return Center of the rectangle.
	 * @since 1.5.1
	 */
	NSRNormalizedPoint center () const;

	/**
	 * @brief Transforms the normalized rectangle.
	 * @param matrix Transformation matrix.
	 * @since 1.5.1
	 */
	void transform (const QTransform& matrix);

	/**
	 * @brief Checks whether the point is located to the bottom of the rectangle
	 * @param pt Point to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	inline bool isBottom (const NSRNormalizedPoint& pt) const {
		return _bottom < pt.getY ();
	}

	/**
	 * @brief Checks whether the point is located to the top of the rectangle
	 * @param pt Point to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	inline bool isTop (const NSRNormalizedPoint& pt) const {
		return _top > pt.getY ();
	}

	/**
	 * @brief Checks whether the point is located under the top of the rectangle
	 * @param pt Point to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	inline bool isBottomOrLevel (const NSRNormalizedPoint& pt) const {
		return _top < pt.getY ();
	}

	/**
	 * @brief Checks whether the point is located above the bottom of the rectangle
	 * @param pt Point to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	inline bool isTopOrLevel (const NSRNormalizedPoint& pt) const {
		return _bottom > pt.getY ();
	}

	/**
	 * @brief Checks whether the point is located to the right of the left arm of rectangle
	 * @param pt Point to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	inline bool isLeft (const NSRNormalizedPoint& pt) const {
		return _left < pt.getX ();
	}

	/**
	 * @brief Checks whether the point is located to the left of the right arm of rectangle
	 * @param pt Point to check.
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	inline bool isRight (const NSRNormalizedPoint& pt) const {
		return _right > pt.getX ();
	}

	/**
	 * @brief Calculates distance of the point to the closest edge
	 * @param x Coordinate of x-axis.
	 * @param y Coordinate of y-axis.
	 * @param xScale Scale for x-axis.
	 * @param yScale Scale for y-axis.
	 * @return Distance to the closest edge or 0 if the point is within the rectangle.
	 * @since 1.5.1
	 */
	double distanceSqr (double x, double y, double xScale, double yScale) const;

private:
	double _left;	/**< The normalized left coordinate	*/
	double _top;	/**< The normalized top coordinate	*/
	double _right;	/**< The normalized right coordinate	*/
	double _bottom;	/**< The normalized bottom coordinate	*/
};

#endif /* __NSRNORMALIZEDRECT_H__ */
