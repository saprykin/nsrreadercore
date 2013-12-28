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
NSRAbstractRenderThread::addRequest (const NSRRenderRequest& page)
{
	QMutexLocker locker (&_requestedMutex);
	_requestedPages.append (page);
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

NSRRenderRequest
NSRAbstractRenderThread::getRenderedPage ()
{
	QMutexLocker locker (&_renderedMutex);

	NSRRenderRequest page;

	if (!_renderedPages.isEmpty ()) {
		page = _renderedPages.takeFirst ();
		page.setCropped (_doc->isAutoCrop ());
	}

	return page;
}

NSRRenderRequest
NSRAbstractRenderThread::getCurrentRequest () const
{
	QMutexLocker locker (&_requestedMutex);
	return _currentRequest;
}

void
NSRAbstractRenderThread::completeRequest (const NSRRenderRequest& page)
{
	QMutexLocker locker (&_renderedMutex);
	_renderedPages.append (page);
}

void
NSRAbstractRenderThread::setCurrentRequest (const NSRRenderRequest& page)
{
	QMutexLocker locker (&_requestedMutex);
	_currentRequest = page;
}
