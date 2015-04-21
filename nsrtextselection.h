#ifndef __NSRTEXTSELECTION_H__
#define __NSRTEXTSELECTION_H__

/**
 * @file nsrtextselection.h
 * @author Alexander Saprykin
 * @brief Text selection
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrnormalizedpoint.h"

/**
 * @class NSRTextSelection nsrtextselection.h
 * @brief Wrapper around the information needed to generate the selection area
 *
 * There are two assumptions inside this class:
 * 1. The start never changes, one instance of this class is used for one selection,
 *    therefore the start of the selection will not change, only end and direction of
 *    the selection will change.
 * 2. By direction we mean the direction in which the end moves in relation to the start,
 *    forward selection is when end is after the start, backward when its before.
*/
class NSRTextSelection
{
public:
	/**
	 * @brief Creates a new text selection with the given start and end points
	 * @param start Start point.
	 * @param end End point.
	 */
	NSRTextSelection (const NSRNormalizedPoint& getStart, const NSRNormalizedPoint& setEnd);

	/** Destroys the text selection */
	~NSRTextSelection ();

	/**
	 * @brief Gets start point of the selection
	 * @return Start point if the selection.
	 */
	NSRNormalizedPoint getStart () const;

	/**
	 * @brief Gets end point of the selection
	 * @return End point of the selection.
	 */
	NSRNormalizedPoint getEnd () const;

	/**
	 * @brief Sets the end point of the selection
	 * @param point End point of the selection.
	 */
	void setEnd (const NSRNormalizedPoint& point);

private:
	NSRNormalizedPoint	_cur[2];	/**< Selection points	*/
};

#endif /* __NSRTEXTSELECTION_H__ */
