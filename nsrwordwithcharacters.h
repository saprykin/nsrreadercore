#ifndef __NSRWORDWITHCHARACTERS_H__
#define __NSRWORDWITHCHARACTERS_H__

/**
 * @file nsrwordwithcharacters.h
 * @author Alexander Saprykin
 * @brief Word with separate characters
 * @copyright Piotr Szymanski, 2005, <niedakh@gmail.com>
 */

#include "nsrtinytextentity.h"

/**
 * @class NSRWordWithCharacters nsrwordwithcharacters.h
 * @brief Word with separate characters
 */
class NSRWordWithCharacters
{
public:
	/**
	 * @brief Creates word with characters
	 * @param w Word.
	 * @param c List of characters.
	 */
	NSRWordWithCharacters (NSRTinyTextEntity *w, const NSRTinyTextEntityList& c);

	/** Destroys word with characters */
	~NSRWordWithCharacters ();

	/**
	 * @brief Gets the word
	 * @return The word.
	 */
	inline NSRTinyTextEntity * getWord () const {
		return _word;
	}

	/**
	 * @brief Gets list of characters
	 * @return List of characters.
	 * @note Returned list will be sharing the same internal pointers.
	 */
	inline NSRTinyTextEntityList getCharacters () const {
		return _characters;
	}

	/**
	 * @brief Gets the word as a string
	 * @return The word as a string.
	 */
	inline QString getText () const {
		return _word != NULL ? _word->getText () : QString ();
	}

	/**
	 * @brief Gets the word's area
	 * @return The word's area.
	 */
	inline NSRNormalizedRect getArea () const {
		return _word != NULL ? _word->getArea () : NSRNormalizedRect ();
	}

private:
	NSRTinyTextEntity *	_word;		/**< The word		*/
	NSRTinyTextEntityList	_characters;	/**< List of characters	*/
};

/** Defines list of words with characters */
typedef QList<NSRWordWithCharacters> NSRWordWithCharactersList;

#endif /* __NSRWORDWITHCHARACTERS_H__ */
