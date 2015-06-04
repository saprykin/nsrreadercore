#include "nsrtocentry.h"

NSRTocEntry::NSRTocEntry (const QString& title, int page) :
	_page (-1),
	_isExternal (false)
{
	_title = title;
	_page  = page;
}

NSRTocEntry::~NSRTocEntry ()
{
	qDeleteAll (_children);
	_children.clear ();
}

QList<const NSRTocEntry *> NSRTocEntry::getChildren () const
{
	QList<const NSRTocEntry *> ret;

	foreach (const NSRTocEntry *entry, _children)
		ret.append (entry);

	return ret;
}

void NSRTocEntry::appendChild (NSRTocEntry *entry)
{
	if (entry == NULL)
		return;

	if (_children.contains (entry))
		return;

	_children.append (entry);
}
