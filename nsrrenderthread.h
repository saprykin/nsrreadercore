#ifndef __NSRRENDERTHREAD_H__
#define __NSRRENDERTHREAD_H__

/**
 * @file nsrrenderthread.h
 * @author Alexander Saprykin
 * @brief Render thread
 */

#include "nsrrenderedpage.h"
#include "nsrabstractdocument.h"

#include <QThread>
#include <QList>
#include <QHash>
#include <QMutex>
#include <QAtomicInt>

/**
 * @class NSRRenderThread nsrrenderthread.h
 * @brief Class for render thread
 */
class NSRRenderThread : public QThread
{
	Q_OBJECT
public:
	/**
	 * @brief Constructor with parameter
	 * @param parent Parent object.
	 */
	explicit NSRRenderThread (QObject *parent = 0);

	/**
	 * @brief Destructor
	 */
	virtual ~NSRRenderThread ();

	/**
	 * @brief Sets render context
	 * @param doc File handler for rendering.
	 */
	void setRenderContext (NSRAbstractDocument *doc);

	/**
	 * @brief Gets render context
	 * @return File handler for rendering.
	 */
	inline NSRAbstractDocument * getRenderContext () const {
		return _doc;
	}

	/**
	 * @brief Adds render request to queue
	 * @param req Render request.
	 */
	void addRequest (const NSRRenderRequest &req);

	/**
	 * @brief Cancels all render requests by type
	 * @param type Render request type.
	 */
	void cancelRequests (NSRRenderRequest::NSRRenderType type);

	/**
	 * @brief Cancel all render request
	 */
	void cancelAllRequests ();

	/**
	 * @brief Checks whether thread has render requests of given type
	 * @param type Render request type.
	 * @return True if thread has render requests of given type, false
	 * otherwise.
	 */
	bool hasRequests (NSRRenderRequest::NSRRenderType type) const;

	/**
	 * @brief Checks whether thread has any render requests
	 * @return True if thread has any render requests, false otherwise.
	 */
	bool hasAnyRequests () const;

	/**
	 * @brief Gets last rendered page
	 * @return Last rendered page.
	 */
	NSRRenderedPage getRenderedPage ();

	/**
	 * @brief Gets current render request
	 * @return Current render request.
	 */
	NSRRenderRequest getCurrentRequest () const;

	/**
	 * @brief Marks current render request as canceled
	 * @param canceled Whether current render request is canceled.
	 */
	inline void setRenderCanceled (bool canceled) {
		_renderCanceled = canceled ? 1 : 0;
	}

	/**
	 * @brief Checks whether current render request is canceled
	 * @return True if current render request is canceled, false otherwise.
	 */
	inline bool isRenderCanceled () const {
		return (_renderCanceled == 1);
	}

	/** Reimplemented from QThread */
	virtual void run ();

Q_SIGNALS:
	/**
	 * @brief Render finished
	 */
	void renderDone ();

private:
	/**
	 * @brief Gets render request from the queue
	 * @return Render request.
	 */
	NSRRenderRequest getRequest ();

	/**
	 * @brief Marks render request as completed
	 * @param page Rendered page.
	 */
	void completeRequest (const NSRRenderedPage& page);

	/**
	 * @brief Sets current render request
	 * @param req New render request.
	 */
	void setCurrentRequest (const NSRRenderRequest& req);

	/**
	 * @brief Prepares render context
	 * @param req Render request.
	 */
	void prepareRenderContext (const NSRRenderRequest& req);

	NSRAbstractDocument		*_doc;			/**< File handler for render	*/
	NSRRenderRequest		_currentRequest;	/**< Current render request	*/
	QList<NSRRenderRequest>		_requestedPages;	/**< List of waiting requests	*/
	QList<NSRRenderedPage>		_renderedPages;		/**< List of rendered pages	*/
	mutable QMutex			_requestedMutex;	/**< Mutex for render requests	*/
	mutable QMutex			_renderedMutex;		/**< Mutex for rendered pages	*/
	QAtomicInt			_renderCanceled;	/**< Render cancel flag		*/
};

#endif /* __NSRRENDERTHREAD_H__ */
