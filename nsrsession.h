#ifndef __NSRSESSION_H__
#define __NSRSESSION_H__

/**
 * @file nsrsession.h
 * @author Alexander Saprykin
 * @brief File session
 */

#include "nsrabstractdocument.h"
#include "nsrreadercore_global.h"

#include <QString>
#include <QPointF>

/**
 * @class NSRSession nsrsession.h
 * @brief Class for file session
 */
class NSRREADERCORE_SHARED NSRSession
{
public:
	/**
	 * @brief Constructor
	 */
	NSRSession ();

	/**
	 * @brief Constructor with parameters
	 * @param file Path to file.
	 * @param page Page number (from 1).
	 * @param zoomText Zoom value for text, in %.
	 * @param zoomGraphic Zoom value for image, in %.
	 * @param isFitToWidth Fit to width flag.
	 * @param pos Last position for image.
	 * @param textPos Last position for text.
	 * @param rotation Rotation angle.
	 */
	NSRSession (const QString& file,
		    int page,
		    int zoomText,
		    double zoomGraphic,
		    bool isFitToWidth,
		    const QPointF& pos,
		    const QPointF& textPos,
		    NSRAbstractDocument::NSRDocumentRotation rotation);

	/**
	 * @brief Gets session file
	 * @return Session file.
	 */
	inline QString getFile () const {
		return _file;
	}

	/**
	 * @brief Gets page number
	 * @return Page number (from 1).
	 */
	inline int getPage () const {
		return _page;
	}

	/**
	 * @brief Gets zoom value for text
	 * @return Zoom value for text, in %.
	 */
	inline int getZoomText () const {
		return _zoomText;
	}

	/**
	 * @brief Gets zoom value for image
	 * @return Zoom value for image, in %.
	 */
	inline double getZoomGraphic () const {
		return _zoomGraphic;
	}

	/**
	 * @brief Checks whether file is set to fit the width
	 * @return True if file is set to fit the width.
	 */
	inline bool isFitToWidth () const {
		return _isFitToWidth;
	}

	/**
	 * @brief Gets last position for image
	 * @return Last position for image.
	 */
	inline QPointF getPosition () const {
		return _pos;
	}

	/**
	 * @brief Gets last position for text
	 * @return Last position for text.
	 */
	inline QPointF getTextPosition () const {
		return _textPos;
	}

	/**
	 * @brief Gets rotation angle
	 * @return Rotation angle.
	 */
	inline NSRAbstractDocument::NSRDocumentRotation getRotation () const {
		return _rotation;
	}

	/**
	 * @brief Gets file password (if any)
	 * @return File password.
	 */
	inline QString getPassword () const {
		return _passwd;
	}

	/**
	 * @brief Sets path to file
	 * @param file Path to file.
	 */
	inline void setFile (const QString& file) {
		_file = file;
	}

	/**
	 * @brief Sets page number
	 * @param page Page number (from 1).
	 */
	inline void setPage (int page) {
		_page = page;
	}

	/**
	 * @brief Sets zoom value for text
	 * @param zoom Zoom value for text, in %.
	 */
	inline void setZoomText (int zoom) {
		_zoomText = zoom;
	}

	/**
	 * @brief Sets zoom value for image
	 * @param zoom Zoom value for image, in %.
	 */
	inline void setZoomGraphic (double zoom) {
		_zoomGraphic = zoom;
	}

	/**
	 * @brief Sets file fit to width
	 * @param fit Fit to width flag.
	 */
	inline void setFitToWidth (bool fit) {
		_isFitToWidth = fit;
	}

	/**
	 * @brief Sets image last position
	 * @param pos Image last position.
	 */
	inline void setPosition (const QPointF& pos) {
		_pos = pos;
	}

	/**
	 * @brief Sets text last position
	 * @param pos Text last position.
	 */
	inline void setTextPosition (const QPointF& pos) {
		_textPos = pos;
	}

	/**
	 * @brief Sets rotation angle
	 * @param rotation Rotation angle.
	 */
	inline void setRotation (NSRAbstractDocument::NSRDocumentRotation rotation) {
		_rotation = rotation;
	}

	/**
	 * @brief Sets file password (if any)
	 * @param password File password.
	 */
	inline void setPassword (const QString& password) {
		_passwd = password;
	}

private:
	QString 					_file;		/**< Path to file		*/
	QString						_passwd;	/**< File password		*/
	QPointF						_pos;		/**< Last image position	*/
	QPointF						_textPos;	/**< Last text position		*/
	double						_zoomGraphic;	/**< Image zoom value		*/
	NSRAbstractDocument::NSRDocumentRotation	_rotation;	/**< Rotation angle		*/
	int						_page;		/**< Page number		*/
	int						_zoomText;	/**< Text zoom value		*/
	bool						_isFitToWidth;	/**< Fit to width flag		*/
};

#endif /* __NSRSESSION_H__ */
