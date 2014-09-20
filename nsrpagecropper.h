#ifndef __NSRPAGECROPPER_H__
#define __NSRPAGECROPPER_H__

/**
 * @file nsrpagecropper.h
 * @author Alexander Saprykin
 * @brief Page cropper
 */

#include "nsrcroppads.h"

/**
 * @class NSRPageCropper nsrpagecropper.h
 * @brief Page cropper class
 */
class NSRPageCropper
{
public:
	/** Pixel order of image data */
	enum NSRPixelOrder {
		NSR_PIXEL_ORDER_RGB	= 0,	/**< 3-bytes RGB	*/
		NSR_PIXEL_ORDER_ARGB	= 1,	/**< 4-bytes ARGB	*/
		NSR_PIXEL_ORDER_BGR	= 2,	/**< 3-bytes BGR	*/
		NSR_PIXEL_ORDER_BGRA	= 3,	/**< 4-bytes BGRA	*/
		NSR_PIXEL_ORDER_RGBA	= 4	/**< 4-bytes RGBA	*/
	};

	/**
	 * @brief Tries to detect crops for given image data
	 * @param data Image data of the page.
	 * @param order	Pixel order of image data.
	 * @param width	Image width, in px.
	 * @param height Image height, in px.
	 * @param stride Image row stride, in bytes.
	 * @param widthLimit Minimum image width, 0 for no limits.
	 * @return Detected image crops.
	 */
	static NSRCropPads findCropPads (unsigned char *	data,
					 NSRPixelOrder		order,
					 int			width,
					 int			height,
					 int			stride,
					 int			widthLimit);

private:
	/** Hidden constructor */
	NSRPageCropper ();
};

#endif /* __NSRPAGECROPPER_H__ */
