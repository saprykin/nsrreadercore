#ifndef NSRREADERCORE_H_
#define NSRREADERCORE_H_

#include <QObject.h>

#include "nsrabstractdocument.h"
#include "nsrrenderthread.h"
#include "nsrrenderzoomthread.h"
#include "nsrrenderrequest.h"
#include "nsrpagescache.h"
#include "nsrsession.h"
#include "nsrsettings.h"

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

	NSRReaderCore (bool isCardMode, QObject *parent = 0);
	virtual ~NSRReaderCore ();

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
	void rotate (double rot);
	double getRotation () const;
	void saveCurrentPagePositions (const QPointF& pos,
				       const QPointF& textPos);
	bool isTextReflow () const;
	bool isTextReflowSwitchSupported () const;
	void switchTextReflow ();
	bool isPasswordProtected (const QString& file) const;
	void invertColors ();
	bool isInvertedColors () const;

Q_SIGNALS:
	void pageRendered (int number);
	void needIndicator (bool enabled);
	void errorWhileOpening (NSRAbstractDocument::NSRDocumentError error);
	void needViewMode (NSRAbstractDocument::NSRDocumentStyle mode);
	void documentOpened (const QString& file);
	void documentClosed (const QString& file);
#ifdef NSR_CORE_LITE_VERSION
	void liteVersionOverPage ();
#endif

private Q_SLOTS:
	void onRenderDone ();
	void onZoomRenderDone ();
	void onZoomThreadFinished ();

private:
	void loadPage (PageLoad					dir,
		       NSRRenderRequest::NSRRenderReason	reason,
		       int page					= 0);
	NSRAbstractDocument * copyDocument (const NSRAbstractDocument *doc);
	NSRAbstractDocument * documentByPath (const QString& path) const;
	double normalizeAngle (double angle) const;

	NSRAbstractDocument				*_doc;
	NSRAbstractDocument				*_zoomDoc;
	NSRRenderThread					*_thread;
	NSRRenderZoomThread				*_zoomThread;
	NSRPagesCache					*_cache;
	NSRRenderedPage					_currentPage;
	NSRRenderRequest				_renderRequest;
	bool						_isCardMode;
};

#endif /* NSRREADERCORE_H_ */
