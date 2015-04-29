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
 * @since 1.5.1
 *
 * This class act like a light wrapper around given text and its characters.
 * The idea is that you can sort, move and copy objects of this class to
 * prevent useless memory allocation calls. Though you can free the memory
 * using freeWord() and freeCharacters().
 *
 * Use this class to reuse pointers allocated previously to perform logical
 * operations above the words.
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
	 * @brief Frees word memory
	 * @since 1.5.1
	 */
	void freeWord ();

	/**
	 * @brief Frees characters memory
	 * @since 1.5.1
	 */
	void freeCharacters ();

	/**
	 * @brief Gets the word
	 * @return The word.
	 * @since 1.5.1
	 */
	inline NSRTinyTextEntity * getWord () const {
		return _word;
	}

	/**
	 * @brief Gets list of characters
	 * @return List of characters.
	 * @since 1.5.1
	 * @note Returned list will be sharing the same internal pointers.
	 */
	inline NSRTinyTextEntityList getCharacters () const {
		return _characters;
	}

	/**
	 * @brief Gets the word as a string
	 * @return The word as a string.
	 * @since 1.5.1
	 */
	inline QString getText () const {
		return _word != NULL ? _word->getText () : QString ();
	}

	/**
	 * @brief Gets the word's area
	 * @return The word's area.
	 * @since 1.5.1
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
