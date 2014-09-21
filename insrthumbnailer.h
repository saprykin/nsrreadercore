#ifndef __INSRTHUMBNAILER_H__
#define __INSRTHUMBNAILER_H__

/**
 * @file insrthumbnailer.h
 * @author Alexander Saprykin
 * @brief Interface for thumbnailer
 */

#include "nsrreadercore_global.h"
#include "nsrrenderedpage.h"

#include <QString>

/**
 * @class INSRThumbnailer insrthumbnailer.h
 * @brief Interface for thumbnailer
 *
 * #INSRThumbnailer interface represents thumbnailer for opened documents.
 */
class NSRREADERCORE_SHARED INSRThumbnailer
{
public:
	/** Destructor */
	virtual ~INSRThumbnailer () {}

	/**
	 * @brief Saves thumbnail of rendered page
	 * @param path Path of the file for which thumbnail was rendered.
	 * @param page Rendered page (thumbnail).
	 * @since 1.4.2
	 */
	virtual void saveThumbnail (const QString&		path,
				    const NSRRenderedPage&	page) = 0;

	/**
	 * @brief Saves empty thumbnail for encrypted file
	 * @param Path of the encrypted file.
	 * @since 1.4.2
	 */
	virtual void saveThumbnailEncrypted (const QString& path) = 0;

	/**
	 * @brief Checks whether file has outdated thumbnail
	 * @param Path of the file to check thumbnail for.
	 * @return True if file has outdated thumbnail (or no thumbnail),
	 * false otherwise.
	 * @since 1.4.2
	 */
	virtual bool isThumbnailOutdated (const QString& path) const = 0;

	/**
	 * @brief Gets thumbnail width to render
	 * @return Thumbnail width, px.
	 * @since 1.4.2
	 */
	virtual int getThumbnailWidth () const = 0;
};

#endif /* __INSRTHUMBNAILER_H__ */
