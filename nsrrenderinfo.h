#ifndef __NSRRENDERINFO_H__
#define __NSRRENDERINFO_H__

/**
 * @file nsrrenderinfo.h
 * @author Alexander Saprykin
 * @brief Page rendering info
 */

#include "nsrreadercore_global.h"

/**
 * @class NSRRenderInfo nsrrenderinfo.h
 * @brief Page rendering info class
 * @since 1.4.3
 *
 * Contains information about rendered page.
 */
class NSRRenderInfo
{
public:
	/**
	 * @brief Constructor
	 */
	NSRRenderInfo ();

	/**
	 * @brief Destructor
	 */
	~NSRRenderInfo ();

	/**
	 * @brief Gets page min zoom value
	 * @return Page min zoom value, in %.
	 * @since 1.4.3
	 */
	inline double getMinZoom () const {
		return _minZoom;
	}

	/**
	 * @brief Gets page max zoom value
	 * @return Page max zoom value, in %.
	 * @since 1.4.3
	 */
	inline double getMaxZoom () const {
		return _maxZoom;
	}

	/**
	 * @brief Checks whether render was successfull
	 * @return True if render was successfull, false otherwise.
	 * @since 1.4.3
	 */
	inline bool isSuccessRender () const {
		return _isSuccessRender;
	}

	/**
	 * @brief Sets page min zoom value
	 * @param minZoom Page min zoom value, in %.
	 * @since 1.4.3
	 */
	inline void setMinZoom (double minZoom) {
		_minZoom = minZoom;
	}

	/**
	 * @brief Sets page max zoom value
	 * @param maxZoom Page max zoom value, in %.
	 * @since 1.4.3
	 */
	inline void setMaxZoom (double maxZoom) {
		_maxZoom = maxZoom;
	}

	/**
	 * @brief Sets render success flag
	 * @param success Render success flag.
	 * @since 1.4.3
	 */
	inline void setSuccessRender (bool success) {
		_isSuccessRender = success;
	}

private:
	double	_minZoom;		/**< Min page zoom value, in %	*/
	double	_maxZoom;		/**< Max page zoom value, in %	*/
	bool	_isSuccessRender;	/**< Rendering result flag	*/
};

#endif /* __NSRRENDERINFO_H__ */
