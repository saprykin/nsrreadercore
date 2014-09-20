#ifndef __NSRCROPPADS_H__
#define __NSRCROPPADS_H__

/**
 * @file nsrcroppads.h
 * @author Alexander Saprykin
 * @brief Page crop pads
 */

#include "nsrabstractdocument.h"

/**
 * @class NSRCropPads nsrcroppads.h
 * @brief Class for page crop pads
 */
class NSRCropPads
{
public:
	/** Constructor */
	NSRCropPads ();

	/** Destructor */
	~NSRCropPads ();

	/**
	 * @brief Gets page scale
	 * @return Page scale, in %.
	 */
	inline double getScale () const {
		return _scale;
	}

	/**
	 * @brief Gets left pad
	 * @return Left pad, in px.
	 */
	inline int getLeft () const {
		return (int) _left;
	}

	/**
	 * @brief Gets right pad
	 * @return Right pad, in px.
	 */
	inline int getRight () const {
		return (int) _right;
	}

	/**
	 * @brief Gets top pad
	 * @return Top pad, in px.
	 */
	inline int getTop () const {
		return (int) _top;
	}

	/**
	 * @brief Gets bottom pad
	 * @return Bottom pad, in px.
	 */
	inline int getBottom () const {
		return (int) _bottom;
	}

	/**
	 * @brief Sets page scale
	 * @param scale Page scale, in px.
	 */
	void setScale (double scale);

	/**
	 * @brief Sets left pad
	 * @param left Left pad, in px.
	 */
	inline void setLeft (int left) {
		_left = left * _scale;
	}

	/**
	 * @brief Sets right pad
	 * @param right Right pad, in px.
	 */
	inline void setRight (int right) {
		_right = right * _scale;
	}

	/**
	 * @brief Sets top pad
	 * @param top Top pad, in px.
	 */
	inline void setTop (int top) {
		_top = top * _scale;
	}

	/**
	 * @brief Sets bottom pad
	 * @param bottom Bottom pad, in px.
	 */
	inline void setBottom (int bottom) {
		_bottom = bottom * _scale;
	}

	/**
	 * @brief Sets page rotation
	 * @param rotation Page rotation.
	 */
	void setRotation (NSRAbstractDocument::NSRDocumentRotation rotation);

private:
	/**
	 * @brief Rotates pads right for 90 degrees
	 */
	void rotateRight ();

	/**
	 * @brief Rotates pads left for 90 degrees
	 */
	void rotateLeft ();

	/**
	 * @brief Rotates pads for 180 degrees
	 */
	void rotate180 ();

	/**
	 * @brief Mirrors pads vertical
	 */
	void mirrorVertical ();

	/**
	 * @brief Mirrors pads horizontal
	 */
	void mirrorHorizontal ();

	double						_scale;		/**< Page scale	*/
	double						_left;		/**< Left pad	*/
	double						_right;		/**< Right pad	*/
	double						_top;		/**< Top pad	*/
	double						_bottom;	/**< Bottom pad	*/
	NSRAbstractDocument::NSRDocumentRotation	_rotation;	/** Clockwise	*/
};

#endif /* __NSRCROPPADS_H__ */
