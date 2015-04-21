#include "nsrwordwithcharacters.h"

NSRWordWithCharacters::NSRWordWithCharacters (NSRTinyTextEntity *w, const NSRTinyTextEntityList& c) :
	_word (w),
	_characters (c)
{
}

NSRWordWithCharacters::~NSRWordWithCharacters ()
{
	if (_word != NULL)
		delete _word;
}
