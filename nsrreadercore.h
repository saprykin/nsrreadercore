#ifndef NSRREADERCORE_H_
#define NSRREADERCORE_H_

#include <QObject.h>

#include "insrsettings.h"
#include "insrthumbnailer.h"
#include "nsrabstractdocument.h"
#include "nsrrenderthread.h"
#include "nsrrenderrequest.h"
#include "nsrpagescache.h"
#include "nsrsession.h"

class NSRReaderCore: public QObject
{
	Q_OBJECT
	Q_ENUMS (PageLoad)
public:
	enum PageLoad {
		PAGE_LOAD_PREV		= 0,
		PAGE_LOAD_NEXT		= 1,
		PAGE_LOAD_CUSTOM	= 2
	};

	enum RotateDirection {
		ROTATE_DIRECTION_RIGHT	= 0,
		ROTATE_DIRECTION_LEFT	= 1
	};

	NSRReaderCore (const INSRSettings *	settings,
		       INSRThumbnailer *	thumbnailer,
		       QObject *		parent = 0);
	virtual ~NSRReaderCore ();
	void prepareForDestruction ();
	inline bool isDestructing () const {
		return _isDestructing;
	}

	static QString getVersion ();

	void openDocument (const QString &path, const QString& password = QString ());
	bool isDocumentOpened () const;
	void closeDocument ();
	QString getDocumentPath () const;

	NSRRenderedPage getCurrentPage () const;
	int getPagesCount () const;
	void reloadSettings ();
	void loadSession (const NSRSession *session);
	void navigateToPage (PageLoad dir, int pageNumber = 0);
	bool isPageRendering () const;
	void setScreenWidth (int width);
	bool isFitToWidth () const;
	double getZoom () const;
	double getMinZoom () const;
	double getMaxZoom () const;
	void setZoom (double zoom, NSRRenderRequest::NSRRenderReason reason);
	void rotate (RotateDirection dir);
	NSRAbstractDocument::NSRDocumentRotation getRotation () const;
	void saveCurrentPagePositions (const QPointF& pos,
				       const QPointF& textPos);
	bool isTextReflow () const;
	bool isTextReflowSwitchSupported () const;
	void switchTextReflow ();
	bool isPasswordProtected (const QString& file) const;
	void invertColors ();
	bool isInvertedColors () const;

	inline void setPagesLimit (int limit) {
		_pagesLimit = limit;
	}
	inline int getPagesLimit () const {
		return _pagesLimit;
	}

Q_SIGNALS:
	void pageRendered (int number);
	void thumbnailRendered ();
	void needIndicator (bool enabled);
	void errorWhileOpening (NSRAbstractDocument::NSRDocumentError error);
	void documentOpened (const QString& file);
	void documentClosed (const QString& file);
	void pagesLimitPassed ();

private Q_SLOTS:
	void onRenderDone ();
	void onZoomRenderDone ();
	void onPreloadRenderDone ();
	void onZoomThreadFinished ();
	void onPreloadThreadFinished ();

private:
	void loadPage (PageLoad					dir,
		       NSRRenderRequest::NSRRenderReason	reason,
		       int page					= 0);
	NSRAbstractDocument * copyDocument (const NSRAbstractDocument *doc);
	NSRAbstractDocument * documentByPath (const QString& path) const;
	bool isPageRelevant (const NSRRenderedPage& page) const;
	void preloadPages ();
	void requestThumbnail ();

	const INSRSettings *	_settings;
	INSRThumbnailer *	_thumbnailer;
	NSRAbstractDocument *	_doc;
	NSRAbstractDocument *	_zoomDoc;
	NSRAbstractDocument *	_preloadDoc;
	NSRRenderThread	*	_thread;
	NSRRenderThread	*	_zoomThread;
	NSRRenderThread	*	_preloadThread;
	NSRPagesCache *		_cache;
	NSRRenderedPage		_currentPage;
	NSRRenderRequest	_renderRequest;
	int			_pagesLimit;
	bool			_isDestructing;
};

#endif /* NSRREADERCORE_H_ */
