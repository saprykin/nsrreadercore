include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckDIRSymbolExists)
include (TestLargeFiles)
include (TestBigEndian)

set (POPPLER_VERSION 0.28.1)

string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

if (SYSTEM_NAME MATCHES "windows")
	set (ENABLE_RELOCATABLE true)
endif()

test_big_endian (WORDS_BIGENDIAN)

check_include_files (stdint.h		HAVE_STDINT_H)
check_include_files (unistd.h		HAVE_UNISTD_H)

set (CMAKE_REQUIRED_FLAGS ${NSR_C_FLAGS_CHECK})

check_function_exists (fseeko		HAVE_FSEEKO)
check_function_exists (fseek64		HAVE_FSEEK64)
check_function_exists (ftell64		HAVE_FTELL64)
check_function_exists (gettimeofday	HAVE_GETTIMEOFDAY)
check_function_exists (gmtime_r		HAVE_GMTIME_R)
check_function_exists (mkstemp		HAVE_MKSTEMP)
check_function_exists (popen		HAVE_POPEN)
check_function_exists (randr_r		HAVE_RANDR_R)

NSR_TEST_LARGE_FILES (POPPLER_HAVE_LARGEFILES)
NSR_CHECK_DIRSYMBOL_EXISTS ("dirent.h"		HAVE_DIRENT_H)
NSR_CHECK_DIRSYMBOL_EXISTS ("ndir.h" 		HAVE_NDIR_H)
NSR_CHECK_DIRSYMBOL_EXISTS ("sys/ndir.h"	HAVE_SYS_NDIR_H)
NSR_CHECK_DIRSYMBOL_EXISTS ("sys/dir.h"		HAVE_SYS_DIR_H)

set (CMAKE_REQUIRED_FLAGS "")

find_package (Threads REQUIRED)

if (CMAKE_USE_PTHREADS_INIT)
	set (HAVE_PTHREAD true)
endif ()

if (SYSTEM_NAME MATCHES "windows")
	set (FONT_CONFIGURATION "win32")
else()
	set (FONT_CONFIGURATION "fontconfig")
endif()

if (FONT_CONFIGURATION STREQUAL "win32")
	set (WITH_FONTCONFIGURATION_WIN32 true)
elseif (FONT_CONFIGURATION STREQUAL "fontconfig")
	set (WITH_FONTCONFIGURATION_FONTCONFIG true)
else()
	message (FATAL_ERROR "Invalid font configuration setting: ${FONT_CONFIGURATION}")
endif()

if (WITH_FONTCONFIGURATION_WIN32)
	if (MINGW)
	# Set the minimum required Internet Explorer version to 5.0
	set (POPPLER_DEFINITIONS ${POPPLER_DEFINITIONS} -D_WIN32_IE=0x0500)
	endif()
endif()

if (WITH_FONTCONFIGURATION_FONTCONFIG)
	find_package (Fontconfig REQUIRED)
endif()

set (POPPLER_LIBRARIES ${POPPLER_LIBRARIES} ${Fontconfig_LIBRARIES})
set (POPPLER_INCLUDES ${POPPLER_INCLUDES} ${Fontconfig_INCLUDE_DIR})

find_package (Freetype REQUIRED)

set (HAVE_FREETYPE_H true)
set (POPPLER_LIBRARIES ${POPPLER_LIBRARIES} ${FREETYPE_LIBRARIES})
set (POPPLER_INCLUDES ${POPPLER_INCLUDES} ${FREETYPE_INCLUDE_DIRS})

# Enable these unconditionally.
set (MULTITHREADED true)
set (OPI_SUPPORT true)
set (TEXTOUT_WORD_LIST true)
set (HAVE_SPLASH true)

set (ENABLE_LIBJPEG true)
set (ENABLE_LIBPNG true)
set (ENABLE_LIBTIFF true)
set (ENABLE_LIBOPENJPEG true)

set (HAVE_LIBTIFF true)
set (HAVE_TIFFIO_H true)

find_package (PNG)
set (ENABLE_LIBPNG ${PNG_FOUND})

if (PNG_FOUND)
	set (POPPLER_LIBRARIES ${POPPLER_LIBRARIES} ${PNG_LIBRARIES})
	set (POPPLER_INCLUDES ${POPPLER_INCLUDES} ${PNG_INCLUDE_DIRS})
	set (POPPLER_DEFINITIONS ${POPPLER_DEFINITIONS} ${PNG_DEFINITIONS})
endif()

find_package (ZLIB)
set (ENABLE_ZLIB ${ZLIB_FOUND})

if (ZLIB_FOUND)
	set (HAVE_LIBZ true)
	set (HAVE_ZLIB_H true)
	set (POPPLER_LIBRARIES ${POPPLER_LIBRARIES} ${ZLIB_LIBRARIES})
	set (POPPLER_INCLUDES ${POPPLER_INCLUDES} ${ZLIB_INCLUDE_DIRS})
endif()

set (USE_CMS false)
set (USE_LCMS1 false)
set (USE_LCMS2 false)

find_package (CURL)
set (ENABLE_LIBCURL ${CURL_FOUND})

if (CURL_FOUND)
	set (POPPLER_HAS_CURL_SUPPORT true)
	set (POPPLER_LIBRARIES ${POPPLER_LIBRARIES} ${CURL_LIBRARIES})
	set (POPPLER_INCLUDES ${POPPLER_INCLUDES} ${CURL_INCLUDE_DIRS})
endif()

configure_file (config-pdf.h.in "${CMAKE_BINARY_DIR}/config-pdf.h")
configure_file (poppler/poppler-config.h.in "${CMAKE_BINARY_DIR}/poppler-config.h")

set (SOURCES
	fofi/FoFiBase.cc
	fofi/FoFiEncodings.cc
	fofi/FoFiIdentifier.cc
	fofi/FoFiTrueType.cc
	fofi/FoFiType1.cc
	fofi/FoFiType1C.cc
	goo/FixedPoint.cc
	goo/gfile.cc
	goo/gmem.cc
	goo/gmempp.cc
	goo/GooHash.cc
	goo/GooList.cc
	goo/GooString.cc
	goo/GooTimer.cc
	goo/grandom.cc
	goo/gstrtod.cc
	goo/ImgWriter.cc
	goo/JpegWriter.cc
	goo/NetPBMWriter.cc
	goo/TiffWriter.cc
	poppler/Annot.cc
	poppler/Array.cc
	poppler/BuiltinFont.cc
	poppler/BuiltinFontTables.cc
	poppler/CachedFile.cc
	poppler/Catalog.cc
	poppler/CharCodeToUnicode.cc
	poppler/CMap.cc
	poppler/DateInfo.cc
	poppler/DCTStream.cc
	poppler/Decrypt.cc
	poppler/Dict.cc
	poppler/Error.cc
	poppler/FileSpec.cc
	poppler/FontEncodingTables.cc
	poppler/FontInfo.cc
	poppler/Form.cc
	poppler/Function.cc
	poppler/Gfx.cc
	poppler/GfxFont.cc
	poppler/GfxState.cc
	poppler/GlobalParams.cc
	poppler/Hints.cc
	poppler/JArithmeticDecoder.cc
	poppler/JBIG2Stream.cc
	poppler/Lexer.cc
	poppler/Linearization.cc
	poppler/Link.cc
	poppler/LocalPDFDocBuilder.cc
	poppler/MarkedContentOutputDev.cc
	poppler/Movie.cc
	poppler/NameToCharCode.cc
	poppler/Object.cc
	poppler/OptionalContent.cc
	poppler/Outline.cc
	poppler/OutputDev.cc
	poppler/Page.cc
	poppler/PageLabelInfo.cc
	poppler/PageTransition.cc
	poppler/Parser.cc
	poppler/PDFDoc.cc
	poppler/PDFDocEncoding.cc
	poppler/PDFDocFactory.cc
	poppler/PopplerCache.cc
	poppler/PreScanOutputDev.cc
	poppler/ProfileData.cc
	poppler/PSOutputDev.cc
	poppler/PSTokenizer.cc
	poppler/Rendition.cc
	poppler/SecurityHandler.cc
	poppler/Sound.cc
	poppler/SplashOutputDev.cc
	poppler/StdinCachedFile.cc
	poppler/StdinPDFDocBuilder.cc
	poppler/Stream.cc
	poppler/strtok_r.cpp
	poppler/StructElement.cc
	poppler/StructTreeRoot.cc
	poppler/TextOutputDev.cc
	poppler/UnicodeMap.cc
	poppler/UnicodeTypeTable.cc
	poppler/UTF.cc
	poppler/ViewerPreferences.cc
	poppler/XpdfPluginAPI.cc
	poppler/XRef.cc
	splash/Splash.cc
	splash/SplashBitmap.cc
	splash/SplashClip.cc
	splash/SplashFont.cc
	splash/SplashFontEngine.cc
	splash/SplashFontFile.cc
	splash/SplashFontFileID.cc
	splash/SplashFTFont.cc
	splash/SplashFTFontEngine.cc
	splash/SplashFTFontFile.cc
	splash/SplashPath.cc
	splash/SplashPattern.cc
	splash/SplashScreen.cc
	splash/SplashState.cc
	splash/SplashT1Font.cc
	splash/SplashT1FontEngine.cc
	splash/SplashT1FontFile.cc
	splash/SplashXPath.cc
	splash/SplashXPathScanner.cc
)

if (ENABLE_LIBPNG)
	set (SOURCES ${SOURCES} goo/PNGWriter.cc)
endif()

if (ENABLE_ZLIB)
	set (SOURCES ${SOURCES} poppler/FlateStream.cc)
endif()

if (ENABLE_LIBCURL)
	set (SOURCES ${SOURCES}
	poppler/CurlCachedFile.cc
	poppler/CurlPDFDocBuilder.cc
)
endif()

if (ENABLE_LIBOPENJPEG)
	set (SOURCES ${SOURCES} poppler/JPEG2000Stream.cc)
else ()
	set (SOURCES ${SOURCES} poppler/JPXStream.cc)
endif()

set (HEADERS
	fofi/FoFiBase.h
	fofi/FoFiEncodings.h
	fofi/FoFiIdentifier.h
	fofi/FoFiTrueType.h
	fofi/FoFiType1.h
	fofi/FoFiType1C.h
	goo/FixedPoint.h
	goo/gfile.h
	goo/gmem.h
	goo/GooHash.h
	goo/GooLikely.h
	goo/GooList.h
	goo/GooMutex.h
	goo/GooString.h
	goo/GooTimer.h
	goo/grandom.h
	goo/gstrtod.h
	goo/gtypes_p.h
	goo/gtypes.h
	goo/ImgWriter.h
	goo/JpegWriter.h
	goo/NetPBMWriter.h
	goo/TiffWriter.h
	poppler/Rendition.h
	poppler/Stream-CCITT.h
	poppler/Stream.h
	poppler/Annot.h
	poppler/Array.h
	poppler/BuiltinFont.h
	poppler/BuiltinFontTables.h
	poppler/CachedFile.h
	poppler/Catalog.h
	poppler/CharCodeToUnicode.h
	poppler/CharTypes.h
	poppler/CMap.h
	poppler/CompactFontTables.h
	poppler/DateInfo.h
	poppler/DCTStream.h
	poppler/Decrypt.h
	poppler/Dict.h
	poppler/Error.h
	poppler/ErrorCodes.h
	poppler/FileSpec.h
	poppler/FlateStream.h
	poppler/FontEncodingTables.h
	poppler/FontInfo.h
	poppler/Form.h
	poppler/Function.h
	poppler/Gfx.h
	poppler/GfxFont.h
	poppler/GfxState_helpers.h
	poppler/GfxState.h
	poppler/GlobalParams.h
	poppler/Hints.h
	poppler/JArithmeticDecoder.h
	poppler/JBIG2Stream.h
	poppler/Lexer.h
	poppler/Linearization.h
	poppler/Link.h
	poppler/LocalPDFDocBuilder.h
	poppler/MarkedContentOutputDev.cc
	poppler/Movie.h
	poppler/NameToCharCode.h
	poppler/NameToUnicodeTable.h
	poppler/Object.h
	poppler/OptionalContent.h
	poppler/Outline.h
	poppler/OutputDev.h
	poppler/Page.h
	poppler/PageLabelInfo.h
	poppler/PageLabelInfo_p.h
	poppler/PageTransition.h
	poppler/Parser.h
	poppler/PDFDoc.h
	poppler/PDFDocBuilder.h
	poppler/PDFDocEncoding.h
	poppler/PDFDocFactory.h
	poppler/PopplerCache.h
	poppler/PreScanOutputDev.h
	poppler/ProfileData.h
	poppler/PSOutputDev.h
	poppler/PSTokenizer.h
	poppler/Rendition.h
	poppler/SecurityHandler.h
	poppler/Sound.h
	poppler/SplashOutputDev.h
	poppler/StdinCachedFile.h
	poppler/StdinPDFDocBuilder.h
	poppler/Stream-CCITT.h
	poppler/Stream.h
	poppler/StructElement.h
	poppler/StructTreeRoot.h
	poppler/TextOutputDev.h
	poppler/UnicodeCClassTables.h
	poppler/UnicodeCompTables.h
	poppler/UnicodeDecompTables.h
	poppler/UnicodeMap.h
	poppler/UnicodeMapTables.h
	poppler/UnicodeTypeTable.h
	poppler/UTF.h
	poppler/UTF8.h
	poppler/ViewerPreferences.h
	poppler/XpdfPluginAPI.h
	poppler/XRef.h
	splash/Splash.h
	splash/SplashBitmap.h
	splash/SplashClip.h
	splash/SplashErrorCodes.h
	splash/SplashFont.h
	splash/SplashFontEngine.h
	splash/SplashFontFile.h
	splash/SplashFontFileID.h
	splash/SplashFTFont.h
	splash/SplashFTFontEngine.h
	splash/SplashFTFontFile.h
	splash/SplashGlyphBitmap.h
	splash/SplashMath.h
	splash/SplashPath.h
	splash/SplashPattern.h
	splash/SplashScreen.h
	splash/SplashState.h
	splash/SplashT1Font.h
	splash/SplashT1FontEngine.h
	splash/SplashT1FontFile.h
	splash/SplashTypes.h
	splash/SplashXPath.h
	splash/SplashXPathScanner.h
)

if (ENABLE_LIBPNG)
	set (HEADERS ${HEADERS} goo/PNGWriter.h)
endif()

if (ENABLE_LIBCURL)
	set (HEADERS ${HEADERS}
		poppler/CurlCachedFile.h
		poppler/CurlPDFDocBuilder.h
	)
endif()

if (ENABLE_LIBOPENJPEG)
	set (HEADERS ${HEADERS} poppler/JPEG2000Stream.h)
else()
	set (HEADERS ${HEADERS} poppler/JPXStream.h)
endif()

if (SYSTEM_NAME MATCHES "windows")
	# gdi32 is needed under win32
	set (POPPLER_LIBRARIES ${POPPLER_LIBRARIES} gdi32)
endif()

include_directories (BEFORE
	${POPPLER_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/fofi
	${CMAKE_CURRENT_SOURCE_DIR}/goo
	${CMAKE_CURRENT_SOURCE_DIR}/poppler
	${CMAKE_BINARY_DIR})
add_definitions (${POPPLER_DEFINITIONS})
add_library (poppler OBJECT ${SOURCES} ${HEADERS})
set_target_properties (poppler PROPERTIES POSITION_INDEPENDENT_CODE true)

set (POPPLER_DEFINITIONS ${POPPLER_DEFINITIONS} PARENT_SCOPE)
set (POPPLER_INCLUDES ${POPPLER_INCLUDES}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/fofi
	${CMAKE_CURRENT_SOURCE_DIR}/goo
	${CMAKE_CURRENT_SOURCE_DIR}/poppler
	${CMAKE_BINARY_DIR}
	PARENT_SCOPE
)
set (POPPLER_LIBRARIES ${POPPLER_LIBRARIES} PARENT_SCOPE)
