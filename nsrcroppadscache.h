#ifndef __NSRCROPPADSCACHE_H__
#define __NSRCROPPADSCACHE_H__

/**
 * @file nsrcroppadscache.h
 * @author Alexander Saprykin
 * @brief Page crop pads cache
 */

#include "nsrcroppads.h"

#include <QObject>
#include <QHash>

/**
 * @class NSRCropPadsCache nsrcroppadscache.h
 * @brief Class for page crop pads cache
 * @since 1.4.2
 */
class NSRCropPadsCache : public QObject
{
	Q_OBJECT
public:
	/**
	 * @brief Constructor with parameter
	 * @param parent Parent object.
	 */
	NSRCropPadsCache (QObject *parent = 0);

	/** Destructor */
	~NSRCropPadsCache ();

	/**
	 * @brief Checks whether crop pads for given page number are
	 * in cache
	 * @param page Page number to check (from 1).
	 * @return True if crop pads are in cache, false otherwise.
	 * @since 1.4.2
	 */
	bool isCropPadsExist (int page) const;

	/**
	 * @brief Gets crop pads for given page number
	 * @param page Page number (from 1).
	 * @return Crop pads for given page number in case of success,
	 * empty crop pads otherwise.
	 * @since 1.4.2
	 */
	NSRCropPads getCropPads (int page) const;

	/**
	 * @brief Adds page crop pads to cache
	 * @param page Page number (from 1).
	 * @param pads Page crop pads.
	 * @since 1.4.2
	 *
	 * If crop pads for given page number are already in cache they
	 * will be replaced.
	 */
	void addCropPads (int page, const NSRCropPads& pads);

	/**
	 * @brief Removes page crop pads from the cache
	 * @param page Page number to remove (from 1).
	 * @since 1.4.2
	 */
	void removePage (int page);

	/**
	 * @brief Removes all page crop pads
	 */
	void clearStorage ();

private:
	QHash<int, NSRCropPads>	_hash;	/**< Page crop pads cache	*/
};

#endif /* __NSRCROPPADSCACHE_H__ */
