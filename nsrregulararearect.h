#ifndef __NSRREGULARAREARECT_H__
#define __NSRREGULARAREARECT_H__

/**
 * @file nsrregulararearect.h
 * @author Alexander Saprykin
 * @brief Regular area rectangle
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrregulararea.h"
#include "nsrnormalizedrect.h"

/**
 * @class NSRRegularAreaRect nsrregulararearect.h
 * @brief Regular area rectangle
 */
class NSRRegularAreaRect : public NSRRegularArea< NSRNormalizedRect, QRect >
{
public:
	/** Constructor */
	NSRRegularAreaRect ();

	/**
	 * @brief Copy constructor
	 * @param rar Regular area to copy.
	 */
	NSRRegularAreaRect (const NSRRegularAreaRect& rar);

	/** Destructor */
	~NSRRegularAreaRect ();

	/**
	 * @brief Assignment operator
	 * @param rar Regular area to assign.
	 * @return Reference to resulting regular area rectangle.
	 */
	NSRRegularAreaRect& operator= (const NSRRegularAreaRect& rar);
};

#endif /* __NSRREGULARAREARECT_H__ */
