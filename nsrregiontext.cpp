#include "nsrregiontext.h"

NSRRegionText::NSRRegionText ()
{
}

NSRRegionText::NSRRegionText (const NSRWordWithCharactersList& wordsWithCharacters, const QRect& area) :
	_words (wordsWithCharacters),
	_area (area)
{
}

QString
NSRRegionText::getString () const
{
	QString res;

	foreach (const NSRWordWithCharacters& word, _words)
		res += word.getText ();

	return res;
}
