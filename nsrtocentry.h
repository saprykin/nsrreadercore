#ifndef __NSRTOCENTRY_H__
#define __NSRTOCENTRY_H__

/**
 * @file nsrtocentry.h
 * @author Alexander Saprykin
 * @brief Table of contents (TOC)
 */

#include <QString>
#include <QList>

/**
 * @class NSRTocEntry nsrtocentry.h
 * @brief TOC representation
 * @since 1.5.2
 */
class NSRTocEntry
{
public:
	/**
	 * @brief Constructor with parameters
	 * @param title Entry title.
	 * @param page Reference page, from 1.
	 */
	NSRTocEntry (const QString& title, int page);

	/** Destructor */
	~NSRTocEntry ();

	/**
	 * @brief Gets list of available children TOC entries
	 * @return Children TOC entries.
	 * @since 1.5.2
	 */
	QList<const NSRTocEntry *> getChildren () const;

	/**
	 * @brief Appends child TOC entry
	 * @param entry Child TOC entry to append.
	 * @since 1.5.2
	 *
	 * Takes ownership of given object.
	 */
	void appendChild (NSRTocEntry *entry);

	/**
	 * @brief Gets entry title
	 * @return Entry title.
	 * @since 1.5.2
	 */
	inline QString getTitle () const {
		return _title;
	}

	/**
	 * @brief Gets external file path
	 * @return External file path.
	 * @since 1.5.2
	 */
	inline QString getExternalFile () const {
		return _externalFile;
	}

	/**
	 * @brief Gets external URI
	 * @return External URI.
	 * @since 1.5.2
	 */
	inline QString getUri () const {
		return _uri;
	}

	/**
	 * @brief Gets page number
	 * @return Page number.
	 * @since 1.5.2
	 */
	inline int getPage () const {
		return _page;
	}

	/**
	 * @brief Checks whether entry is referenced to external resource
	 * @return True in case of success, false otherwise.
	 * @since 1.5.2
	 */
	inline bool isExternal () const {
		return _isExternal;
	}

	/**
	 * @brief Sets entry title
	 * @param title Entry title.
	 * @since 1.5.2
	 */
	inline void setTitle (const QString& title) {
		_title = title;
	}

	/**
	 * @brief Sets external file path.
	 * @param externalFile External file path.
	 * @since 1.5.2
	 */
	inline void setExternalFile (const QString& externalFile) {
		_externalFile = externalFile;
	}

	/**
	 * @brief Sets external URI
	 * @param uri External URI.
	 * @since 1.5.2
	 */
	inline void setUri (const QString& uri) {
		_uri = uri;
	}

	/**
	 * @brief Sets page number
	 * @param page Page number, from 1.
	 * @since 1.5.2
	 */
	inline void setPage (int page) {
		_page = page;
	}

	/**
	 * @brief Sets entry as external reference
	 * @param external Whether the entry is external reference.
	 * @since 1.5.2
	 */
	inline void setExternal (bool external) {
		_isExternal = external;
	}

private:
	QList<NSRTocEntry *>	_children;	/**< Children entries if any				*/
	QString			_title;		/**< TOC entry title					*/
	QString			_externalFile;	/**< External file in case of external reference	*/
	QString			_uri;		/**< URI in case of destination URI			*/
	int			_page;		/**< Page number, from 1				*/
	bool			_isExternal;	/**< Whether reference is external			*/
};

#endif /* __NSRTOCENTRY_H__ */
