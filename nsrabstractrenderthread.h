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
	void addRequest (const NSRRenderRequest &req);
	void cancelRequests ();
	bool hasRequests ();
	NSRRenderedPage getRenderedPage ();
	NSRRenderRequest getCurrentRequest () const;

	virtual void run () = 0;

protected:
	NSRRenderRequest getRequest ();
	void completeRequest (const NSRRenderedPage& page);
	void setCurrentRequest (const NSRRenderRequest& req);
	void prepareRenderContext (const NSRRenderRequest& req);

private:
	NSRAbstractDocument		*_doc;
	NSRRenderRequest		_currentRequest;
	QList<NSRRenderRequest>		_requestedPages;
	QList<NSRRenderedPage>		_renderedPages;
	mutable QMutex			_requestedMutex;
	mutable QMutex			_renderedMutex;
};

#endif /* NSRABSTRACTRENDERTHREAD_H_ */
