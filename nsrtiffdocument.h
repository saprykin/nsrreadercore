#ifndef NSRTIFFDOCUMENT_H
#define NSRTIFFDOCUMENT_H

#include "nsrabstractdocument.h"
#include "nsrcroppads.h"

#include "tiff/tiffio.h"

class NSRTIFFDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	NSRTIFFDocument (const QString& file, QObject *parent = 0);
	virtual ~NSRTIFFDocument ();
	int getNumberOfPages () const;
	void renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid () const;
	double getMaxZoom ();
	double getMinZoom ();
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;
	inline NSRAbstractDocument::NSRDocumentStyle getPrefferedDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_GRAPHIC;
	}
	inline void setEncoding (const QString& encoding) {
		Q_UNUSED (encoding);
	}
	inline QString getEncoding () const {
		return QString ("");
	}

private:
	void updateCropPads ();
	void rotateRightMirrorHorizontal (QImage ** image, char **buf);
	void rotateRightMirrorVertical (QImage ** image, char **buf);

	NSRCropPads		_pads;
	QImage			_origImage;
	QImage			_image;
	QSize			_cachedPageSize;
	TIFF			*_tiff;
	int			_pageCount;
	int			_cachedPage;
};

#endif // NSRTIFFDOCUMENT_H
