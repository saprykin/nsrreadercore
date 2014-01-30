#ifndef NSRRENDERTHREAD_H
#define NSRRENDERTHREAD_H

#include "nsrrenderedpage.h"
#include "nsrabstractdocument.h"

#include <QThread>
#include <QList>
#include <QHash>
#include <QMutex>
#include <QAtomicInt>

class NSRRenderThread : public QThread
{
	Q_OBJECT
public:
	explicit NSRRenderThread (QObject *parent = 0);
	virtual ~NSRRenderThread ();

	void setRenderContext (NSRAbstractDocument *doc);

	inline NSRAbstractDocument * getRenderContext () const {
		return _doc;
	}

	void addRequest (const NSRRenderRequest &req);
	void cancelRequests (NSRRenderRequest::NSRRenderType type);
	void cancelAllRequests ();
	bool hasRequests (NSRRenderRequest::NSRRenderType type) const;
	bool hasAnyRequests () const;
	NSRRenderedPage getRenderedPage ();
	NSRRenderRequest getCurrentRequest () const;

	inline void setRenderCanceled (bool canceled) {
		_renderCanceled = canceled ? 1 : 0;
	}

	inline bool isRenderCanceled () const {
		return (_renderCanceled == 1);
	}

	virtual void run ();

Q_SIGNALS:
	void renderDone ();

private:
	NSRRenderRequest getRequest ();
	void completeRequest (const NSRRenderedPage& page);
	void setCurrentRequest (const NSRRenderRequest& req);
	void prepareRenderContext (const NSRRenderRequest& req);

	NSRAbstractDocument		*_doc;
	NSRRenderRequest		_currentRequest;
	QList<NSRRenderRequest>		_requestedPages;
	QList<NSRRenderedPage>		_renderedPages;
	mutable QMutex			_requestedMutex;
	mutable QMutex			_renderedMutex;
	QAtomicInt			_renderCanceled;
};

#endif // NSRRENDERTHREAD_H
