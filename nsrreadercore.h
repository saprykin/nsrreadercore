#ifndef __NSRREADERCORE_H__
#define __NSRREADERCORE_H__

/**
 * @file nsrreadercore.h
 * @author Alexander Saprykin
 * @brief Rendering engine API
 */

#include "interfaces/insrsettings.h"
#include "interfaces/insrthumbnailer.h"
#include "nsrabstractdocument.h"
#include "nsrrenderthread.h"
#include "nsrrenderrequest.h"
#include "nsrpagescache.h"
#include "nsrcroppadscache.h"
#include "nsrsession.h"
#include "nsrreadercore_global.h"

#include <QObject.h>

/**
 * @class NSRReaderCore nsrreadercore.h
 * @brief Main render class
 */
class NSRREADERCORE_SHARED NSRReaderCore: public QObject
{
	Q_OBJECT
	Q_ENUMS (PageLoad)
public:
	/** Page load direction */
	enum PageLoad {
		PAGE_LOAD_PREV		= 0,	/**< Previous page	*/
		PAGE_LOAD_NEXT		= 1,	/**< Next page		*/
		PAGE_LOAD_CUSTOM	= 2	/**< Custom page	*/
	};

	/** Rotation direction */
	enum RotateDirection {
		ROTATE_DIRECTION_RIGHT	= 0,	/**< 90 degrees cw	*/
		ROTATE_DIRECTION_LEFT	= 1	/**< 90 degrees ccw	*/
	};

	/**
	 * @brief Constructor with parameters
	 * @param settings #INSRSettings implementation.
	 * @param thumbnailer #INSRThumbnailer implementation.
	 * @param parent Parent object.
	 */
	NSRReaderCore (const INSRSettings *	settings,
		       INSRThumbnailer *	thumbnailer,
		       QObject *		parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRReaderCore ();

	/**
	 * @brief Prepares engine object for destruction
	 *
	 * Stops all rendering threads and tries to minimize resources
	 * usage as soon as possible.
	 */
	void prepareForDestruction ();

	/**
	 * @brief Checks whether engine object is preparing for
	 * destruction
	 * @return True if engine object is preparing for destruction,
	 * false otherwise.
	 */
	inline bool isDestructing () const {
		return _isDestructing;
	}

	/**
	 * @brief Gets engine version
	 * @return Engine version.
	 */
	static QString getVersion ();

	/**
	 * @brief Loads file session
	 * @param session Session to load.
	 */
	void loadSession (const NSRSession *session);

	/**
	 * @brief Resets loaded session.
	 */
	void resetSession ();

	/**
	 * @brief Checks whether session currently is loaded
	 * @return True is session is loaded, false otherwise.
	 */
	bool isSessionLoaded () const;

	/**
	 * @brief Gets file path of loaded session
	 * @return File path of loaded session.
	 */
	QString getSessionFile () const;

	/**
	 * @brief Reloads rendering settings
	 *
	 * Initiates page re-rendering if need.
	 */
	void reloadSettings ();

	/**
	 * @brief Gets pages count
	 * @return Pages count if session is loaded, 0 otherwise.
	 */
	int getPagesCount () const;

	/**
	 * @brief Gets last rendered page of loaded session
	 * @return Last rendered page of loaded session.
	 */
	NSRRenderedPage getCurrentPage () const;

	/**
	 * @brief Navigates to selected page
	 * @param dir Page load direction.
	 * @param pageNumber Page number if need.
	 */
	void navigateToPage (PageLoad dir, int pageNumber = 0);

	/**
	 * @brief Checks whether page is currently rendering
	 * @return True if page is rendering, false otherwise.
	 */
	bool isPageRendering () const;

	/**
	 * @brief Sets device screen width
	 * @param width Screen width, in px.
	 */
	void setScreenWidth (int width);

	/**
	 * @brief Checks whether pages are rendering fitted to screen width
	 * @return True if pages are rendering fitted to screen width, false
	 * otherwise.
	 * @note Use setZoom() to activate fit to width.
	 */
	bool isFitToWidth () const;

	/**
	 * @brief Gets zoom value
	 * @return Zoom value, in %.
	 */
	double getZoom () const;

	/**
	 * @brief Sets zoom for current session
	 * @param zoom Zoom value, in %.
	 * @param reason Rendering reason.
	 */
	void setZoom (double zoom, NSRRenderRequest::NSRRenderReason reason);

	/**
	 * @brief Gets rotation for current session
	 * @return Rotation for current session.
	 */
	NSRAbstractDocument::NSRDocumentRotation getRotation () const;

	/**
	 * @brief Rotates pages for current session
	 * @param dir Rotation direction.
	 */
	void rotate (RotateDirection dir);

	/**
	 * @brief Saves current page scrolling positions
	 * @param pos Image position.
	 * @param textPos Text view position.
	 */
	void saveCurrentPagePositions (const QPointF& pos,
				       const QPointF& textPos);

	/**
	 * @brief Checks whether text reflow is enabled
	 * @return True if text reflow is enabled, false otherwise.
	 */
	bool isTextReflow () const;

	/**
	 * @brief Checks whether text reflow is supported for current
	 * session
	 * @return True if text reflow is supported, false otherwise.
	 */
	bool isTextReflowSwitchSupported () const;

	/**
	 * @brief Switches text reflow for current session
	 */
	void switchTextReflow ();

	/**
	 * @brief Checks whether file is password protected
	 * @param file Path to file.
	 * @return True if file is password protected, false
	 * otherwise.
	 */
	bool isPasswordProtected (const QString& file) const;

	/**
	 * @brief Checks whether current session has inverted colors
	 * @return True if session has inverted colors, false otherwise.
	 */
	bool isInvertedColors () const;

	/**
	 * @brief Switches inverted colors for current session
	 */
	void switchInvertedColors ();

	/**
	 * @brief Gets pages limit engine can load
	 * @return Pages limit engine can load.
	 *
	 * It defines maximum page number engine can load. It is useful
	 * for evaluation version.
	 */
	inline int getPagesLimit () const {
		return _pagesLimit;
	}

	/**
	 * @brief Sets pages limit engine can load
	 * @param limit Pages limit engine can load.
	 */
	inline void setPagesLimit (int limit) {
		_pagesLimit = limit;
	}

	/**
	 * @brief Gets pages cache size
	 * @return Pages cache size, bytes.
	 * @since 1.4.3
	 */
	qint64 getCacheSize () const;

	/**
	 * @brief Sets pages cache size
	 * @param cacheSize Pages cache size, bytes.
	 * @since 1.4.3
	 */
	void setCacheSize (qint64 cacheSize);

Q_SIGNALS:
	/**
	 * @brief Page rendered
	 * @param number Page number.
	 */
	void pageRendered (int number);

	/**
	 * @brief Session thumbnail rendered
	 */
	void thumbnailRendered ();

	/**
	 * @brief Possible long async action is performing
	 * @param enabled Whether to show indicator for UI.
	 */
	void needIndicator (bool enabled);

	/**
	 * @brief Error while opening file
	 * @param error Error.
	 */
	void errorWhileOpening (NSRAbstractDocument::NSRDocumentError error);

	/**
	 * @brief Session file opened
	 * @param file Path to file.
	 */
	void sessionFileOpened (const QString& file);

	/**
	 * @brief Session file closed
	 * @param file Path to file.
	 */
	void sessionFileClosed (const QString& file);

	/**
	 * @brief Engine detected attempt to pass pages limit
	 */
	void pagesLimitPassed ();

private Q_SLOTS:
	/**
	 * @brief On page rendering finished
	 */
	void onRenderDone ();

	/**
	 * @brief On page zooming finished
	 */
	void onZoomRenderDone ();

	/**
	 * @brief On page preloading finished
	 */
	void onPreloadRenderDone ();

	/**
	 * @brief On zoom thread finished
	 */
	void onZoomThreadFinished ();

	/**
	 * @brief On preload thread finished
	 */
	void onPreloadThreadFinished ();

private:
	/**
	 * @brief Opens session file
	 * @param path Path to file.
	 * @param password File password (if any).
	 */
	void openFile (const QString &path, const QString& password = QString ());

	/**
	 * @brief Closes file of current session
	 */
	void closeFile ();

	/**
	 * @brief Checks whether session file is opened
	 * @return True if session file is opened, false otherwise.
	 */
	bool isFileOpened () const;

	/**
	 * @brief Loads page.
	 * @param dir Loading direction.
	 * @param reason Rendering reason.
	 * @param page Page number (if need), from 1.
	 */
	void loadPage (PageLoad					dir,
		       NSRRenderRequest::NSRRenderReason	reason,
		       int					page = 0);

	/**
	 * @brief Copies file handler
	 * @param doc File handler to copy.
	 * @return Copy of the given file handler in case of success,
	 * NULL otherwise.
	 */
	NSRAbstractDocument * copyFileHandler (const NSRAbstractDocument *doc);

	/**
	 * @brief Creates file handler by path
	 * @param path Path to file.
	 * @param passwd File password if any.
	 * @return New file handler in case of success, NULL otherwise.
	 */
	NSRAbstractDocument * fileHandlerByPath (const QString& path, const QString& passwd = QString ()) const;

	/**
	 * @brief Checks whether rendered page is still relevant
	 * @param page Rendered page.
	 * @return True if rendered page is relevant, false otherwise.
	 */
	bool isPageRelevant (const NSRRenderedPage& page) const;

	/**
	 * @brief Preloads pages from current one into cache
	 */
	void preloadPages ();

	/**
	 * @brief Requests thumbnail rendering for current session
	 */
	void requestThumbnail ();

	const INSRSettings *	_settings;	/**< Rendering settings		*/
	INSRThumbnailer *	_thumbnailer;	/**< Thumbnailer		*/
	NSRAbstractDocument *	_doc;		/**< Main file handler		*/
	NSRAbstractDocument *	_zoomDoc;	/**< Zoom handler		*/
	NSRAbstractDocument *	_preloadDoc;	/**< Preload handler		*/
	NSRRenderThread	*	_thread;	/**< Main render thread		*/
	NSRRenderThread	*	_zoomThread;	/**< Zoom render thread		*/
	NSRRenderThread	*	_preloadThread;	/**< Preload render thread	*/
	NSRPagesCache *		_cache;		/**< Pages cache		*/
	NSRCropPadsCache *	_cropPadsCache;	/**< Page crop pads cache	*/
	NSRRenderedPage		_currentPage;	/**< Last rendered page		*/
	NSRRenderRequest	_renderRequest;	/**< Session rendering request	*/
	int			_pagesLimit;	/**< Pages limit		*/
	int			_screenWidth;	/**< Device screen width	*/
	bool			_isDestructing;	/**< Destructing flag		*/
};

#endif /* __NSRREADERCORE_H__ */
