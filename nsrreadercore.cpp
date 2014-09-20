#include "nsrreadercore.h"
#include "nsrpopplerdocument.h"
#include "nsrdjvudocument.h"
#include "nsrtiffdocument.h"
#include "nsrtextdocument.h"

#include <QVariant>
#include <QFile>
#include <QFileInfo>

#include <float.h>

#define NSR_CORE_MAIN_RENDER_PROP	"nsr-main-render"
#define NSR_CORE_VERSION		"1.4.2"

NSRReaderCore::NSRReaderCore (const INSRSettings *	settings,
			      INSRThumbnailer *		thumbnailer,
			      QObject *			parent) :
	QObject (parent),
	_settings (settings),
	_thumbnailer (thumbnailer),
	_doc (NULL),
	_zoomDoc (NULL),
	_preloadDoc (NULL),
	_thread (NULL),
	_zoomThread (NULL),
	_preloadThread (NULL),
	_cache (NULL),
	_pagesLimit (0),
	_screenWidth (0),
	_isDestructing (false)
{
	_thread		= new NSRRenderThread (this);
	_zoomThread	= new NSRRenderThread (this);
	_preloadThread	= new NSRRenderThread (this);
	_cache		= new NSRPagesCache (this);

	bool ok = connect (_thread, SIGNAL (renderDone ()), this, SLOT (onRenderDone ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_zoomThread, SIGNAL (renderDone ()), this, SLOT (onZoomRenderDone ()));
	Q_ASSERT (ok);

	ok = connect (_zoomThread, SIGNAL (finished ()), this, SLOT (onZoomThreadFinished ()));
	Q_ASSERT (ok);

	ok = connect (_preloadThread, SIGNAL (renderDone ()), this, SLOT (onPreloadRenderDone ()));
	Q_ASSERT (ok);

	ok = connect (_preloadThread, SIGNAL (finished ()), this, SLOT (onPreloadThreadFinished ()));
	Q_ASSERT (ok);
}

NSRReaderCore::~NSRReaderCore ()
{
	disconnect (this);

	_thread->deleteLater ();
	_zoomThread->deleteLater ();
	_preloadThread->deleteLater ();
}

void
NSRReaderCore::prepareForDestruction ()
{
	_isDestructing = true;
	_thread->cancelAllRequests ();
	_thread->setRenderCanceled (true);
	_zoomThread->cancelAllRequests ();
	_zoomThread->setRenderCanceled (true);
	_preloadThread->cancelAllRequests ();
	_preloadThread->setRenderCanceled (true);
}

QString
NSRReaderCore::getVersion ()
{
	return QString (NSR_CORE_VERSION);
}

void
NSRReaderCore::openDocument (const QString &path,  const QString& password)
{
	if (isPageRendering ())
		return;

	closeDocument ();

	_doc = documentByPath (path);

	if (_doc == NULL)
		return;

	_doc->setParent (_thread);
	_doc->setPassword (password);
	_renderRequest = NSRRenderRequest ();

	if (!_doc->isValid ()) {
		emit errorWhileOpening (_doc->getLastError ());
		delete _doc;
		_doc = NULL;
		return;
	}

	if (_settings == NULL) {
		_renderRequest.setInvertColors (false);
		_renderRequest.setEncoding ("");
	} else {
		QString encoding = _settings->isEncodingAutodetection () ? QString ()
									 : _settings->getTextEncoding ();

		_renderRequest.setInvertColors (_settings->isInvertedColors ());
		_renderRequest.setAutoCrop (_settings->isAutoCrop ());
		_renderRequest.setEncoding (encoding);
	}

	if (_settings != NULL && _settings->isStarting ()) {
		if ((_settings->isWordWrap () &&
		    _doc->isDocumentStyleSupported (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT)) ||
		    !_doc->isDocumentStyleSupported (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC))
			_renderRequest.setTextOnly (true);
		else
			_renderRequest.setTextOnly (false);
	} else
		_renderRequest.setTextOnly (_doc->getPrefferedDocumentStyle () == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);

	_thread->setRenderContext (_doc);

	if (_zoomDoc == NULL) {
		_zoomDoc = copyDocument (_doc);
		_zoomDoc->setParent (_zoomThread);
		_zoomThread->setRenderContext (_zoomDoc);
	}

	if (_preloadDoc == NULL) {
		_preloadDoc = copyDocument (_doc);
		_preloadDoc->setParent (_preloadThread);
		_preloadThread->setRenderContext (_preloadDoc);
	}

	emit documentOpened (path);
}

bool
NSRReaderCore::isDocumentOpened () const
{
	if (_doc == NULL)
		return false;

	return _doc->isValid ();
}

void
NSRReaderCore::closeDocument ()
{
	if (isPageRendering ())
		return;

	QString path;

	if (_doc != NULL) {
		path = _doc->getDocumentPath ();
		delete _doc;
		_doc = NULL;
		_thread->setRenderContext (NULL);
	}

	/* Check whether we can delete zoom document */
	if (_zoomDoc != NULL) {
		if (!_zoomThread->isRunning ()) {
			delete _zoomDoc;
			_zoomDoc = NULL;
			_zoomThread->setRenderContext (NULL);
		} else {
			_zoomThread->setRenderCanceled (true);
			_zoomThread->cancelAllRequests ();
		}
	}

	/* Check whether we can delete preload document */
	if (_preloadDoc != NULL) {
		if (!_preloadThread->isRunning ()) {
			delete _preloadDoc;
			_preloadDoc = NULL;
			_preloadThread->setRenderContext (NULL);
		} else {
			_preloadThread->setRenderCanceled (true);
			_preloadThread->cancelAllRequests ();
		}
	}

	_renderRequest = NSRRenderRequest ();
	_currentPage = NSRRenderedPage ();
	_cache->clearStorage ();

	if (!path.isEmpty ())
		emit documentClosed (path);
}

QString
NSRReaderCore::getDocumentPath () const
{
	if (_doc == NULL)
		return QString ();
	else
		return _doc->getDocumentPath ();
}

NSRRenderedPage
NSRReaderCore::getCurrentPage () const
{
	return _currentPage;
}

int
NSRReaderCore::getPagesCount () const
{
	if (_doc == NULL)
		return 0;

	return _doc->getNumberOfPages ();
}

void
NSRReaderCore::reloadSettings ()
{
	if (_doc == NULL || _settings == NULL || isPageRendering ())
		return;

	bool	needReload = false;
	bool	needThumbnail = false;
	bool	wasCropped = _renderRequest.isAutoCrop ();
	QString	wasEncoding = _renderRequest.getEncoding ();
	QString newEncoding = _settings->isEncodingAutodetection () ? QString ()
								    : _settings->getTextEncoding ();

	_renderRequest.setAutoCrop (_settings->isAutoCrop ());
	_renderRequest.setEncoding (newEncoding);

	if (wasCropped != _renderRequest.isAutoCrop ()) {
		/* Do not clear text from cache if text mode is remained */
		if (_renderRequest.isTextOnly ())
			_cache->removePagesWithImages ();
		else {
			_cache->clearStorage ();
			needReload = true;
		}
	}

	if (wasEncoding != _renderRequest.getEncoding () && _doc->isEncodingUsed ()) {
		_cache->clearStorage ();
		needReload = true;
		needThumbnail = true;
	}

	if (needReload)
		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_SETTINGS, _renderRequest.getNumber ());

	if (needThumbnail)
		requestThumbnail ();
}

void
NSRReaderCore::loadSession (const NSRSession *session)
{
	if (isPageRendering ())
		return;

	if (session == NULL)
		return;

	QString file = session->getFile ();

	if (!QFile::exists (file))
		return;

	openDocument (file, session->getPassword ());

	if (isDocumentOpened ()) {
		_renderRequest.setRenderType (NSRRenderRequest::NSR_RENDER_TYPE_PAGE);
		_renderRequest.setRotation (session->getRotation ());
		_renderRequest.setZoom (session->getZoomGraphic ());
		_renderRequest.setZoomToWidth (session->isFitToWidth ());
		_renderRequest.setScreenWidth (_screenWidth);


		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_NAVIGATION, session->getPage ());

		if (_thumbnailer != NULL) {
			if (!_doc->getPassword().isEmpty ())
				_thumbnailer->saveThumbnailEncrypted (_doc->getDocumentPath ());
			else if (_thumbnailer->isThumbnailOutdated (_doc->getDocumentPath ()))
				requestThumbnail ();
		}
	}
}

void
NSRReaderCore::navigateToPage (PageLoad dir, int pageNumber)
{
	loadPage (dir, NSRRenderRequest::NSR_RENDER_REASON_NAVIGATION, pageNumber);
}

bool
NSRReaderCore::isPageRendering () const
{
	return _thread->isRunning () ||
	       (_zoomThread->isRunning () && !_zoomThread->isRenderCanceled () &&
		_zoomThread->property(NSR_CORE_MAIN_RENDER_PROP).toBool ()) ||
	       (_preloadThread->isRunning () && !_preloadThread->isRenderCanceled () &&
		_preloadThread->property(NSR_CORE_MAIN_RENDER_PROP).toBool ());
}

void
NSRReaderCore::setScreenWidth (int width)
{
	if (width <= 0)
		return;

	_screenWidth = width;

	if (isDocumentOpened ())
		_renderRequest.setScreenWidth (width);
}

bool
NSRReaderCore::isFitToWidth () const
{
	return _renderRequest.isZoomToWidth ();
}

double
NSRReaderCore::getZoom () const
{
	return _renderRequest.getZoom ();
}

double
NSRReaderCore::getMinZoom () const
{
	if (!isDocumentOpened ())
		return 0;

	return _doc->getMinZoom ();
}

double
NSRReaderCore::getMaxZoom () const
{
	if (!isDocumentOpened ())
		return 0;

	return _doc->getMaxZoom ();
}

void
NSRReaderCore::setZoom (double zoom, NSRRenderRequest::NSRRenderReason reason)
{
	if (isPageRendering () || zoom <= DBL_EPSILON)
		return;

	bool toWidth = (reason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
			reason == NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH);

	if (!toWidth && qAbs (_renderRequest.getZoom () - zoom) <= DBL_EPSILON)
		return;

	if (_renderRequest.isTextOnly ())
		return;

	if (reason != NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH)
		_cache->clearStorage ();

	_renderRequest.setZoomToWidth (toWidth);
	_renderRequest.setZoom (zoom);

	loadPage (PAGE_LOAD_CUSTOM, reason, _renderRequest.getNumber ());
}

void
NSRReaderCore::rotate (RotateDirection dir)
{
	if (isPageRendering ())
		return;

	NSRAbstractDocument::NSRDocumentRotation rotation = _renderRequest.getRotation ();

	if (dir == ROTATE_DIRECTION_RIGHT)
		rotation = (rotation == NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270 ?
			    NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0 :
			    (NSRAbstractDocument::NSRDocumentRotation) (((int) rotation) + 90));
	else
		rotation = (rotation == NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0 ?
			    NSRAbstractDocument::NSR_DOCUMENT_ROTATION_270 :
			    (NSRAbstractDocument::NSRDocumentRotation) (((int) rotation) - 90));

	_renderRequest.setRotation (rotation);
	_cache->clearStorage ();

	loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_ROTATION, _renderRequest.getNumber ());
}

NSRAbstractDocument::NSRDocumentRotation
NSRReaderCore::getRotation () const
{
	return _renderRequest.getRotation ();
}

void
NSRReaderCore::saveCurrentPagePositions (const QPointF& pos,
					 const QPointF& textPos)
{
	_cache->updatePagePositions (_currentPage.getNumber (), pos, textPos);
}

bool
NSRReaderCore::isTextReflow () const
{
	if (!isDocumentOpened ())
		return false;

	return _renderRequest.isTextOnly ();
}

bool
NSRReaderCore::isTextReflowSwitchSupported () const
{
	if (!isDocumentOpened ())
		return false;

	return (_doc->isDocumentStyleSupported (NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC) &&
		_doc->isDocumentStyleSupported (NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT));
}

void
NSRReaderCore::switchTextReflow ()
{
	if (!isTextReflowSwitchSupported () || isPageRendering ())
		return;

	bool needReload = false;

	/* Check whether we need to re-render the page */
	if (_renderRequest.isTextOnly ()) {
		_cache->removePagesWithoutImages ();
		needReload = true;
	}

	_renderRequest.setTextOnly (!_renderRequest.isTextOnly ());

	if (needReload)
		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_SETTINGS, _renderRequest.getNumber ());
}

void
NSRReaderCore::onRenderDone ()
{
	if (_isDestructing)
		return;

	NSRRenderedPage page = _thread->getRenderedPage ();

	if (_thread->isRenderCanceled ())
		return;

	if (!isPageRelevant (page))
		return;

	_cache->addPage (page);

	if (_renderRequest.getNumber () == page.getNumber ()) {
		_renderRequest.setZoom (page.getRenderedZoom ());
		_currentPage = page;

		emit needIndicator (false);
		emit pageRendered (_renderRequest.getNumber ());
	}

	preloadPages ();
}

void
NSRReaderCore::onZoomRenderDone ()
{
	if (_isDestructing)
		return;

	NSRRenderedPage page = _zoomThread->getRenderedPage ();

	if (_thumbnailer != NULL &&
	    page.getRenderType () == NSRRenderRequest::NSR_RENDER_TYPE_THUMBNAIL) {
		_thumbnailer->saveThumbnail (_zoomDoc->getDocumentPath (), page);
		emit thumbnailRendered ();
		return;
	}

	if (!page.isImageValid () && !_zoomThread->property(NSR_CORE_MAIN_RENDER_PROP).toBool ())
		return;

	/* We do not need to reset document changed flag because it would be
	 * done almost immediately in onZoomThreadFinished() slot */
	if (_zoomThread->isRenderCanceled ())
		return;

	if (!isPageRelevant (page))
		return;

	_cache->addPage (page);

	if (_renderRequest.getNumber () == page.getNumber ()) {
		_renderRequest.setZoom (page.getRenderedZoom ());
		_currentPage = page;

		if (_zoomThread->property(NSR_CORE_MAIN_RENDER_PROP).toBool ()) {
			_zoomThread->setProperty (NSR_CORE_MAIN_RENDER_PROP, false);
			emit needIndicator (false);
		}

		emit pageRendered (_renderRequest.getNumber ());
		preloadPages ();
	}
}

void
NSRReaderCore::onPreloadRenderDone ()
{
	if (_isDestructing)
		return;

	NSRRenderedPage page = _preloadThread->getRenderedPage ();

	if (!page.isValid ())
		return;

	/* We do not need to reset document changed flag because it would be
	 * done almost immediately in onpreloadThreadFinished() slot */
	if (_preloadThread->isRenderCanceled ())
		return;

	if (!isPageRelevant (page))
		return;

	_cache->addPage (page);

	if (_renderRequest.getNumber () == page.getNumber ()) {
		_renderRequest.setZoom (page.getRenderedZoom ());
		_currentPage = page;

		if (_preloadThread->property(NSR_CORE_MAIN_RENDER_PROP).toBool ()) {
			_preloadThread->setProperty (NSR_CORE_MAIN_RENDER_PROP, false);
			emit needIndicator (false);
			preloadPages ();
		}

		emit pageRendered (_renderRequest.getNumber ());
	}
}

void
NSRReaderCore::onZoomThreadFinished ()
{
	if (_isDestructing)
		return;

	if (_zoomThread->isRenderCanceled ()) {
		/* All requests must be canceled on document opening */
		delete _zoomDoc;
		_zoomDoc = copyDocument (_doc);
		_zoomDoc->setParent (_zoomThread);
		_zoomThread->setRenderContext (_zoomDoc);
		_zoomThread->setRenderCanceled (false);
	}

	if (_zoomThread->hasAnyRequests ())
		_zoomThread->start (QThread::LowestPriority);
}

void
NSRReaderCore::onPreloadThreadFinished ()
{
	if (_isDestructing)
		return;

	if (_preloadThread->isRenderCanceled ()) {
		/* All requests must be canceled on document opening */
		delete _preloadDoc;
		_preloadDoc = copyDocument (_doc);
		_preloadDoc->setParent (_preloadThread);
		_preloadThread->setRenderContext (_preloadDoc);
		_preloadThread->setRenderCanceled (false);
	}

	if (_preloadThread->hasAnyRequests ())
		_preloadThread->start (QThread::LowestPriority);
}

void
NSRReaderCore::loadPage (PageLoad				dir,
			 NSRRenderRequest::NSRRenderReason	reason,
			 int					page)
{
	if (!isDocumentOpened () || (isPageRendering () && reason != NSRRenderRequest::NSR_RENDER_REASON_PRELOAD))
		return;

	int pageToLoad = _renderRequest.getNumber ();

	switch (dir) {
	case PAGE_LOAD_PREV:
		pageToLoad -= 1;
		break;
	case PAGE_LOAD_NEXT:
		pageToLoad += 1;
		break;
	case PAGE_LOAD_CUSTOM:
		pageToLoad = page;
		break;
	default:
		break;
	}

	if (pageToLoad < 1)
		pageToLoad = 1;
	else if (pageToLoad > _doc->getNumberOfPages ())
		pageToLoad = _doc->getNumberOfPages ();

	if (_pagesLimit > 0 && pageToLoad > _pagesLimit) {
		pageToLoad = _pagesLimit;
		emit pagesLimitPassed ();
	}

	if (reason != NSRRenderRequest::NSR_RENDER_REASON_PRELOAD)
		_renderRequest.setNumber (pageToLoad);

	NSRRenderRequest req (_renderRequest);
	req.setRenderReason (reason);

	if (reason == NSRRenderRequest::NSR_RENDER_REASON_PRELOAD)
		req.setNumber (pageToLoad);

	if (reason != NSRRenderRequest::NSR_RENDER_REASON_PRELOAD &&
	    reason != NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH &&
	    _cache->isPageExists (pageToLoad)) {
		QString suffix = QFileInfo(_doc->getDocumentPath ()).suffix().toLower ();

		_currentPage = _cache->getPage (pageToLoad);
		emit pageRendered (pageToLoad);
		preloadPages ();

		return;
	}

	NSRRenderRequest::NSRRenderReason reqReason = req.getRenderReason ();

	if (reqReason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM ||
	    reqReason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
	    reqReason == NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH) {
		_preloadThread->cancelAllRequests ();
		_zoomThread->cancelRequests (NSRRenderRequest::NSR_RENDER_TYPE_PAGE);

		if (_zoomThread->isRunning ()) {
			NSRRenderRequest zoomReq = _zoomThread->getCurrentRequest ();

			if (!_zoomThread->isRenderCanceled () && isPageRelevant (zoomReq) &&
			    zoomReq.getNumber () == req.getNumber ())
				return;
			else
				_zoomThread->addRequest (req);
		} else {
			_zoomThread->addRequest (req);
			_zoomThread->start (QThread::LowestPriority);
		}
	} else if (reqReason == NSRRenderRequest::NSR_RENDER_REASON_PRELOAD) {
		req.setRenderReason (NSRRenderRequest::NSR_RENDER_REASON_NAVIGATION);

		/* Do not cancel preload requests because we can add several requests at once */
		_preloadThread->addRequest (req);

		if (!_preloadThread->isRunning ())
			_preloadThread->start (QThread::LowestPriority);
	} else {
		emit needIndicator (true);

		_zoomThread->cancelRequests (NSRRenderRequest::NSR_RENDER_TYPE_PAGE);
		_thread->cancelAllRequests ();

		/* OK, we are already preloading required page */
		if (_preloadThread->isRunning ()) {
			NSRRenderRequest preloadReq = _preloadThread->getCurrentRequest ();

			if (!_preloadThread->isRenderCanceled () && isPageRelevant (preloadReq) &&
			    preloadReq.getNumber () == req.getNumber ()) {
				_preloadThread->setProperty (NSR_CORE_MAIN_RENDER_PROP, true);
				return;
			}
		}

		if (_zoomThread->isRunning ()) {
			NSRRenderRequest zoomReq = _zoomThread->getCurrentRequest ();

			if (!_zoomThread->isRenderCanceled () && isPageRelevant (zoomReq) &&
			    zoomReq.getNumber () == req.getNumber ()) {
				_zoomThread->setProperty (NSR_CORE_MAIN_RENDER_PROP, true);
				return;
			}
		}

		_preloadThread->cancelAllRequests ();
		_thread->addRequest (req);

		if (!_thread->isRunning ())
			_thread->start (QThread::NormalPriority);
	}
}

NSRAbstractDocument*
NSRReaderCore::copyDocument (const NSRAbstractDocument* doc)
{
	NSRAbstractDocument *res;

	if (!isDocumentOpened ())
		return NULL;

	res = documentByPath (doc->getDocumentPath ());

	if (res == NULL)
		return NULL;

	res->setPassword (doc->getPassword ());

	if (!res->isValid ()) {
		delete res;
		res = NULL;
	}

	return res;
}

NSRAbstractDocument*
NSRReaderCore::documentByPath (const QString& path) const
{
	NSRAbstractDocument	*res = NULL;
	QString			suffix = QFileInfo(path).suffix().toLower ();

	if (!QFile::exists (path))
		return NULL;

	if (suffix == "pdf")
		res = new NSRPopplerDocument (path);
	else if (suffix == "djvu" || suffix == "djv")
		res = new NSRDjVuDocument (path);
	else if (suffix == "tiff" || suffix == "tif")
		res = new NSRTIFFDocument (path);
	else
		res = new NSRTextDocument (path);

	return res;
}

bool
NSRReaderCore::isPasswordProtected (const QString& file) const
{
	QString			suffix = QFileInfo(file).suffix().toLower ();
	bool			res = false;

	if (suffix != "pdf")
		return res;

	NSRAbstractDocument *doc = documentByPath (file);

	if (doc->getLastError () == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
		res = true;

	delete doc;
	return res;
}

void
NSRReaderCore::invertColors ()
{
	if (!isDocumentOpened () || isPageRendering ())
		return;

	bool needReload = false;

	_renderRequest.setInvertColors (!_renderRequest.isInvertColors ());

	/* Do not clear text from cache if text mode is remained */
	if (_renderRequest.isTextOnly ())
		_cache->removePagesWithImages ();
	else {
		_cache->clearStorage ();
		needReload = true;
	}

	if (needReload)
		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_SETTINGS, _renderRequest.getNumber ());
}

bool
NSRReaderCore::isInvertedColors () const
{
	if (!isDocumentOpened ())
		return false;

	return _renderRequest.isInvertColors ();
}

bool
NSRReaderCore::isPageRelevant (const NSRRenderedPage& page) const
{
	if (_doc == NULL)
		return false;

	bool relevant = _renderRequest.isAutoCrop () == page.isAutoCrop () &&
			_renderRequest.isTextOnly () == page.isTextOnly () &&
			_renderRequest.isInvertColors () == page.isInvertColors () &&
			_renderRequest.isZoomToWidth () == page.isZoomToWidth () &&
			_renderRequest.getRenderType () == page.getRenderType () &&
			_renderRequest.getRotation () == page.getRotation ();

	if (_doc->isEncodingUsed ())
		relevant = relevant && (_renderRequest.getEncoding () == page.getEncoding ());

	if (!_renderRequest.isZoomToWidth ())
			relevant = relevant && qAbs (_renderRequest.getZoom () - page.getZoom ()) <= DBL_EPSILON;

	return relevant;
}

void
NSRReaderCore::preloadPages ()
{
	if (!isDocumentOpened ())
		return;

	int pageToLoadNext = qMin (_doc->getNumberOfPages (), _renderRequest.getNumber () + 1);
	int pageToLoadPrev = qMax (1, _renderRequest.getNumber () - 1);

	bool needNext = !_cache->isPageExists (pageToLoadNext) && pageToLoadNext != _renderRequest.getNumber ();
	bool needPrev = !_cache->isPageExists (pageToLoadPrev) && pageToLoadPrev != _renderRequest.getNumber ();

	if (_pagesLimit > 0 && pageToLoadNext > _pagesLimit)
		needNext = false;

	if (!needNext && !needPrev)
		return;
	else
		_preloadThread->cancelAllRequests ();

	if (_preloadThread->isRunning () && !_preloadThread->isRenderCanceled ()) {
		NSRRenderRequest preloadReq = _preloadThread->getCurrentRequest ();

		if (needNext && isPageRelevant (preloadReq) && preloadReq.getNumber () == pageToLoadNext)
			needNext = false;

		if (needPrev && isPageRelevant (preloadReq) && preloadReq.getNumber () == pageToLoadPrev)
			needPrev = false;
	}

	if (needNext)
		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_PRELOAD, pageToLoadNext);

	if (needPrev)
		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_PRELOAD, pageToLoadPrev);
}

void
NSRReaderCore::requestThumbnail ()
{
	if (!isDocumentOpened () || _thumbnailer == NULL)
		return;

	NSRRenderRequest thumbRequest (1, NSRRenderRequest::NSR_RENDER_REASON_THUMBNAIL);

	thumbRequest.setRenderType (NSRRenderRequest::NSR_RENDER_TYPE_THUMBNAIL);
	thumbRequest.setEncoding (_renderRequest.getEncoding ());
	thumbRequest.setAutoCrop (false);
	thumbRequest.setInvertColors (false);
	thumbRequest.setTextOnly (false);
	thumbRequest.setZoomToWidth (true);
	thumbRequest.setRotation (NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0);
	thumbRequest.setScreenWidth (_thumbnailer->getThumbnailWidth ());

	_zoomThread->cancelRequests (NSRRenderRequest::NSR_RENDER_TYPE_THUMBNAIL);
	_zoomThread->addRequest (thumbRequest);

	if (!_zoomThread->isRunning ())
		_zoomThread->start (QThread::LowestPriority);
}
