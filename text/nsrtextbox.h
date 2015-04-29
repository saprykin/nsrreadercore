#ifndef __NSRTEXTBOX_H__
#define __NSRTEXTBOX_H__

/**
 * @file nsrtextbox.h
 * @author Alexander Saprykin
 * @brief Text with related area
 * @copyright 2005, Brad Hards <bradh@frogmouth.net>
 *            2006-2008, Albert Astals Cid <aacid@kde.org>
 *            2008, Pino Toscano <pino@kde.org>
 */

#include <QString>
#include <QRectF>
#include <QVector>

/**
 * @class NSRTextBox nsrtextbox.h
 * @brief Text with related area
 * @since 1.5.1
 *
 * Describes the physical location of text on a document page.
 */
class NSRTextBox
{
public:
	/**
	 * @brief Constructor with parameters
	 * @param text Text.
	 * @param bBox Rectangle that contains text.
	 *
	 * Coordinates for the @a bBox are in points = 1/72 of an inch.
	 */
	NSRTextBox (const QString& text, const QRectF& bBox);

	/** Destructor */
	~NSRTextBox ();

	/**
	 * @brief Gets text of the box
	 * @return Text of the box.
	 * @since 1.5.1
	 */
	inline QString getText () const {
		return _text;
	}

	/**
	 * @brief Gets the position of the text
	 * @return Position of the text, in point, i.e., 1/72 of an inch.
	 * @since 1.5.1
	 */
	inline QRectF getBoundingBox () const {
		return _bBox;
	}

	/**
	 * @brief Gets pointer to the next text box, if any
	 * @return Pointer to the next text box if any, NULL otherwise.
	 * @since 1.5.1
	 */
	inline NSRTextBox * getNextWord () const {
		return _nextWord;
	}

	/**
	 * @brief Gets bounding box of the i-th character of the word
	 * @param i Index of the character.
	 * @return Bounding box of the i-th character.
	 * @since 1.5.1
	 */
	inline QRectF getCharBoundingBox (int i) const {
		return _charBBoxes.value (i);
	}

	/**
	 * @brief Gets char bounding boxes
	 * @return Char bounding boxes.
	 * @since 1.5.1
	 */
	inline QVector<QRectF>& getCharBoundingBoxes () {
		return _charBBoxes;
	}

	/**
	 * @brief Checks whether there is a space character after this text box
	 * @return True in case of success, false otherwise.
	 * @since 1.5.1
	 */
	inline bool hasSpaceAfter () const {
		return _hasSpaceAfter;
	}

	/**
	 * @brief Sets text
	 * @param text Text to set.
	 * @since 1.5.1
	 */
	inline void setText (const QString text) {
		_text = text;
	}

	/**
	 * @brief Sets text bounding box
	 * @param bBox Text bounding box.
	 * @since 1.5.1
	 */
	inline void setBoundingBox (const QRectF& bBox) {
		_bBox = bBox;
	}

	/**
	 * @brief Sets next word
	 * @param word Next word.
	 * @since 1.5.1
	 */
	inline void setNextWord (NSRTextBox *word) {
		_nextWord = word;
	}

	/**
	 * @brief Sets whether the next word is a space
	 * @param hasSpace Whether the next word is a space.
	 * @since 1.5.1
	 */
	inline void setHasSpaceAfter (bool hasSpace) {
		_hasSpaceAfter = hasSpace;
	}

private:
	Q_DISABLE_COPY (NSRTextBox)

	QString		_text;		/**< Text				*/
	QRectF		_bBox;		/**< Bounding box of the text		*/
	NSRTextBox *	_nextWord;	/**< Next word, if any			*/
	QVector<QRectF> _charBBoxes;	/**< Bounding box of each character	*/
	bool		_hasSpaceAfter;	/**< Space after this text box		*/
};

#endif /* __NSRTEXTBOX_H__ */
