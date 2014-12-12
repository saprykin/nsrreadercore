cmake_minimum_required (VERSION 2.8.11)
project (nsrreadercore C CXX)

string (TOLOWER ${CMAKE_C_COMPILER_ID} COMPILER_ID)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug")
endif()

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

set (CMAKE_AUTOMOC true)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-Bsymbolic")
endif()

if (CMAKE_COMPILER_IS_GNUCXX OR COMPILER_ID STREQUAL "clang")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-function")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-unused-function")
endif()

if (COMPILER_ID STREQUAL "clang")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-private-field -Wno-unused-const-variable")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field -Wno-unused-const-variable")
endif()

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${NSR_C_FLAGS_DEBUG}")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${NSR_C_FLAGS_RELEASE}")
set (CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${NSR_C_FLAGS_MINSIZEREL}")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${NSR_C_FLAGS_RELWITHDEBINFO}")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${NSR_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${NSR_CXX_FLAGS_RELEASE}")
set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${NSR_CXX_FLAGS_MINSIZEREL}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${NSR_CXX_FLAGS_RELWITHDEBINFO}")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions (${NSR_PLATFORM_DEFINES_DEBUG})
else()
	add_definitions (${NSR_PLATFORM_DEFINES_RELEASE})
endif()

include_directories (BEFORE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR})

add_subdirectory (3dparty/jbigkit)
add_subdirectory (3dparty/openjpeg)
add_subdirectory (3dparty/libjpeg-turbo)
add_subdirectory (3dparty/tiff)
add_subdirectory (3dparty/djvu)
add_subdirectory (3dparty/poppler)
add_subdirectory (3dparty/universalchardet)

add_dependencies (djvulibre jpegturbo)
add_dependencies (djvulibre tiff)
add_dependencies (poppler jpegturbo)
add_dependencies (poppler tiff)
add_dependencies (poppler openjpeg)
add_dependencies (tiff jpegturbo)
add_dependencies (tiff jbigkit)

set (SOURCES
	nsrabstractdocument.cpp
	nsrcharsetdetector.cpp
	nsrcroppads.cpp
	nsrcroppadscache.cpp
	nsrdjvudocument.cpp
	nsrpagecropper.cpp
	nsrpagescache.cpp
	nsrpopplerdocument.cpp
	nsrreadercore.cpp
	nsrrenderedpage.cpp
	nsrrenderinfo.cpp
	nsrrenderrequest.cpp
	nsrrenderthread.cpp
	nsrsession.cpp
	nsrtextdocument.cpp
	nsrtiffdocument.cpp
)

set (HEADERS
	insrsettings.h
	insrthumbnailer.h
	nsrabstractdocument.h
	nsrcroppads.h
	nsrcroppadscache.h
	nsrdjvudocument.h
	nsrpagecropper.h
	nsrpagescache.h
	nsrpopplerdocument.h
	nsrreadercore.h
	nsrreadercore_global.h
	nsrrenderedpage.h
	nsrrenderinfo.h
	nsrrenderrequest.h
	nsrrenderthread.h
	nsrsession.h
	nsrtextdocument.h
	nsrtiffdocument.h
)

include_directories (
	BEFORE
	3dparty
	${DJVU_INCLUDES}
	${LIBJPEGTURBO_INCLUDES}
	${POPPLER_INCLUDES}
	${TIFF_INCLUDES}
	${UNIVERSALCHARDET_INCLUDES}
	${JBIGKIT_INCLUDES}
	${OPENJPEG_INCLUDES}
)

include_directories (AFTER ${NSR_INCLUDE_DIRECTORIES})

add_library (${PROJECT_NAME} SHARED
	${SOURCES}
	${THIRDPARTY_SOURCES}
	${HEADERS}
	${THIRDPARTY_HEADERS}
	$<TARGET_OBJECTS:jbigkit>
	$<TARGET_OBJECTS:jpegturbo>
	$<TARGET_OBJECTS:openjpeg>
	$<TARGET_OBJECTS:tiff>
	$<TARGET_OBJECTS:djvulibre>
	$<TARGET_OBJECTS:poppler>
	$<TARGET_OBJECTS:universalchardet>
)

add_definitions ("${DJVU_DEFINITIONS}
	${LIBJPEGTURBO_DEFINITIONS}
	${POPPLER_DEFINITIONS}
	${TIFF_DEFINITIONS}
	${UNIVERSALCHARDET_DEFINITIONS}
	${JBIGKIT_DEFINITIONS}
	${OPENJPEG_DEFINITIONS}"
)

target_link_libraries (${PROJECT_NAME}
	${QT_LIBRARIES}
	${NSR_LIBRARIES}
	${DJVU_LIBRARIES}
	${LIBJPEGTURBO_LIBRARIES}
	${POPPLER_LIBRARIES}
	${TIFF_LIBRARIES}
	${UNIVERSALCHARDET_LIBRARIES}
	${JBIGKIT_LIBRARIES}
	${OPENJPEG_LIBRARIES}
)

add_definitions (-DNSRREADERCORE_LIBRARY)
