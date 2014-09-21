#ifndef __NSRREADERCORE_GLOBAL_H__
#define __NSRREADERCORE_GLOBAL_H__

/**
 * @file nsrreadercore_global.h
 * @author Alexander Saprykin
 * @brief Global library header
 */

#include <qglobal.h>

/** @def NSRREADERCORE_SHARED Library export/import macro */
#ifdef NSRREADERCORE_LIBRARY
#  define NSRREADERCORE_SHARED	Q_DECL_EXPORT
#else
#  define NSRREADERCORE_SHARED	Q_DECL_IMPORT
#endif

#endif /* __NSRREADERCORE_GLOBAL_H__ */
