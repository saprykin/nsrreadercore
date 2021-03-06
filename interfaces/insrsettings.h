#ifndef __INSRSETTINGS_H__
#define __INSRSETTINGS_H__

/**
 * @file insrsettings.h
 * @author Alexander Saprykin
 * @brief Interface for rendering settings
 */

#include "nsrreadercore_global.h"

#include <QString>

/**
 * @class INSRSettings insrsettings.h
 * @brief Interface for rendering settings
 * @since 1.4.2
 *
 * #INSRSettings interface represents rendering settings.
 */
class NSRREADERCORE_SHARED INSRSettings
{
public:
	/** Destructor */
	virtual ~INSRSettings () {}

	/**
	 * @brief Checks whether word wrap is enabled
	 * @return True if word wrap is enabled, false otherwise.
	 * @since 1.4.2
	 */
	virtual bool isWordWrap () const = 0;

	/**
	 * @brief Checks whether inverted colors is enabled
	 * @return True if inverted colors is enabled, false otherwise.
	 * @since 1.4.2
	 */
	virtual bool isInvertedColors () const = 0;

	/**
	 * @brief Checks whether page autocrop is enabled
	 * @return True if page autocrop is enabled, false otherwise.
	 * @since 1.4.2
	 */
	virtual bool isAutoCrop () const = 0;

	/**
	 * @brief Checks whether text encoding autodetection is enabled.
	 * @return True if text encoding autodetection is enabled, false otherwise.
	 * @since 1.4.2
	 */
	virtual bool isEncodingAutodetection () const = 0;

	/**
	 * @brief Gets text encoding name
	 * @return Text encoding name.
	 * @since 1.4.2
	 */
	virtual QString getTextEncoding () const = 0;

	/**
	 * @brief Checks whether the app is starting
	 * @return True is app is starting, false otherwise.
	 * @since 1.4.2
	 */
	virtual bool isStarting () const = 0;
};

#endif /* __INSRSETTINGS_H__ */
