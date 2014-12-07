include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckTypeSize)
include (CheckCInline)
include (TestLargeFiles)
include (TestBigEndian)

set (TIFF_VERSION 4.0.3)

string (TOLOWER ${CMAKE_C_COMPILER_ID} COMPILER_ID)

if (COMPILER_ID STREQUAL "clang")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-to-void-pointer-cast")
endif()

check_include_files (assert.h		HAVE_ASSERT_H)
check_include_files (fcntl.h		HAVE_FCNTL_H)
check_include_files (io.h		HAVE_IO_H)
check_include_files (search.h		HAVE_SEARCH_H)
check_include_files (string.h		HAVE_STRING_H)
check_include_files (unistd.h		HAVE_UNISTD_H)
check_include_files (sys/types.h	HAVE_SYS_TYPES_H)

set (CMAKE_REQUIRED_FLAGS ${NSR_C_FLAGS_CHECK})

check_function_exists (mmap HAVE_MMAP)

set (HAVE_IEEEFP true)
set (CXX_SUPPORT true)

check_type_size ("signed short"		SIZEOF_SIGNED_SHORT)
check_type_size ("unsigned short"	SIZEOF_UNSIGNED_SHORT)
check_type_size ("signed int"		SIZEOF_SIGNED_INT)
check_type_size ("unsigned int"		SIZEOF_UNSIGNED_INT)
check_type_size ("signed long"		SIZEOF_SIGNED_LONG)
check_type_size ("unsigned long"	SIZEOF_UNSIGNED_LONG)
check_type_size ("signed long long"	SIZEOF_SIGNED_LONG_LONG)
check_type_size ("unsigned long long"	SIZEOF_UNSIGNED_LONG_LONG)
check_type_size ("unsigned char *"	SIZEOF_UNSIGNED_CHAR_P)
check_type_size ("ptrdiff_t"		SIZEOF_PTRDIFFT_T)
check_type_size ("off_t"		SIZEOF_OFF_T)
check_type_size ("size_t"		SIZEOF_SIZE_T)

set (TIFF_INT8_T	"signed char")
set (TIFF_UINT8_T	"unsigned char")
set (TIFF_INT16_T	"signed short")
set (TIFF_UINT16_T	"unsigned short")

if (SIZEOF_SIGNED_INT EQUAL 4)
	set (TIFF_INT32_T "signed int")
	set (TIFF_INT32_FORMAT "%d")
elseif (SIZEOF_SIGNED_LONG EQUAL 4)
	set (TIFF_INT32_T "signed long")
	set (TIFF_INT32_FORMAT "%ld")
else()
	set (TIFF_INT32_T "none")
	set (TIFF_INT32_FORMAT "none")
endif()

if (SIZEOF_UNSIGNED_INT EQUAL 4)
	set (TIFF_UINT32_T "unsigned int")
	set (TIFF_UINT32_FORMAT "%u")
elseif (SIZEOF_UNSIGNED_LONG EQUAL 4)
	set (TIFF_UINT32_T "unsigned long")
	set (TIFF_UINT32_FORMAT "%lu")
else()
	set (TIFF_UINT32_T "none")
	set (TIFF_UINT32_FORMAT "none")
endif()

if (SIZEOF_SIGNED_LONG EQUAL 8)
	set (TIFF_INT64_T "signed long")
	set (TIFF_INT64_FORMAT "%ld")
elseif (SIZEOF_SIGNED_LONG_LONG EQUAL 8)
	set (TIFF_INT64_T "signed long long")

	if (MINGW)
		set (TIFF_INT64_FORMAT "%I64d")
	else()
		set (TIFF_INT64_FORMAT "%lld")
	endif()
else()
	set (TIFF_INT64_T "none")
	set (TIFF_INT64_FORMAT "none")
endif()

if (SIZEOF_UNSIGNED_LONG EQUAL 8)
	set (TIFF_UINT64_T "unsigned long")
	set (TIFF_UINT64_FORMAT "%lu")
elseif (SIZEOF_UNSIGNED_LONG_LONG EQUAL 8)
	set (TIFF_UINT64_T "unsigned long long")

	if (MINGW)
		set (TIFF_UINT64_FORMAT "%I64u")
	else()
		set (TIFF_UINT64_FORMAT "%llu")
	endif()
else()
	set (TIFF_UINT64_T "none")
	set (TIFF_UINT64_FORMAT "none")
endif()

if (SIZEOF_SIGNED_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
	set (TIFF_SSIZE_T "signed long")
	set (TIFF_SSIZE_FORMAT "%ld")
elseif (SIZEOF_SIGNED_LONG_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
	set (TIFF_SSIZE_T "signed long long")

	if (MINGW)
		set (TIFF_SSIZE_FORMAT "%I64d")
	else()
		set (TIFF_SSIZE_FORMAT "%lld")
	endif()
else()
	set (TIFF_SSIZE_T "unknown")
	set (TIFF_SSIZE_FORMAT "unknown")
endif()

if (NOT HAVE_SIZEOF_PTRDIFF_T)
	set (TIFF_PTRDIFF_T ${TIFF_SSIZE_T})
	set (TIFF_PTRDIFF_FORMAT ${TIFF_SSIZE_FORMAT})
else()
	set (TIFF_PTRDIFF_T "ptrdiff_t")
	set (TIFF_PTRDIFF_FORMAT "%ld")
endif()

if (NOT HAVE_SIZEOF_OFF_T)
	if (SIZEOF_SIGNED_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
		set (TIFF_OFF_T "signed long")
	elseif (SIZEOF_SIGNED_LONG_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
		set (TIFF_OFF_T "signed long long")
	else()
		set (TIFF_OFF_T "signed int")
	endif()

	set (HAVE_CUSTOM_OFF_T true)
endif()

if (NOT HAVE_SIZEOF_SIZE_T)
	if (SIZEOF_UNSIGNED_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
		set (TIFF_SIZE_T "unsigned long")
	elseif (SIZEOF_UNSIGNED_LONG_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
		set (TIFF_SIZE_T "unsigned long long")
	else()
		set (TIFF_SIZE_T "unsigned int")
	endif()

	set (HAVE_CUSTOM_SIZE_T true)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR
    CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
	set (TIFF_HOST_FILLORDER "FILLORDER_LSB2MSB")
else()
	set (TIFF_HOST_FILLORDER "FILLORDER_MSB2LSB")
endif()

test_big_endian (HOST_BIGENDIAN)
set (WORDS_BIGENDIAN ${HOST_BIGENDIAN})

NSR_TEST_LARGE_FILES (TIFF_HAVE_LARGEFILES)
NSR_CHECK_C_INLINE (TIFF_C_INLINE)

if (NOT TIFF_C_INLINE STREQUAL "inline")
	set (HAVE_CUSTOM_INLINE true)
	set (TIFF_INLINE_MODIFIER ${TIFF_C_INLINE})
endif()

set (CCITT_SUPPORT		true)
set (PACKBITS_SUPPORT 		true)
set (LZW_SUPPORT 		true)
set (THUNDER_SUPPORT 		true)
set (NEXT_SUPPORT 		true)
set (LOGLUV_SUPPORT 		true)
set (MDI_SUPPORT 		true)
set (OJPEG_SUPPORT 		true)
set (JBIG_SUPPORT 		false)
set (JPEG_SUPPORT 		true)
set (JPEG_DUAL_MODE_8_12	false)
set (LIBJPEG_12_PATH 		"")

find_package (ZLIB)

if (ZLIB_FOUND)
	set (ZIP_SUPPORT true)
	set (PIXARLOG_SUPPORT true)
	set (TIFF_LIBRARIES ${TIFF_LIBRARIES} ${ZLIB_LIBRARIES})
	set (TIFF_INCLUDES ${TIFF_INCLUDES} ${ZLIB_INCLUDE_DIRS})
endif()

find_package (LibLZMA)

if (LIBLZMA_FOUND)
	set (LZMA_SUPPORT true)
	set (TIFF_LIBRARIES ${TIFF_LIBRARIES} ${LIBLZMA_LIBRARIES})
	set (TIFF_INCLUDES ${TIFF_INCLUDES} ${LIBLZMA_INCLUDE_DIRS})
endif()

if (NOT CYGWIN)
	check_include_files (windows.h HAVE_WINDOWS_H)

	if (HAVE_WINDOWS_H)
		set (USE_WIN32_FILEIO true)
	endif()
endif()

set (STRIPCHOP_DEFAULT			"TIFF_STRIPCHOP")
set (STRIP_SIZE_DEFAULT			8192)
set (DEFER_STRILE_LOAD			false)
set (CHUNKY_STRIP_READ_SUPPORT		false)
set (SUBIFD_SUPPORT			true)
set (DEFAULT_EXTRASAMPLE_AS_ALPHA	true)
set (CHECK_JPEG_YCBCR_SUBSAMPLING	true)

set (CMAKE_REQUIRED_FLAGS "")

configure_file (tif_config.h.in ${CMAKE_BINARY_DIR}/tif_config.h)
configure_file (tiffconf.h.in ${CMAKE_BINARY_DIR}/tiffconf.h)

set (SOURCES
	tif_aux.c
	tif_close.c
	tif_codec.c
	tif_color.c
	tif_compress.c
	tif_dir.c
	tif_dirinfo.c
	tif_dirread.c
	tif_dirwrite.c
	tif_dumpmode.c
	tif_error.c
	tif_extension.c
	tif_fax3.c
	tif_fax3sm.c
	tif_flush.c
	tif_getimage.c
	tif_jbig.c
	tif_jpeg_12.c
	tif_jpeg.c
	tif_luv.c
	tif_lzma.c
	tif_lzw.c
	tif_next.c
	tif_ojpeg.c
	tif_open.c
	tif_packbits.c
	tif_pixarlog.c
	tif_predict.c
	tif_print.c
	tif_read.c
	tif_stream.cxx
	tif_strip.c
	tif_swab.c
	tif_thunder.c
	tif_tile.c
	tif_version.c
	tif_warning.c
	tif_write.c
	tif_zip.c
)

if (USE_WIN32_FILEIO)
	set (SOURCES ${SOURCES} tif_win32.c)
else()
	set (SOURCES ${SOURCES} tif_unix.c)
endif()

set (HEADERS
	t4.h
	tif_dir.h
	tif_fax3.h
	tif_predict.h
	tiff.h
	tiffio.h
	tiffiop.h
	tiffvers.h
	uvcode.h
)

include_directories (BEFORE
	${TIFF_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_BINARY_DIR})
add_definitions (${TIFF_DEFINITIONS})
add_library (tiff OBJECT ${SOURCES} ${HEADERS})
set_target_properties (tiff PROPERTIES POSITION_INDEPENDENT_CODE true)

set (TIFF_DEFINITIONS ${TIFF_DEFINITIONS} PARENT_SCOPE)
set (TIFF_INCLUDES ${TIFF_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_BINARY_DIR} PARENT_SCOPE)
set (TIFF_LIBRARIES ${TIFF_LIBRARIES} PARENT_SCOPE)
