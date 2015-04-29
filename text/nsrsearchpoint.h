#ifndef __NSRSEARCHPOINT_H__
#define __NSRSEARCHPOINT_H__

/**
 * @file nsrsearchpoint.h
 * @author Alexander Saprykin
 * @brief Text search point
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrtinytextentity.h"

/**
 * @class NSRSearchPoint nsrsearchpoint.h
 * @brief Text search point
 * @since 1.5.1
 */
class NSRSearchPoint
{
public:
	/** Default constructor */
	NSRSearchPoint ();

	/**
	 * @brief Gets begin iterator
	 * @return Begin iterator.
	 * @since 1.5.1
	 */
	inline NSRTinyTextEntityList::ConstIterator getBegin () const {
		return _itBegin;
	}

	/**
	 * @brief Gets end iterator
	 * @return End iterator.
	 * @since 1.5.1
	 */
	inline NSRTinyTextEntityList::ConstIterator getEnd () const {
		return _itEnd;
	}

	/**
	 * @brief Gets begin offset
	 * @return Begin offset.
	 * @since 1.5.1
	 */
	inline int getOffsetBegin () const {
		return _offsetBegin;
	}

	/**
	 * @brief Gets end offset
	 * @return End offset.
	 * @since 1.5.1
	 */
	inline int getOffsetEnd () const {
		return _offsetEnd;
	}

	/**
	 * @brief Sets begin iterator
	 * @param begin Begin iterator.
	 * @since 1.5.1
	 */
	inline void setBegin (const NSRTinyTextEntityList::ConstIterator& begin) {
		_itBegin = begin;
	}

	/**
	 * @brief Sets end iterator
	 * @param end End iterator.
	 * @since 1.5.1
	 */
	inline void setEnd (const NSRTinyTextEntityList::ConstIterator& end) {
		_itEnd = end;
	}

	/**
	 * @brief Sets begin offset
	 * @param offset Begin offset.
	 * @since 1.5.1
	 */
	inline void setOffsetBegin (int offset) {
		_offsetBegin = offset;
	}

	/**
	 * @brief Sets end offset
	 * @param offset End offset.
	 * @since 1.5.1
	 */
	inline void setOffsetEnd (int offset) {
		_offsetEnd = offset;
	}

private:
	NSRTinyTextEntityList::ConstIterator _itBegin;	/**< First character of the match	*/
	NSRTinyTextEntityList::ConstIterator _itEnd;	/**< Last character of the match	*/

	/**
	 * The index of the first character of the match in (*_itBegin)->getText ().
	 * Satisfies 0 <= _offsetBegin < (*_itBegin)->getText().length ().
	 */
	int			_offsetBegin;

	/**
	 * One plus the index of the last character of the match in (*_itEnd)->getText ().
	 * Satisfies 0 < _offsetEnd <= (*_itEnd)->getText().length ().
	 */
	int			_offsetEnd;
};

#endif /* __NSRSEARCHPOINT_H__ */
