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

NSRTocEntry *NSRTocEntry::clone () const
{
	NSRTocEntry *entry = new NSRTocEntry (_title, _page);

	entry->_isExternal   = _isExternal;
	entry->_externalFile = _externalFile;
	entry->_uri          = _uri;

	foreach (NSRTocEntry* child, _children)
		entry->appendChild (child->clone ());

	return entry;
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
