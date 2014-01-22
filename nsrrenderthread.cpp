#include "nsrrenderthread.h"
#include "nsrthumbnailer.h"

#include <QMutexLocker>

NSRRenderThread::NSRRenderThread (QObject *parent) :
	QThread (parent),
	_doc (NULL),
	_renderCanceled (0),
	_renderThumbnail (false)
{
}

NSRRenderThread::~NSRRenderThread ()
{
	QMutexLocker locker (&_requestedMutex);
	QMutexLocker rlocker (&_renderedMutex);

	_requestedPages.clear ();
	_renderedPages.clear ();
}

void
NSRRenderThread::setRenderContext (NSRAbstractDocument* doc)
{
	if (isRunning ())
		return;

	_doc = doc;
}

void
NSRRenderThread::addRequest (const NSRRenderRequest& req)
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.append (req);
}

void
NSRRenderThread::cancelRequests ()
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.clear ();
}

bool
NSRRenderThread::hasRequests () const
{
	QMutexLocker locker (&_requestedMutex);
	return !_requestedPages.isEmpty ();
}

NSRRenderRequest
NSRRenderThread::getRequest ()
{
	QMutexLocker locker (&_requestedMutex);

	if (_requestedPages.isEmpty ())
		return NSRRenderRequest ();
	else
		return _requestedPages.takeLast ();
}

NSRRenderedPage
NSRRenderThread::getRenderedPage ()
{
	QMutexLocker locker (&_renderedMutex);

	NSRRenderedPage page;

	if (!_renderedPages.isEmpty ())
		page = _renderedPages.takeFirst ();

	return page;
}

NSRRenderRequest
NSRRenderThread::getCurrentRequest () const
{
	QMutexLocker locker (&_requestedMutex);
	return _currentRequest;
}

void
NSRRenderThread::completeRequest (const NSRRenderedPage& page)
{
	QMutexLocker locker (&_renderedMutex);
	_renderedPages.append (page);
}

void
NSRRenderThread::setCurrentRequest (const NSRRenderRequest& req)
{
	QMutexLocker locker (&_requestedMutex);
	_currentRequest = req;
}

void
NSRRenderThread::prepareRenderContext (const NSRRenderRequest& req)
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	if (req.isZoomToWidth () && req.getRenderReason () != NSRRenderRequest::NSR_RENDER_REASON_CROP_TO_WIDTH)
		_doc->zoomToWidth (req.getScreenWidth ());
	else
		_doc->setZoom (req.getZoom ());

	_doc->setTextOnly (req.isTextOnly ());
	_doc->setInvertedColors (req.isInvertColors ());
	_doc->setAutoCrop (req.isAutoCrop ());
	_doc->setRotation (req.getRotation ());
	_doc->setEncoding (req.getEncoding ());
}

void
NSRRenderThread::updateThumbnail ()
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	if (!_doc->getPassword().isEmpty ())
		NSRThumbnailer::saveThumbnailEncrypted (_doc->getDocumentPath ());
	else if (NSRThumbnailer::isThumbnailOutdated (_doc->getDocumentPath ())) {
		NSRRenderedPage	thumbPage;

		_doc->zoomToWidth (256);
		_doc->setTextOnly (false);
		_doc->setInvertedColors (false);
		_doc->setAutoCrop (false);
		_doc->renderPage (1);

		thumbPage.setRenderedZoom (_doc->getZoom ());
		thumbPage.setImage (_doc->getCurrentPage ());

		_doc->setTextOnly (true);
		_doc->renderPage (1);

		thumbPage.setText (_doc->getText ());

		NSRThumbnailer::saveThumbnail (_doc->getDocumentPath (), thumbPage);
	}
}

void
NSRRenderThread::run ()
{
	if (_doc == NULL) {
		emit renderDone ();
		return;
	}

	bool hasPage = hasRequests ();

	while (hasPage) {
		/* Does we have new pages to render? */
		NSRRenderedPage page (getRequest ());

		if (page.getNumber () < 1 || page.getNumber () > _doc->getNumberOfPages ()) {
			emit renderDone ();
			return;
		}

		if (isRenderCanceled ())
			return;

		setCurrentRequest (page);
		prepareRenderContext (page);

		/* Render image only if we are in graphic mode */
		if (!_doc->isTextOnly ()) {
			_doc->renderPage (page.getNumber ());
			page.setImage (_doc->getCurrentPage ());
			page.setRenderedZoom (_doc->getZoom ());
		}

		if (isRenderCanceled ()) {
			setCurrentRequest (NSRRenderRequest ());
			return;
		}

		_doc->setTextOnly (true);
		_doc->renderPage (page.getNumber ());
		page.setText (_doc->getText ());

		if (_renderThumbnail)
			updateThumbnail ();

		completeRequest (page);
		emit renderDone ();

		hasPage = hasRequests ();

		setCurrentRequest (NSRRenderRequest ());
	}
}
