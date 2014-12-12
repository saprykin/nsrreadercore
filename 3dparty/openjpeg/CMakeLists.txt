include (TestBigEndian)
include (TestLargeFiles)
include (CheckIncludeFile)
include (EnsureFileInclude)

set (OPENJPEG_VERSION_MAJOR 2)
set (OPENJPEG_VERSION_MINOR 1)
set (OPENJPEG_VERSION_BUILD 0)
set (PACKAGE_VERSION
	"${OPENJPEG_VERSION_MAJOR}.${OPENJPEG_VERSION_MINOR}.${OPENJPEG_VERSION_BUILD}"
)

test_big_endian (OPJ_BIG_ENDIAN)

# Mandatory files
ensure_file_include ("string.h"		HAVE_STRING_H	YES)
ensure_file_include ("memory.h"		HAVE_MEMORY_H	YES)
ensure_file_include ("stdlib.h"		HAVE_STDLIB_H	YES)
ensure_file_include ("stdio.h"		HAVE_STDIO_H	YES)
ensure_file_include ("math.h"		HAVE_MATH_H	YES)
ensure_file_include ("float.h"		HAVE_FLOAT_H	YES)
ensure_file_include ("time.h"		HAVE_TIME_H	YES)
ensure_file_include ("stdarg.h"		HAVE_STDARG_H	YES)
ensure_file_include ("ctype.h"		HAVE_CTYPE_H	YES)
ensure_file_include ("assert.h"		HAVE_ASSERT_H	YES)

# For the following files, we provide an alternative, they are not mandatory
ensure_file_include ("stdint.h"		OPJ_HAVE_STDINT_H	NO)
ensure_file_include ("inttypes.h"	OPJ_HAVE_INTTYPES_H	NO)

# Why check this one ? for openjpip ?
check_include_file ("strings.h"		HAVE_STRINGS_H)
check_include_file ("sys/stat.h"	HAVE_SYS_STAT_H)
check_include_file ("sys/types.h"	HAVE_SYS_TYPES_H)
check_include_file ("unistd.h"		HAVE_UNISTD_H)

# Enable Large file support
NSR_TEST_LARGE_FILES (OPJ_HAVE_LARGEFILES)
set (OPJ_HAVE_FSEEKO ${NSR_HAVE_FSEEKO})

# For all builds, make sure openjpeg is std99 compliant:
# set (CMAKE_C_FLAGS "-Wall -std=c99 ${CMAKE_C_FLAGS}")
# FIXME: this setting prevented us from setting a coverage build.
# Do not use ffast-math for all build, it would produce incorrect results, only set for release:
if (CMAKE_COMPILER_IS_GNUCC)
	set (CMAKE_C_FLAGS_RELEASE "-ffast-math ${CMAKE_C_FLAGS_RELEASE}")
endif()

# On Visual Studio 8 MS deprecated C. This removes all 1.276E1265 security
# warnings
if (MSVC AND NOT ITK_ENABLE_VISUAL_STUDIO_DEPRECATED_C_WARNINGS)
	add_definitions(
		-D_CRT_FAR_MAPPINGS_NO_DEPRECATE
		-D_CRT_IS_WCTYPE_NO_DEPRECATE
		-D_CRT_MANAGED_FP_NO_DEPRECATE
		-D_CRT_NONSTDC_NO_DEPRECATE
		-D_CRT_SECURE_NO_DEPRECATE
		-D_CRT_SECURE_NO_DEPRECATE_GLOBALS
		-D_CRT_SETERRORMODE_BEEP_SLEEP_NO_DEPRECATE
		-D_CRT_TIME_FUNCTIONS_NO_DEPRECATE
		-D_CRT_VCCLRIT_NO_DEPRECATE
		-D_SCL_SECURE_NO_DEPRECATE
	)
endif()

configure_file (opj_config.h.in ${CMAKE_BINARY_DIR}/opj_config.h @ONLY)
configure_file (opj_config_private.h.in ${CMAKE_BINARY_DIR}/opj_config_private.h @ONLY)

set (SOURCES
	bio.c
	cio.c
	dwt.c
	event.c
	image.c
	j2k_lib.c
	j2k.c
	jp2.c
	jpt.c
	mct.c
	mqc.c
	openjpeg.c
	pi.c
	raw.c
	t1.c
	t2.c
	tcd.c
	tgt.c
)

set (HEADERS
	bio.h
	cio.h
	dwt.h
	event.h
	fix.h
	image.h
	int.h
	j2k_lib.h
	j2k.h
	jp2.h
	jpt.h
	mct.h
	mj2_convert.h
	mj2.h
	mqc.h
	openjpeg.h
	opj_includes.h
	opj_malloc.h
	pi.h
	raw.h
	t1_luts.h
	t1.h
	t2.h
	tcd.h
	tgt.h
)

include_directories (BEFORE ${CMAKE_BINARY_DIR})

add_library (openjpeg OBJECT ${SOURCES} ${HEADERS})
set_target_properties (openjpeg PROPERTIES POSITION_INDEPENDENT_CODE true)

set (OPENJPEG_DEFINITIONS "" PARENT_SCOPE)
set (OPENJPEG_INCLUDES ${OPENJPEG_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set (OPENJPEG_LIBRARIES "" PARENT_SCOPE)
