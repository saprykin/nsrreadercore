#include "nsrcroppads.h"

#include <qmath.h>
#include <float.h>

NSRCropPads::NSRCropPads () :
	_scale (1.0),
	_left (0.0),
	_right (0.0),
	_top (0.0),
	_bottom (0.0),
	_rotation (NSRAbstractDocument::NSR_DOCUMENT_ROTATION_0)
{
}

NSRCropPads::~NSRCropPads ()
{
}

void
NSRCropPads::setScale (double scale)
{
	if (scale < 0)
		return;

	if (qAbs (scale) <= DBL_EPSILON) {
		_left	= 0.0;
		_right	= 0.0;
		_top	= 0.0;
		_bottom	= 0.0;
	} else {
		double ratio = qAbs (_scale) <= DBL_EPSILON ? scale : scale / _scale;

		_left	*= ratio;
		_right	*= ratio;
		_top	*= ratio;
		_bottom	*= ratio;
	}

	_scale = scale;
}

void
NSRCropPads::rotateLeft ()
{
	double tmp = _top;

	_top	= _right;
	_right	= _bottom;
	_bottom	= _left;
	_left	= tmp;
}

void
NSRCropPads::rotateRight ()
{
	double tmp = _top;

	_top	= _left;
	_left	= _bottom;
	_bottom	= _right;
	_right	= tmp;
}

void
NSRCropPads::rotate180 ()
{
	mirrorHorizontal ();
	mirrorVertical ();
}

void
NSRCropPads::setRotation (NSRAbstractDocument::NSRDocumentRotation rotation)
{
	int delta = (int) (rotation - _rotation);

	if (delta < 0)
		delta += 360;

	if (delta == 0)
		return;

	switch (delta) {
	case 90:
		rotateRight ();
		break;
	case 180:
		rotateRight ();
		rotateRight ();
		break;
	case 270:
		rotateLeft ();
		break;
	default:
		break;
	}

	_rotation = rotation;
}

void
NSRCropPads::mirrorVertical ()
{
	double tmp = _top;
	_top = _bottom;
	_bottom = tmp;
}

void
NSRCropPads::mirrorHorizontal ()
{
	double tmp = _left;
	_left = _right;
	_right = tmp;
}
