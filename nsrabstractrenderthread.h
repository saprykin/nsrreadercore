#ifndef NSRABSTRACTRENDERTHREAD_H_
#define NSRABSTRACTRENDERTHREAD_H_

#include "nsrrenderedpage.h"
#include "nsrabstractdocument.h"

#include <QThread>
#include <QMutex>
#include <QList>

class NSRAbstractRenderThread : public QThread
{
public:
	NSRAbstractRenderThread (QObject *parent = 0);
	virtual ~NSRAbstractRenderThread ();

	void setRenderContext (NSRAbstractDocument *doc);
	NSRAbstractDocument * getRenderContext ();
	void addRequest (const NSRRenderedPage &page);
	void cancelRequests ();
	bool hasRequests ();
	NSRRenderedPage getRenderedPage ();
	NSRRenderedPage getCurrentRequest () const;

	virtual void run () = 0;

protected:
	NSRRenderedPage getRequest ();
	void completeRequest (const NSRRenderedPage& page);
	void setCurrentRequest (const NSRRenderedPage& page);

private:
	NSRAbstractDocument		*_doc;
	NSRRenderedPage			_currentRequest;
	QList<NSRRenderedPage>		_requestedPages;
	QList<NSRRenderedPage>		_renderedPages;
	mutable QMutex			_requestedMutex;
	mutable QMutex			_renderedMutex;
};

#endif /* NSRABSTRACTRENDERTHREAD_H_ */
