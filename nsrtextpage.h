#ifndef __NSRTEXTPAGE_H__
#define __NSRTEXTPAGE_H__

/**
 * @file nsrtextpage.h
 * @author Alexander Saprykin
 * @brief Text of the page
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrabstractdocument.h"
#include "nsrtextentity.h"
#include "nsrtinytextentity.h"
#include "nsrtextselection.h"
#include "nsrnormalizedrect.h"
#include "nsrregulararearect.h"
#include "nsrsearchpoint.h"
#include "nsrregiontext.h"

#include <QMap>
#include <QList>
#include <QString>
#include <QTransform>
#include <QSizeF>

/**
 * @brief Returns whether the two strings are matched
 * @param from First string.
 * @param to Second string.
 * @return True if the strings are matched.
 */
typedef bool (*NSRTextComparisonFunction) (const QStringRef& from, const QStringRef& to);

/**
 * @class NSRTextPage nsrtextpage.h
 * @brief Text of the page.
 * @since 1.5.1
 *
 * Represents the text of a page by providing #NSRTextEntity items for every
 * word/character of the page.
 */
class NSRTextPage
{
public:
	/** Describes the direction of searching */
	enum SearchDirection {
		FromTop,	/**< Searching from top of the page, next result is to be found,
				     there was no earlier search result */
		FromBottom,	/**< Searching from bottom of the page, next result is to be found,
				     there was no earlier search result */
		NextResult,	/**< Searching for the next result on the page, earlier result should
				     be located so we search from the last result not from the beginning
				     of the page */
		PreviousResult	/**< Searching for the previous result on the page, earlier result
				     should be located so we search from the last result not from the
				     beginning of the page */
	};

	/** Defines the behaviour of adding characters to text result */
	enum TextAreaInclusionBehaviour {
		AnyPixelTextAreaInclusionBehaviour,	/**< A character is included into text result if
							     any pixel of his bounding box is in the given area */
		CentralPixelTextAreaInclusionBehaviour	/**< A character is included into text result if
							     the central pixel of his bounding box is in the
							     given area */
	};

	/**
	 * @brief Creates a new text page
	 * @param size Size of the page, in pixels.
	 * @param rotation Page rotation angle (user defined).
	 * @param orientation Page orientation angle (file defined).
	 */
	NSRTextPage (const QSizeF&				size,
		     NSRAbstractDocument::NSRDocumentRotation	rotation,
		     NSRAbstractDocument::NSRDocumentRotation	orientation);

	/**
	 * @brief Creates a new text page with given words
	 * @param size Size of the page, in pixels.
	 * @param rotation Page rotation angle (user defined).
	 * @param orientation Page orientation angle (file defined).
	 * @param words Text words.
	 */
	NSRTextPage (const QSizeF&				size,
		     NSRAbstractDocument::NSRDocumentRotation	rotation,
		     NSRAbstractDocument::NSRDocumentRotation	orientation,
		     const NSRTextEntityList&			words);

	/** Destroys the text page */
	~NSRTextPage ();

	/**
	 * @brief Appends the text with given area as new text entity
	 * @param text Text to append.
	 * @param area Text area.
	 * @since 1.5.1
	 */
	void append (const QString& text, const NSRNormalizedRect& area);

	/**
	 * @brief Searches text on the page
	 * @param id An unique id for this search.
	 * @param text The search text.
	 * @param direction The direction of the search
	 * @param caseSensitivity Case sensivity.
	 * @return Bounding rectangle of the text which matches the following criteria
	 * or NULL if the search is not successful.
	 * @since 1.5.1
	 * @note Ownership of the returned area belongs to the caller.
	 */
	NSRRegularAreaRect * findText (int			id,
				       const QString&		text,
				       SearchDirection		direction,
				       Qt::CaseSensitivity	caseSensitivity);

	/**
	 * @brief Extracts text from the page
	 * @param rect Text rectangle to extract.
	 * @return Empty string if @a rect is a valid pointer to a null area,
	 * the whole page text if @a rect is a NULL pointer, the text which is
	 * included by rectangular area @a rect otherwise.
	 * @since 1.5.1
	 *
	 * Uses #TextAreaInclusionBehaviour::AnyPixelTextAreaInclusionBehaviour.
	 */
	QString text (const NSRRegularAreaRect *rect = NULL) const;

	/**
	 * @brief Extracts text from the page
	 * @param rect Text rectangle to extract.
	 * @param b Text area inclusion behaviour.
	 * @return Empty string if @a rect is a valid pointer to a null area,
	 * the whole page text if @a rect is a NULL pointer, the text which is
	 * included by rectangular area @a rect otherwise.
	 * @since 1.5.1
	 */
	QString text (const NSRRegularAreaRect *rect, TextAreaInclusionBehaviour b) const;

	/**
	 * @brief Extracts text entities.
	 * @param rect Text rectangle to extract.
	 * @param b Text area inclusion behaviour.
	 * @return Words including their bounding rectangles.
	 * @since 1.5.1
	 * @note Ownership of the contents of the returned list belongs to the caller.
	 */
	NSRTextEntityList words (const NSRRegularAreaRect *rect, TextAreaInclusionBehaviour b) const;

	/**
	 * @brief Gets the area and text of the word at the given point
	 * @param p Point where to search.
	 * @param[out] word Text at a given point.
	 * @return Text area.
	 * @since 1.5.1
	 * @note Ownership of the returned area belongs to the caller.
	 */
	NSRRegularAreaRect * wordAt (const NSRNormalizedPoint& p, QString *word = NULL) const;

	/**
	 * @brief Gets the rectangular area of a given selection
	 * @param selection Selection to get rectangular area for.
	 * @return Rectangular area of a given selection.
	 * @since 1.5.1
	 * @note Ownership of the returned area belongs to the caller.
	 */
	NSRRegularAreaRect * textArea (const NSRTextSelection& selection) const;

	/**
	 * @brief Copies text list.
	 * @param list List to copy.
	 * @since 1.5.1
	 */
	void setWordList (const NSRTinyTextEntityList& list);

	/**
	 * @brief Makes necessary modifications in the text list to make
	 * the text order correct, so that text selection works fine
	 * @since 1.5.1
	 */
	void correctTextOrder ();

private:
	Q_DISABLE_COPY (NSRTextPage)

	/**
	 * @brief Maps search point to area
	 * @param sp Search point to map.
	 * @return Mapped area.
	 * @since 1.5.1
	 * @note Ownership of the returned area belongs to the caller.
	 */
	NSRRegularAreaRect * searchPointToArea (const NSRSearchPoint *sp);

	/**
	 * @brief Searches text in forward direction
	 * @param searchID Unique search id.
	 * @param query Search query.
	 * @param comparer Text match function.
	 * @param start Start position.
	 * @param startOffset Text start offset within the word.
	 * @param end End position.
	 * @return Text area in case of success, NULL otherwise.
	 * @since 1.5.1
	 * @note Ownership of the returned area belongs to the caller.
	 */
	NSRRegularAreaRect * findTextInternalForward (int						searchID,
						      const QString&					query,
						      NSRTextComparisonFunction				comparer,
						      const NSRTinyTextEntityList::ConstIterator&	start,
						      int						startOffset,
						      const NSRTinyTextEntityList::ConstIterator&	end);

	/**
	 * @brief Searches text in backward direction
	 * @param searchID Unique search id.
	 * @param query Search query.
	 * @param comparer Text match function.
	 * @param start Start position.
	 * @param startOffset Text start offset within the word.
	 * @param end End position.
	 * @return Text area in case of success, NULL otherwise.
	 * @since 1.5.1
	 * @note Ownership of the returned area belongs to the caller.
	 */
	NSRRegularAreaRect * findTextInternalBackward (int						searchID,
						       const QString&					query,
						       NSRTextComparisonFunction			comparer,
						       const NSRTinyTextEntityList::ConstIterator&	start,
						       int						startOffset,
						       const NSRTinyTextEntityList::ConstIterator&	end);

	/**
	 * @brief Gets page transformation matrix
	 * @return Page transformation matrix.
	 * @since 1.5.1
	 */
	QTransform getTransformMatrix () const;

	NSRTinyTextEntityList				_words;		/**< Page words				*/
	QMap< int, NSRSearchPoint * >			_searchPoints;	/**< Search points			*/
	QSizeF						_size;		/**< Page size, in pixels		*/
	NSRAbstractDocument::NSRDocumentRotation	_rotation;	/**< Rotation angle (user defined)	*/
	NSRAbstractDocument::NSRDocumentRotation	_orientation;	/**< Orientation angle (file defined)	*/
};

#endif /* __NSRTEXTPAGE_H__ */
