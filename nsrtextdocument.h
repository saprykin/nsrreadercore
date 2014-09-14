#ifndef NSRTEXTDOCUMENT_H
#define NSRTEXTDOCUMENT_H

#include "nsrabstractdocument.h"

#include <QMap>

class NSRTextDocument : public NSRAbstractDocument
{
	Q_OBJECT
public:
	NSRTextDocument (const QString& file, QObject *parent = 0);
	virtual ~NSRTextDocument ();

	int getNumberOfPages () const;
	void renderPage (int page);
	NSR_CORE_IMAGE_DATATYPE getCurrentPage ();
	bool isValid ()	const;
	double getMaxZoom ();
	double getMinZoom ();
	void setTextOnly (bool textOnly);
	QString getText ();
	bool isEncodingUsed () const;
	bool isAutoCrop () const;
	bool isDocumentStyleSupported (NSRAbstractDocument::NSRDocumentStyle style) const;
	inline NSRAbstractDocument::NSRDocumentStyle getPrefferedDocumentStyle () const {
		return NSRAbstractDocument::NSR_DOCUMENT_STYLE_TEXT;
	}

private:
	QString detectCharset ();

	QString	_text;
	QString	_autodetectedEncoding;
	int	_pagesCount;
	bool	_wasEncodingDetected;
};

#endif // NSRTEXTDOCUMENT_H
