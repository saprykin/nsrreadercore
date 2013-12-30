#include "nsrrenderzoomthread.h"

NSRRenderZoomThread::NSRRenderZoomThread (QObject *parent) :
	NSRAbstractRenderThread (parent)
{
}

NSRRenderZoomThread::~NSRRenderZoomThread ()
{
}

void
NSRRenderZoomThread::run ()
{
	NSRAbstractDocument	*doc = getRenderContext ();
	bool			hasPage;

	if (doc == NULL)
		return;

	do {
		/* Does we have new pages to render? */
		if (isDocumentChanged ()) {
			cancelRequests ();
			return;
		}

		NSRRenderedPage page (getRequest ());

		/* Last page is only one that is actual, so clear
		 * all other */
		cancelRequests ();

		if (page.getNumber () < 1 ||
		    page.getNumber () > doc->getNumberOfPages ())
			return;

		setCurrentRequest (page);
		prepareRenderContext (page);

		/* Render image only if we are in graphic mode */
		if (!doc->isTextOnly ()) {
			doc->renderPage (page.getNumber ());
			page.setImage (doc->getCurrentPage ());
			page.setZoom (doc->getZoom ());
		}

		if (isDocumentChanged ()) {
			setCurrentRequest (NSRRenderRequest ());
			return;
		}

		hasPage = hasRequests ();

		/* We need this page only if there is no more
		 * pages requested. If there are more pages
		 * requested then current page is outdated */
		if (!hasRequests ()) {
			completeRequest (page);
			emit renderDone ();
		}

		setCurrentRequest (NSRRenderRequest ());
	} while (hasPage);
}

void
NSRRenderZoomThread::setDocumentChanged (bool changed)
{
	_documentChanged = changed ? 1 : 0;
}

bool
NSRRenderZoomThread::isDocumentChanged ()
{
	return (_documentChanged == 1);
}
