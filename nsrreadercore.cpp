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
#define NSR_CORE_VERSION		"1.4.3"

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
	_cropPadsCache (NULL),
	_pagesLimit (0),
	_screenWidth (0),
	_isDestructing (false)
{
	_thread		= new NSRRenderThread (this);
	_zoomThread	= new NSRRenderThread (this);
	_preloadThread	= new NSRRenderThread (this);
	_cache		= new NSRPagesCache (this);
	_cropPadsCache	= new NSRCropPadsCache (this);

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
NSRReaderCore::loadSession (const NSRSession *session)
{
	if (isPageRendering ())
		return;

	if (session == NULL)
		return;

	QString file = session->getFile ();

	if (!QFile::exists (file))
		return;

	_renderRequest = NSRRenderRequest ();

	openFile (file, session->getPassword ());

	if (isFileOpened ()) {
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
			_renderRequest.setTextOnly (_doc->getPreferredDocumentStyle () == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);

		_renderRequest.setRenderType (NSRRenderRequest::NSR_RENDER_TYPE_PAGE);
		_renderRequest.setRotation (session->getRotation ());
		_renderRequest.setZoom (session->getZoomGraphic ());
		_renderRequest.setZoomToWidth (session->isFitToWidth ());
		_renderRequest.setScreenWidth (_screenWidth);

		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_NAVIGATION, session->getPage ());

		if (_thumbnailer != NULL) {
			if (!_doc->getPassword().isEmpty ()) {
				_thumbnailer->saveThumbnailEncrypted (_doc->getDocumentPath ());
				emit thumbnailRendered ();
			} else if (_thumbnailer->isThumbnailOutdated (_doc->getDocumentPath ()))
				requestThumbnail ();
		}
	}
}

void
NSRReaderCore::resetSession ()
{
	if (isPageRendering ())
		return;

	if (isFileOpened ())
		closeFile ();

	_renderRequest = NSRRenderRequest ();
}

bool
NSRReaderCore::isSessionLoaded () const
{
	return isFileOpened ();
}

QString
NSRReaderCore::getSessionFile () const
{
	if (_doc == NULL)
		return QString ();
	else
		return _doc->getDocumentPath ();
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

int
NSRReaderCore::getPagesCount () const
{
	if (_doc == NULL)
		return 0;

	return _doc->getPagesCount ();
}

NSRRenderedPage
NSRReaderCore::getCurrentPage () const
{
	return _currentPage;
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

	if (isFileOpened ())
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

void
NSRReaderCore::setZoom (double zoom, NSRRenderRequest::NSRRenderReason reason)
{
	if (isPageRendering () || zoom <= DBL_EPSILON)
		return;

	bool toWidth = (reason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH);

	if (!toWidth && qAbs (_renderRequest.getZoom () - zoom) <= DBL_EPSILON)
		return;

	if (_renderRequest.isTextOnly ())
		return;

	_cache->clearStorage ();

	_renderRequest.setZoomToWidth (toWidth);
	_renderRequest.setZoom (zoom);

	loadPage (PAGE_LOAD_CUSTOM, reason, _renderRequest.getNumber ());
}

NSRAbstractDocument::NSRDocumentRotation
NSRReaderCore::getRotation () const
{
	return _renderRequest.getRotation ();
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

void
NSRReaderCore::saveCurrentPagePositions (const QPointF& pos,
					 const QPointF& textPos)
{
	_cache->updatePagePositions (_currentPage.getNumber (), pos, textPos);
}

bool
NSRReaderCore::isTextReflow () const
{
	if (!isFileOpened ())
		return false;

	return _renderRequest.isTextOnly ();
}

bool
NSRReaderCore::isTextReflowSwitchSupported () const
{
	if (!isFileOpened ())
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

bool
NSRReaderCore::isPasswordProtected (const QString& file) const
{
	QString	suffix = QFileInfo(file).suffix().toLower ();
	bool	res = false;

	if (suffix != "pdf")
		return res;

	NSRAbstractDocument *doc = fileHandlerByPath (file);

	if (doc->getLastError () == NSRAbstractDocument::NSR_DOCUMENT_ERROR_PASSWD)
		res = true;

	delete doc;
	return res;
}

bool
NSRReaderCore::isInvertedColors () const
{
	if (!isFileOpened ())
		return false;

	return _renderRequest.isInvertColors ();
}

void
NSRReaderCore::switchInvertedColors ()
{
	if (!isFileOpened () || isPageRendering ())
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

qint64 NSRReaderCore::getCacheSize () const
{
	return _cache->getMaxMemory ();
}

void NSRReaderCore::setCacheSize (qint64 cacheSize)
{
	_cache->setMaxMemory (cacheSize);
}

void
NSRReaderCore::onRenderDone ()
{
	if (_isDestructing)
		return;

	NSRRenderedPage page = _thread->getRenderedPage ();

	if (_thread->isRenderCanceled ())
		return;

	if (page.getCropPads().isDetected () && !_cropPadsCache->isCropPadsExist (page.getNumber ()))
		_cropPadsCache->addCropPads (page.getNumber (), page.getCropPads ());

	if (!isPageRelevant (page))
		return;

	_cache->addPage (page);

	if (_renderRequest.getNumber () == page.getNumber ()) {
		if (!_renderRequest.isTextOnly ())
			_renderRequest.setZoom (page.getRenderedZoom ());

		_currentPage = page;

		if (page.getCropPads().isDetected ())
			_renderRequest.setCropPads (page.getCropPads ());

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

	if (page.getCropPads().isDetected () && !_cropPadsCache->isCropPadsExist (page.getNumber ()))
		_cropPadsCache->addCropPads (page.getNumber (), page.getCropPads ());

	if (!isPageRelevant (page))
		return;

	_cache->addPage (page);

	if (_renderRequest.getNumber () == page.getNumber ()) {
		if (!_renderRequest.isTextOnly ())
			_renderRequest.setZoom (page.getRenderedZoom ());

		_currentPage = page;

		if (_zoomThread->property(NSR_CORE_MAIN_RENDER_PROP).toBool ()) {
			_zoomThread->setProperty (NSR_CORE_MAIN_RENDER_PROP, false);
			emit needIndicator (false);
		}

		if (page.getCropPads().isDetected ())
			_renderRequest.setCropPads (page.getCropPads ());

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
	 * done almost immediately in onPreloadThreadFinished() slot */
	if (_preloadThread->isRenderCanceled ())
		return;

	if (page.getCropPads().isDetected () && !_cropPadsCache->isCropPadsExist (page.getNumber ()))
		_cropPadsCache->addCropPads (page.getNumber (), page.getCropPads ());

	if (!isPageRelevant (page))
		return;

	_cache->addPage (page);

	if (_renderRequest.getNumber () == page.getNumber ()) {
		if (!_renderRequest.isTextOnly ())
			_renderRequest.setZoom (page.getRenderedZoom ());

		_currentPage = page;

		if (_preloadThread->property(NSR_CORE_MAIN_RENDER_PROP).toBool ()) {
			_preloadThread->setProperty (NSR_CORE_MAIN_RENDER_PROP, false);
			emit needIndicator (false);
			preloadPages ();
		}

		if (page.getCropPads().isDetected ())
			_renderRequest.setCropPads (page.getCropPads ());

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
		_zoomDoc = copyFileHandler (_doc);
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
		_preloadDoc = copyFileHandler (_doc);
		_preloadDoc->setParent (_preloadThread);
		_preloadThread->setRenderContext (_preloadDoc);
		_preloadThread->setRenderCanceled (false);
	}

	if (_preloadThread->hasAnyRequests ())
		_preloadThread->start (QThread::LowestPriority);
}

void
NSRReaderCore::openFile (const QString &path, const QString& password)
{
	closeFile ();

	_doc = fileHandlerByPath (path, password);

	if (_doc == NULL)
		return;

	_doc->setParent (_thread);

	if (!_doc->isValid ()) {
		emit errorWhileOpening (_doc->getLastError ());
		delete _doc;
		_doc = NULL;
		return;
	}

	_thread->setRenderContext (_doc);

	if (_zoomDoc == NULL) {
		_zoomDoc = copyFileHandler (_doc);
		_zoomDoc->setParent (_zoomThread);
		_zoomThread->setRenderContext (_zoomDoc);
	}

	if (_preloadDoc == NULL) {
		_preloadDoc = copyFileHandler (_doc);
		_preloadDoc->setParent (_preloadThread);
		_preloadThread->setRenderContext (_preloadDoc);
	}

	emit sessionFileOpened (path);
}

void
NSRReaderCore::closeFile ()
{
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
	_cropPadsCache->clearStorage ();

	if (!path.isEmpty ())
		emit sessionFileClosed (path);
}

bool
NSRReaderCore::isFileOpened () const
{
	if (_doc == NULL)
		return false;

	return _doc->isValid ();
}

void
NSRReaderCore::loadPage (PageLoad				dir,
			 NSRRenderRequest::NSRRenderReason	reason,
			 int					page)
{
	if (!isFileOpened () || (isPageRendering () && reason != NSRRenderRequest::NSR_RENDER_REASON_PRELOAD))
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
	else if (pageToLoad > _doc->getPagesCount ())
		pageToLoad = _doc->getPagesCount ();

	if (_pagesLimit > 0 && pageToLoad > _pagesLimit) {
		pageToLoad = _pagesLimit;
		emit pagesLimitPassed ();
	}

	if (reason != NSRRenderRequest::NSR_RENDER_REASON_PRELOAD) {
		_renderRequest.setNumber (pageToLoad);
		_renderRequest.setCropPads (_cropPadsCache->getCropPads (pageToLoad));
	}

	NSRRenderRequest req (_renderRequest);
	req.setRenderReason (reason);

	if (reason == NSRRenderRequest::NSR_RENDER_REASON_PRELOAD) {
		req.setNumber (pageToLoad);
		req.setCropPads (_cropPadsCache->getCropPads (pageToLoad));
	}

	if (reason != NSRRenderRequest::NSR_RENDER_REASON_PRELOAD &&
	    _cache->isPageExists (pageToLoad)) {
		_currentPage = _cache->getPage (pageToLoad);
		emit pageRendered (pageToLoad);
		preloadPages ();

		return;
	}

	NSRRenderRequest::NSRRenderReason reqReason = req.getRenderReason ();

	if (reqReason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM ||
	    reqReason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH) {
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
NSRReaderCore::copyFileHandler (const NSRAbstractDocument* doc)
{
	NSRAbstractDocument *res;

	res = fileHandlerByPath (doc->getDocumentPath (), doc->getPassword ());

	if (res == NULL)
		return NULL;

	if (!res->isValid ()) {
		delete res;
		res = NULL;
	}

	return res;
}

NSRAbstractDocument*
NSRReaderCore::fileHandlerByPath (const QString& path, const QString& passwd) const
{
	NSRAbstractDocument	*res = NULL;
	QString			suffix = QFileInfo(path).suffix().toLower ();

	if (!QFile::exists (path))
		return NULL;

	if (suffix == "pdf")
		res = new NSRPopplerDocument (path, passwd);
	else if (suffix == "djvu" || suffix == "djv")
		res = new NSRDjVuDocument (path);
	else if (suffix == "tiff" || suffix == "tif")
		res = new NSRTIFFDocument (path);
	else
		res = new NSRTextDocument (path);

	return res;
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
	if (!isFileOpened ())
		return;

	int pageToLoadNext = qMin (_doc->getPagesCount (), _renderRequest.getNumber () + 1);
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
	if (!isFileOpened () || _thumbnailer == NULL)
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
