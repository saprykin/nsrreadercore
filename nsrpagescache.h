#ifndef __NSRPAGESCACHE_H__
#define __NSRPAGESCACHE_H__

/**
 * @file nsrpagescache.h
 * @author Alexander Saprykin
 * @brief Rendered pages cache
 */

#include "nsrrenderedpage.h"

#include <QObject>
#include <QHash>
#include <QList>

/**
 * @class NSRPagesCache nsrpagescache.h
 * @brief Class for rendered pages cache
 */
class NSRPagesCache : public QObject
{
	Q_OBJECT
public:
	/**
	 * @brief Constructor with parameter
	 * @param parent Parent object.
	 */
	NSRPagesCache (QObject *parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRPagesCache ();

	/**
	 * @brief Checks whether page is in cache
	 * @param number Page number (from 1).
	 * @return True if page is in cache, false otherwise.
	 */
	bool isPageExists (int number) const;

	/**
	 * @brief Gets cached page
	 * @param number Page number (from 1).
	 * @return Cached page is exists, empty page otherwise.
	 */
	NSRRenderedPage getPage (int number) const;

	/**
	 * @brief Adds page into cache
	 * @param page Rendered page.
	 * @note If page with the same number is in cache it will be
	 * replaced.
	 */
	void addPage (const NSRRenderedPage &page);

	/**
	 * @brief Removes page from the cache
	 * @param number Page number (from 1).
	 */
	void removePage (int number);

	/**
	 * @brief Clears cache and remove all pages.
	 */
	void clearStorage ();

	/**
	 * @brief Updates page scroll positions
	 * @param number Page number (from 1).
	 * @param pos Image positions, in px.
	 * @param textPos Text view position, in px.
	 */
	void updatePagePositions (int			number,
				  const QPointF&	pos,
				  const QPointF& 	textPos);

	/**
	 * @brief Removes pages without images (text only)
	 */
	void removePagesWithoutImages ();

	/**
	 * @brief Removes pages with images.
	 */
	void removePagesWithImages ();

private:
	QHash<int, NSRRenderedPage>	_hash;		/**< Pages cache		*/
	QList<int>			_pages;		/**< Pages numbers		*/
	qint64				_usedMemory;	/**< Memory used by pages	*/
};

#endif /* __NSRPAGESCACHE_H__ */
