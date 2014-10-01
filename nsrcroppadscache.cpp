#include "nsrcroppadscache.h"

NSRCropPadsCache::NSRCropPadsCache (QObject *parent) :
	QObject (parent)
{
}

NSRCropPadsCache::~NSRCropPadsCache ()
{
}

bool
NSRCropPadsCache::isCropPadsExist (int page) const
{
	return _hash.contains (page);
}

NSRCropPads
NSRCropPadsCache::getCropPads (int page) const
{
	return _hash.value (page);
}

void
NSRCropPadsCache::addCropPads (int page, const NSRCropPads &pads)
{
	if (page <= 0)
		return;

	_hash.insert (page, pads);
}

void
NSRCropPadsCache::removePage (int page)
{
	_hash.remove (page);
}

void
NSRCropPadsCache::clearStorage ()
{
	_hash.clear ();
}
