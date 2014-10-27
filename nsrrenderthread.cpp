#include "nsrrenderthread.h"
#include "nsrpagecropper.h"

#include <QMutexLocker>

#define NSR_RENDER_THREAD_CROP_WIDTH	256

NSRRenderThread::NSRRenderThread (QObject *parent) :
	QThread (parent),
	_doc (NULL),
	_renderCanceled (0)
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
NSRRenderThread::cancelRequests (NSRRenderRequest::NSRRenderType type)
{
	QMutexLocker locker (&_requestedMutex);

	for (int i = _requestedPages.count () - 1; i >= 0; --i)
		if (_requestedPages.at(i).getRenderType () == type)
			_requestedPages.takeAt (i);
}

void
NSRRenderThread::cancelAllRequests ()
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.clear ();
}

bool
NSRRenderThread::hasRequests (NSRRenderRequest::NSRRenderType type) const
{
	QMutexLocker locker (&_requestedMutex);

	int count = _requestedPages.count ();

	for (int i = 0; i < count; ++i)
		if (_requestedPages.at(i).getRenderType () == type)
			return true;

	return false;
}

bool
NSRRenderThread::hasAnyRequests () const
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

	if (req.isZoomToWidth ()) {
		_doc->setZoomToWidth (true);
	} else
		_doc->setZoom (req.getZoom ());

	_doc->setPageWidth (req.getScreenWidth ());
	_doc->setTextOnly (req.isTextOnly ());
	_doc->setInvertedColors (req.isInvertColors ());
	_doc->setAutoCrop (req.isAutoCrop ());
	_doc->setCropPads (req.getCropPads ());
	_doc->setRotation (req.getRotation ());
	_doc->setEncoding (req.getEncoding ());
}

void
NSRRenderThread::prepareRenderContextForCrop (const NSRRenderRequest& req)
{
	if (_doc == NULL || !_doc->isValid ())
		return;

	_doc->setZoomToWidth (true);
	_doc->setPageWidth (NSR_RENDER_THREAD_CROP_WIDTH);
	_doc->setTextOnly (false);
	_doc->setInvertedColors (req.isInvertColors ());
	_doc->setAutoCrop (false);
	_doc->setRotation (NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0);
	_doc->setEncoding (req.getEncoding ());
}

void
NSRRenderThread::run ()
{
	if (_doc == NULL) {
		emit renderDone ();
		return;
	}

	bool hasPage = hasAnyRequests ();

	while (hasPage) {
		/* Do we have new pages to render? */
		NSRRenderedPage page (getRequest ());

		if (page.getNumber () < 1 || page.getNumber () > _doc->getPagesCount ()) {
			emit renderDone ();
			return;
		}

		if (isRenderCanceled ())
			return;

		setCurrentRequest (page);

		if (!page.isTextOnly() && page.isAutoCrop ()) {
			if (!page.getCropPads().isDetected ()) {
				prepareRenderContextForCrop (page);

				_doc->renderPage (page.getNumber ());

				bb::ImageData imgData = _doc->getCurrentPage ();
				NSRCropPads pads = NSRPageCropper::findCropPads (imgData.pixels (),
										 NSRPageCropper::NSR_PIXEL_ORDER_RGBA,
										 imgData.width (),
										 imgData.height (),
										 imgData.bytesPerLine (),
										 0);
				page.setCropPads (pads);
			}
		}

		prepareRenderContext (page);

		/* Render image only if we are in graphic mode */
		if (!_doc->isTextOnly ()) {
			NSRRenderInfo rinfo = _doc->renderPage (page.getNumber ());

			page.setImage (_doc->getCurrentPage ());
			page.setRenderedZoom (_doc->getZoom ());
			page.setMinRenderZoom (rinfo.getMinZoom ());
			page.setMaxRenderZoom (rinfo.getMaxZoom ());
		}

		if (isRenderCanceled ()) {
			setCurrentRequest (NSRRenderRequest ());
			return;
		}

		_doc->setTextOnly (true);
		_doc->renderPage (page.getNumber ());
		page.setText (_doc->getText ());

		completeRequest (page);
		emit renderDone ();

		hasPage = hasAnyRequests ();

		setCurrentRequest (NSRRenderRequest ());
	}
}
