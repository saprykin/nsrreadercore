#ifndef __NSRCROPPADS_H__
#define __NSRCROPPADS_H__

/**
 * @file nsrcroppads.h
 * @author Alexander Saprykin
 * @brief Page crop pads
 */

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
	 * @brief Checks whether pads were detected
	 * @return True if pads were detected, false otherwise.
	 * @since 1.4.2
	 */
	inline bool isDetected () const {
		return _isDetected;
	}

	/**
	 * @brief Gets left pad
	 * @return Left pad, relative to width.
	 */
	inline double getLeft () const {
		return _left;
	}

	/**
	 * @brief Gets right pad
	 * @return Right pad, relative to width.
	 */
	inline double getRight () const {
		return _right;
	}

	/**
	 * @brief Gets top pad
	 * @return Top pad, relative to height.
	 */
	inline double getTop () const {
		return _top;
	}

	/**
	 * @brief Gets bottom pad
	 * @return Bottom pad, relative to height.
	 */
	inline double getBottom () const {
		return _bottom;
	}

	/**
	 * @brief Sets detected flag
	 * @param detected Whether pads were detected.
	 * @since 1.4.2
	 */
	inline void setDetected (bool detected) {
		_isDetected = detected;
	}

	/**
	 * @brief Sets left pad
	 * @param left Left pad, relative to width.
	 */
	inline void setLeft (double left) {
		_left = left;
	}

	/**
	 * @brief Sets right pad
	 * @param right Right pad, relative to width.
	 */
	inline void setRight (double right) {
		_right = right;
	}

	/**
	 * @brief Sets top pad
	 * @param top Top pad, relative to height.
	 */
	inline void setTop (double top) {
		_top = top;
	}

	/**
	 * @brief Sets bottom pad
	 * @param bottom Bottom pad, relative to height.
	 */
	inline void setBottom (double bottom) {
		_bottom = bottom;
	}

	/**
	 * @brief Sets page rotation
	 * @param rotation Page rotation.
	 */
	void setRotation (unsigned int rotation);

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

	double		_left;		/**< Left pad		*/
	double		_right;		/**< Right pad		*/
	double		_top;		/**< Top pad		*/
	double		_bottom;	/**< Bottom pad		*/
	unsigned int	_rotation;	/**< Clockwise		*/
	bool		_isDetected;	/**< Detected flag	*/
};

#endif /* __NSRCROPPADS_H__ */
