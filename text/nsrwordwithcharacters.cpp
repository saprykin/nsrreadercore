#include "nsrwordwithcharacters.h"

NSRWordWithCharacters::NSRWordWithCharacters (NSRTinyTextEntity *w, const NSRTinyTextEntityList& c) :
	_word (w),
	_characters (c)
{
}

NSRWordWithCharacters::~NSRWordWithCharacters ()
{
}

void NSRWordWithCharacters::freeWord ()
{
	if (_word != NULL) {
		delete _word;
		_word = NULL;
	}
}

void NSRWordWithCharacters::freeCharacters ()
{
	qDeleteAll (_characters);
	_characters.clear ();
}
