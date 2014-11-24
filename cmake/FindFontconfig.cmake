# - Try to find the Fontconfig
# Once done this will define
#
#  Fontconfig_FOUND - system has Fontconfig
#  Fontconfig_INCLUDE_DIR - The include directory to use for the Fontconfig headers
#  Fontconfig_LIBRARIES - Link these to use Fontconfig

include (FindPackageHandleStandardArgs)

if (Fontconfig_LIBRARIES AND Fontconfig_INCLUDE_DIR)
	# In cache already
	set (Fontconfig_FOUND true)
else()
	find_path (Fontconfig_INCLUDE_DIR fontconfig/fontconfig.h
		PATHS
		/usr/X11/include
	)

	find_library (Fontconfig_LIBRARIES NAMES fontconfig)

	find_package_handle_standard_args (Fontconfig
		FOUND_VAR
		Fontconfig_FOUND
		REQUIRED_VARS
		Fontconfig_LIBRARIES
		Fontconfig_INCLUDE_DIR
	)

	mark_as_advanced (
		Fontconfig_LIBRARIES
		Fontconfig_INCLUDE_DIR
	)
endif()
