#ifndef __NSRTINYTEXTENTITY_H__
#define __NSRTINYTEXTENTITY_H__

/**
 * @file nsrtinytextentity.h
 * @author Alexander Saprykin
 * @brief Tiny text entity
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrnormalizedrect.h"

#include <QString>

/**
 * @class NSRTinyTextEntity
 * @brief Tiny text entity
 * @since 1.5.1
 *
 * Rationale behind tiny text entity.
 * Instead of storing directly a @a QString for the text of an entity,
 * we store the UTF-16 data and their length. This way, we save about
 * 4 int's wrt a @a QString, and we can create a new string from that
 * raw data (that's the only penalty of that).
 * Even better, if the string we need to store has at most
 * #MaxStaticChars characters, then we store those in place of the @a QChar*
 * that would be used (with new[] + free[]) for the data.
 */
class NSRTinyTextEntity
{
	/** Maximum count of static chars without memory allocation */
	static const int MaxStaticChars = sizeof (QChar *) / sizeof (QChar);

public:
	/**
	 * @brief Creates new tiny text entity
	 * @param text Text data.
	 * @param rect Bounding rectangle.
	 */
	NSRTinyTextEntity (const QString& text, const NSRNormalizedRect& rect);

	/** Deletes tiny text entity */
	~NSRTinyTextEntity ();

	/**
	 * @brief Gets entity text
	 * @return Entity text.
	 * @since 1.5.1
	 */
	QString getText () const;

	/**
	 * @brief Gets entity bounding rectangle
	 * @return Entity bounding rectangle.
	 * @since 1.5.1
	 */
	inline NSRNormalizedRect& getArea () {
		return _area;
	}

	/**
	 * @brief Gets transformed bounding rectangle
	 * @param matrix Transformation matrix.
	 * @return Transformed bounding rectangle.
	 * @since 1.5.1
	 */
	NSRNormalizedRect getTransformedArea (const QTransform& matrix) const;

private:
	Q_DISABLE_COPY (NSRTinyTextEntity)

	union {
		QChar *	data;
		ushort	qc[MaxStaticChars];
	} _d;	/**< Text data */

	int			_length;	/**< Text length	*/
	NSRNormalizedRect	_area;		/**< Bounding rectangle	*/
};

/** Tiny text entity list */
typedef QList< NSRTinyTextEntity * > NSRTinyTextEntityList;

#endif /* __NSRTINYTEXTENTITY_H__ */
