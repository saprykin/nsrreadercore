#include "nsrabstractrenderthread.h"

#include <QMutexLocker>

NSRAbstractRenderThread::NSRAbstractRenderThread (QObject *parent) :
	QThread (parent),
	_doc (NULL)
{
}

NSRAbstractRenderThread::~NSRAbstractRenderThread ()
{
	QMutexLocker locker (&_requestedMutex);
	QMutexLocker rlocker (&_renderedMutex);

	_requestedPages.clear ();
	_renderedPages.clear ();
}

void
NSRAbstractRenderThread::setRenderContext (NSRAbstractDocument* doc)
{
	if (isRunning ())
		return;

	_doc = doc;
}

NSRAbstractDocument *
NSRAbstractRenderThread::getRenderContext ()
{
	return _doc;
}

void
NSRAbstractRenderThread::addRequest (const NSRRenderRequest& req)
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.append (req);
}

void
NSRAbstractRenderThread::cancelRequests ()
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.clear ();
}

bool
NSRAbstractRenderThread::hasRequests ()
{
	QMutexLocker locker (&_requestedMutex);
	return !_requestedPages.isEmpty ();
}

NSRRenderRequest
NSRAbstractRenderThread::getRequest ()
{
	QMutexLocker locker (&_requestedMutex);

	if (_requestedPages.isEmpty ())
		return NSRRenderRequest ();
	else
		return _requestedPages.takeLast ();
}

NSRRenderedPage
NSRAbstractRenderThread::getRenderedPage ()
{
	QMutexLocker locker (&_renderedMutex);

	NSRRenderedPage page;

	if (!_renderedPages.isEmpty ())
		page = _renderedPages.takeFirst ();

	return page;
}

NSRRenderRequest
NSRAbstractRenderThread::getCurrentRequest () const
{
	QMutexLocker locker (&_requestedMutex);
	return _currentRequest;
}

void
NSRAbstractRenderThread::completeRequest (const NSRRenderedPage& page)
{
	QMutexLocker locker (&_renderedMutex);
	_renderedPages.append (page);
}

void
NSRAbstractRenderThread::setCurrentRequest (const NSRRenderRequest& req)
{
	QMutexLocker locker (&_requestedMutex);
	_currentRequest = req;
}

void
NSRAbstractRenderThread::prepareRenderContext (const NSRRenderRequest& req)
{
	if (_doc == NULL)
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
