#include "nsrreadercore.h"
#include "nsrpopplerdocument.h"
#include "nsrdjvudocument.h"
#include "nsrtiffdocument.h"
#include "nsrtextdocument.h"

#include <QFile>
#include <QFileInfo>

#include <float.h>

NSRReaderCore::NSRReaderCore (bool isCardMode, QObject *parent) :
	QObject (parent),
	_doc (NULL),
	_zoomDoc (NULL),
	_thread (NULL),
	_zoomThread (NULL),
	_cache (NULL),
	_isCardMode (isCardMode)
{
	_thread		= new NSRRenderThread (this);
	_zoomThread	= new NSRRenderThread (this);
	_cache		= new NSRPagesCache (this);

	bool ok = connect (_thread, SIGNAL (renderDone ()), this, SLOT (onRenderDone ()));
	Q_UNUSED (ok);
	Q_ASSERT (ok);

	ok = connect (_zoomThread, SIGNAL (renderDone ()), this, SLOT (onZoomRenderDone ()));
	Q_ASSERT (ok);

	ok = connect (_zoomThread, SIGNAL (finished ()), this, SLOT (onZoomThreadFinished ()));
	Q_ASSERT (ok);

	_thread->setThumbnailRender (!_isCardMode);
}

NSRReaderCore::~NSRReaderCore ()
{
	if (_doc != NULL)
		closeDocument ();

	/* Force zoom thread termination */
	if (_zoomDoc != NULL && _zoomThread->isRunning ()) {
		_zoomThread->terminate ();
		delete _zoomDoc;
	}
}

void
NSRReaderCore::openDocument (const QString &path,  const QString& password)
{
	closeDocument ();

	_doc = documentByPath (path);

	if (_doc == NULL)
		return;

	_doc->setPassword (password);
	_renderRequest = NSRRenderRequest ();

	if (!_doc->isValid ()) {
		emit errorWhileOpening (_doc->getLastError ());
		delete _doc;
		_doc = NULL;
		return;
	}

	if (_isCardMode)
		_renderRequest.setInvertColors (false);
	else {
		_renderRequest.setInvertColors (NSRSettings::instance()->isInvertedColors ());
		_renderRequest.setAutoCrop (NSRSettings::instance()->isAutoCrop ());
		_renderRequest.setEncoding (NSRSettings::instance()->getTextEncoding ());
	}

	if (!_isCardMode && NSRSettings::instance()->isStarting ())
		_renderRequest.setTextOnly (NSRSettings::instance()->isWordWrap ());
	else
		_renderRequest.setTextOnly (_doc->getPrefferedDocumentStyle () == NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT);

	_thread->setRenderContext (_doc);

	if (_zoomDoc == NULL) {
		_zoomDoc = copyDocument (_doc);
		_zoomThread->setRenderContext (_zoomDoc);
	}

	if (!_isCardMode)
		NSRSettings::instance()->addLastDocument (path);

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
			_zoomThread->cancelRequests ();
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
	if (_doc == NULL || _isCardMode)
		return;

	bool	needReload = false;
	bool	wasCropped = _renderRequest.isAutoCrop ();
	QString	wasEncoding = _renderRequest.getEncoding ();

	_renderRequest.setAutoCrop (NSRSettings::instance()->isAutoCrop ());
	_renderRequest.setEncoding (NSRSettings::instance()->getTextEncoding ());

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
	}

	if (needReload)
		loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_SETTINGS, _renderRequest.getNumber ());
}

void
NSRReaderCore::loadSession (const NSRSession *session)
{
	if (session == NULL)
		return;

	QString file = session->getFile ();

	if (QFile::exists (file)) {
		openDocument (file, session->getPassword ());

		if (isDocumentOpened ()) {
			_renderRequest.setRotation (session->getRotation ());
			_renderRequest.setZoom (session->getZoomGraphic ());
			_renderRequest.setZoomToWidth (session->isFitToWidth ());

			if (_renderRequest.isZoomToWidth ())
				_renderRequest.setScreenWidth (session->getZoomScreenWidth ());

			loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_NAVIGATION, session->getPage ());
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
	return _thread->isRunning ();
}

void
NSRReaderCore::setScreenWidth (int width)
{
	if (width <= 0)
		return;

	if (_doc == NULL || !_doc->isValid ())
		return;

	_renderRequest.setScreenWidth (width);
}

bool
NSRReaderCore::isFitToWidth () const
{
	if (_doc == NULL || !_doc->isValid ())
		return false;

	return _renderRequest.isZoomToWidth ();
}

double
NSRReaderCore::getZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _renderRequest.getZoom ();
}

double
NSRReaderCore::getMinZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getMinZoom ();
}

double
NSRReaderCore::getMaxZoom () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _doc->getMaxZoom ();
}

void
NSRReaderCore::setZoom (double zoom, NSRRenderRequest::NSRRenderReason reason)
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	if (zoom <= 0)
		return;

	bool toWidth = (reason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
			reason == NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH);

	if (!toWidth && qAbs (_renderRequest.getZoom () - zoom) <= DBL_EPSILON)
		return;

	if (_renderRequest.isTextOnly ())
		return;

	NSRRenderedPage cachedPage = _cache->getPage (_renderRequest.getNumber ());

	if (reason == NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH)
		_cache->removePage (_renderRequest.getNumber ());
	else
		_cache->clearStorage ();

	_renderRequest.setZoomToWidth (toWidth);
	_renderRequest.setZoom (zoom);

	loadPage (PAGE_LOAD_CUSTOM, reason, _renderRequest.getNumber ());
}

void
NSRReaderCore::rotate (double rot)
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	int newRot = (int) normalizeAngle (_renderRequest.getRotation () + rot);

	if (newRot == (int) (_renderRequest.getRotation () + 0.5))
		return;

	_renderRequest.setRotation (newRot);
	_cache->clearStorage ();

	loadPage (PAGE_LOAD_CUSTOM, NSRRenderRequest::NSR_RENDER_REASON_ROTATION, _renderRequest.getNumber ());
}

double
NSRReaderCore::getRotation () const
{
	if (_doc == NULL || !_doc->isValid ())
		return 0;

	return _renderRequest.getRotation ();
}

void
NSRReaderCore::saveCurrentPagePositions (const QPointF& pos,
					 const QPointF& textPos)
{
	if (_doc == NULL || !_doc->isValid ())
		return;

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
	if (!isTextReflowSwitchSupported ())
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
	NSRRenderedPage page = _thread->getRenderedPage ();

	if (!isPageRelevant (page))
		return;

	_currentPage = page;

	/* In case of fit to width zoom the overall page zoom can be changed */
	if (_renderRequest.isZoomToWidth ())
		_renderRequest.setZoom (_currentPage.getZoom ());

	_cache->addPage (_currentPage);

	emit needIndicator (false);
	emit pageRendered (_renderRequest.getNumber ());
}

void
NSRReaderCore::onZoomRenderDone ()
{
	NSRRenderedPage page = _zoomThread->getRenderedPage ();

	if (!page.isImageValid ())
		return;

	/* We do not need to reset document changed flag because it would be
	 * done almost immediately in onZoomThreadFinished() slot */
	if (_zoomThread->isRenderCanceled ())
		return;

	if (!isPageRelevant (page))
		return;

	if (_renderRequest.isZoomToWidth ())
		_renderRequest.setZoom (page.getZoom ());

	_cache->addPage (page);

	if (_renderRequest.getNumber () == page.getNumber ()) {
		_currentPage = page;
		emit pageRendered (_renderRequest.getNumber ());
	}
}

void
NSRReaderCore::onZoomThreadFinished ()
{
	if (_zoomThread->isRenderCanceled ()) {
		/* All requests must be canceled on document opening */
		delete _zoomDoc;
		_zoomDoc = copyDocument (_doc);
		_zoomThread->setRenderContext (_zoomDoc);
		_zoomThread->setRenderCanceled (false);
	}

	if (_zoomThread->hasRequests ())
		_zoomThread->start ();
}

void
NSRReaderCore::loadPage (PageLoad				dir,
			 NSRRenderRequest::NSRRenderReason	reason,
			 int					page)
{
	if (_doc == NULL || _thread->isRunning ())
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

#ifdef NSR_CORE_LITE_VERSION
	if (pageToLoad > NSRSettings::getMaxAllowedPages ()) {
		pageToLoad = NSRSettings::getMaxAllowedPages ();
		emit liteVersionOverPage ();
	}
#endif

	_renderRequest.setNumber (pageToLoad);
	NSRRenderRequest req (_renderRequest);
	req.setRenderReason (reason);

	if (_cache->isPageExists (pageToLoad)) {
		QString suffix = QFileInfo(_doc->getDocumentPath ()).suffix().toLower ();

		_currentPage = _cache->getPage (pageToLoad);

		emit pageRendered (pageToLoad);

		return;
	}

	NSRRenderRequest::NSRRenderReason reason = req.getRenderReason ();

	if (reason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM ||
	    reason == NSRRenderRequest::NSR_RENDER_REASON_ZOOM_TO_WIDTH ||
	    reason == NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH) {
		_zoomThread->cancelRequests ();
		_zoomThread->addRequest (req);

		if (!_zoomThread->isRunning ())
			_zoomThread->start ();
	} else {
		emit needIndicator (true);

		_zoomThread->cancelRequests ();
		_thread->cancelRequests ();
		_thread->addRequest (req);

		if (!_thread->isRunning ())
			_thread->start ();
	}
}

NSRAbstractDocument*
NSRReaderCore::copyDocument (const NSRAbstractDocument* doc)
{
	NSRAbstractDocument *res;

	if (doc == NULL || !doc->isValid ())
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
	if (!isDocumentOpened ())
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

double
NSRReaderCore::normalizeAngle (double angle) const
{
	if (qAbs (angle) / 360.0 > 1.0)
		angle -= ((long) (angle / 360.0) * 360);

	if (angle < 0)
		angle += 360.0;
	else if (qAbs (angle - 360.0) <= DBL_EPSILON)
		angle = 0.0;

	return angle;
}

bool
NSRReaderCore::isPageRelevant (const NSRRenderedPage& page) const
{
	bool relevant = _renderRequest.isAutoCrop () == page.isAutoCrop () &&
			_renderRequest.isTextOnly () == page.isTextOnly () &&
			_renderRequest.isInvertColors () == page.isInvertColors () &&
			_renderRequest.isZoomToWidth () == page.isZoomToWidth ();

	if (!_renderRequest.isZoomToWidth ())
		relevant = relevant && qAbs (_renderRequest.getZoom () - page.getZoom ()) <= DBL_EPSILON;

	return relevant;
}
