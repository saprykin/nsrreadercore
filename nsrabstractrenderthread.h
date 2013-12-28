#ifndef NSRABSTRACTRENDERTHREAD_H_
#define NSRABSTRACTRENDERTHREAD_H_

#include "nsrrenderrequest.h"
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
	void addRequest (const NSRRenderRequest &page);
	void cancelRequests ();
	bool hasRequests ();
	NSRRenderRequest getRenderedPage ();
	NSRRenderRequest getCurrentRequest () const;

	virtual void run () = 0;

protected:
	NSRRenderRequest getRequest ();
	void completeRequest (const NSRRenderRequest& page);
	void setCurrentRequest (const NSRRenderRequest& page);

private:
	NSRAbstractDocument		*_doc;
	NSRRenderRequest		_currentRequest;
	QList<NSRRenderRequest>		_requestedPages;
	QList<NSRRenderRequest>		_renderedPages;
	mutable QMutex			_requestedMutex;
	mutable QMutex			_renderedMutex;
};

#endif /* NSRABSTRACTRENDERTHREAD_H_ */
