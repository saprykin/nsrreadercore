#ifndef NSRCROPPADS_H_
#define NSRCROPPADS_H_

#include "nsrabstractdocument.h"

class NSRCropPads
{
public:
	NSRCropPads ();
	~NSRCropPads ();

	inline double getScale () const {
		return _scale;
	}

	inline int getLeft () const {
		return (int) _left;
	}

	inline int getRight () const {
		return (int) _right;
	}

	inline int getTop () const {
		return (int) _top;
	}

	inline int getBottom () const {
		return (int) _bottom;
	}

	void setScale (double scale);

	inline void setLeft (int left) {
		_left = left * _scale;
	}

	inline void setRight (int right) {
		_right = right * _scale;
	}

	inline void setTop (int top) {
		_top = top * _scale;
	}

	inline void setBottom (int bottom) {
		_bottom = bottom * _scale;
	}

	void setRotation (NSRAbstractDocument::NSRDocumentRotation rotation);

private:
	void rotateRight ();
	void rotateLeft ();
	void rotate180 ();
	void mirrorVertical ();
	void mirrorHorizontal ();

	double						_scale;
	double						_left;
	double						_right;
	double						_top;
	double						_bottom;
	NSRAbstractDocument::NSRDocumentRotation	_rotation;	/** Clockwise */
};

#endif /* NSRCROPPADS_H_ */
