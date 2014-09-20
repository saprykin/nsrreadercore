#ifndef NSRSESSION_H
#define NSRSESSION_H

#include "nsrabstractdocument.h"
#include "nsrreadercore_global.h"

#include <QString>
#include <QPointF>

class NSRREADERCORE_SHARED NSRSession
{
public:
	NSRSession ();
	NSRSession (const QString& file,
		    int page,
		    int zoomText,
		    double zoomGraphic,
		    bool isFitToWidth,
		    const QPointF& pos,
		    const QPointF& textPos,
		    NSRAbstractDocument::NSRDocumentRotation rotation);

	inline QString getFile () const {return _file;}
	inline int getPage () const {return _page;}
	inline int getZoomText () const {return _zoomText;}
	inline double getZoomGraphic () const {return _zoomGraphic;}
	inline bool isFitToWidth () const {return _isFitToWidth;}
	inline QPointF getPosition () const {return _pos;}
	inline QPointF getTextPosition () const {return _textPos;}
	inline NSRAbstractDocument::NSRDocumentRotation getRotation () const {return _rotation;}
	inline QString getPassword () const {return _passwd;}

	inline void setFile (const QString& file) {_file = file;}
	inline void setPage (int page) {_page = page;}
	inline void setZoomText (int zoom) {_zoomText = zoom;}
	inline void setZoomGraphic (double zoom) {_zoomGraphic = zoom;}
	inline void setFitToWidth (bool fit) {_isFitToWidth = fit;}
	inline void setPosition (const QPointF& pos) {_pos = pos;}
	inline void setTextPosition (const QPointF& pos) {_textPos = pos;}
	inline void setRotation (NSRAbstractDocument::NSRDocumentRotation rotation) {_rotation = rotation;}
	inline void setPassword (const QString& password) {_passwd = password;}

private:
	QString 					_file;
	QString						_passwd;
	QPointF						_pos;
	QPointF						_textPos;
	double						_zoomGraphic;
	NSRAbstractDocument::NSRDocumentRotation	_rotation;
	int						_page;
	int						_zoomText;
	bool						_isFitToWidth;
};

#endif /* NSRSESSION_H */
