#ifndef __NSRTEXTENTITY_H__
#define __NSRTEXTENTITY_H__

/**
 * @file nsrtextentity.h
 * @author Alexander Saprykin
 * @brief Abstract text entity
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrnormalizedrect.h"

#include <QList>
#include <QTransform>

/**
 * @class NSRTextEntity nsrtextentity.h
 * @brief Abstract text entity
 * @since 1.5.1
 *
 * A document can provide different forms of information about textual representation
 * of its contents. It can include information about positions of every character on the
 * page, this is the best possibility.
 *
 * But also it can provide information only about positions of every word on the page (not the character).
 * Furthermore it can provide information only about the position of the whole page's text on the page.
 *
 * Also some document types have glyphes - sets of characters rendered as one, so in search they should
 * appear as a text but are only one character when drawn on screen. We need to allow this.
 */
class NSRTextEntity
{
public:
	/**
	 * @brief Creates a new text entity with the given text and area
	 * @param text Text data.
	 * @param area Area reactangle.
	 *
	 * Object becomes an owner of the given @a area.
	 */
	NSRTextEntity (const QString& text, NSRNormalizedRect *area);

	/** Destroys the text entity */
	~NSRTextEntity ();

	/**
	 * @brief Gets the text of the text entity
	 * @return Text of the text entity.
	 * @since 1.5.1
	 */
	inline QString getText () const {
		return _text;
	}

	/**
	 * @brief Gets the bounding area of the text entity
	 * @return Bounding area of the text entity.
	 * @since 1.5.1
	 */
	inline NSRNormalizedRect * getArea () const {
		return _area;
	}

	/**
	 * @brief Gets the transformed area of the text entity
	 * @return Transformed area of the text entity.
	 * @since 1.5.1
	 */
	NSRNormalizedRect getTransformedArea (const QTransform& matrix) const;

private:
	Q_DISABLE_COPY (NSRTextEntity)

	QString			_text;	/**< Text data		*/
	NSRNormalizedRect *	_area;	/**< Bounding area	*/
};

/** List of #NSRTextEntity */
typedef QList<NSRTextEntity *> NSRTextEntityList;

#endif /* __NSRTEXTENTITY_H__ */
