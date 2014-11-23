cmake_minimum_required (VERSION 2.8.11)
project (nsrreadercore C CXX)
enable_language (ASM_NASM)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug")
endif()

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

if (NOT NSR_PLATFORM)
	string (TOLOWER ${CMAKE_SYSTEM_NAME} NSR_PLATFORM)
endif()

set (CMAKE_AUTOMOC true)

# Enable SIMD instructions for JPEG decoding
set (LIBJPEGTURBO_WITH_SIMD true)

find_package (Qt4 REQUIRED)
include (${QT_USE_FILE})

add_subdirectory (3dparty/djvu)

foreach (DJVU_FILE ${DJVU_SOURCES})
	set (DJVU_FILE "3dparty/djvu/${DJVU_FILE}")
	set (THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${DJVU_FILE})
endforeach()

foreach (DJVU_FILE ${DJVU_HEADERS})
	set (DJVU_FILE "3dparty/djvu/${DJVU_FILE}")
	set (THIRDPARTY_HEADERS ${THIRDPARTY_HEADERS} ${DJVU_FILE})
endforeach()

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

set (POPPLER_SOURCES
	3dparty/poppler/fofi/FoFiBase.cc
	3dparty/poppler/fofi/FoFiEncodings.cc
	3dparty/poppler/fofi/FoFiIdentifier.cc
	3dparty/poppler/fofi/FoFiTrueType.cc
	3dparty/poppler/fofi/FoFiType1.cc
	3dparty/poppler/fofi/FoFiType1C.cc
	3dparty/poppler/goo/FixedPoint.cc
	3dparty/poppler/goo/gfile.cc
	3dparty/poppler/goo/gmem.cc
	3dparty/poppler/goo/gmempp.cc
	3dparty/poppler/goo/GooHash.cc
	3dparty/poppler/goo/GooList.cc
	3dparty/poppler/goo/GooString.cc
	3dparty/poppler/goo/GooTimer.cc
	3dparty/poppler/goo/grandom.cc
	3dparty/poppler/goo/gstrtod.cc
	3dparty/poppler/goo/ImgWriter.cc
	3dparty/poppler/goo/JpegWriter.cc
	3dparty/poppler/goo/PNGWriter.cc
	3dparty/poppler/goo/TiffWriter.cc
	3dparty/poppler/poppler/Annot.cc
	3dparty/poppler/poppler/Array.cc
	3dparty/poppler/poppler/BuiltinFont.cc
	3dparty/poppler/poppler/BuiltinFontTables.cc
	3dparty/poppler/poppler/CachedFile.cc
	3dparty/poppler/poppler/Catalog.cc
	3dparty/poppler/poppler/CharCodeToUnicode.cc
	3dparty/poppler/poppler/CMap.cc
	3dparty/poppler/poppler/CurlCachedFile.cc
	3dparty/poppler/poppler/CurlPDFDocBuilder.cc
	3dparty/poppler/poppler/DateInfo.cc
	3dparty/poppler/poppler/DCTStream.cc
	3dparty/poppler/poppler/Decrypt.cc
	3dparty/poppler/poppler/Dict.cc
	3dparty/poppler/poppler/Error.cc
	3dparty/poppler/poppler/FileSpec.cc
	3dparty/poppler/poppler/FlateStream.cc
	3dparty/poppler/poppler/FontEncodingTables.cc
	3dparty/poppler/poppler/FontInfo.cc
	3dparty/poppler/poppler/Form.cc
	3dparty/poppler/poppler/Function.cc
	3dparty/poppler/poppler/Gfx.cc
	3dparty/poppler/poppler/GfxFont.cc
	3dparty/poppler/poppler/GfxState.cc
	3dparty/poppler/poppler/GlobalParams.cc
	3dparty/poppler/poppler/Hints.cc
	3dparty/poppler/poppler/JArithmeticDecoder.cc
	3dparty/poppler/poppler/JBIG2Stream.cc
	3dparty/poppler/poppler/JPXStream.cc
	3dparty/poppler/poppler/Lexer.cc
	3dparty/poppler/poppler/Linearization.cc
	3dparty/poppler/poppler/Link.cc
	3dparty/poppler/poppler/LocalPDFDocBuilder.cc
	3dparty/poppler/poppler/Movie.cc
	3dparty/poppler/poppler/NameToCharCode.cc
	3dparty/poppler/poppler/Object.cc
	3dparty/poppler/poppler/OptionalContent.cc
	3dparty/poppler/poppler/Outline.cc
	3dparty/poppler/poppler/OutputDev.cc
	3dparty/poppler/poppler/Page.cc
	3dparty/poppler/poppler/PageLabelInfo.cc
	3dparty/poppler/poppler/PageTransition.cc
	3dparty/poppler/poppler/Parser.cc
	3dparty/poppler/poppler/PDFDoc.cc
	3dparty/poppler/poppler/PDFDocEncoding.cc
	3dparty/poppler/poppler/PDFDocFactory.cc
	3dparty/poppler/poppler/PopplerCache.cc
	3dparty/poppler/poppler/PreScanOutputDev.cc
	3dparty/poppler/poppler/ProfileData.cc
	3dparty/poppler/poppler/PSOutputDev.cc
	3dparty/poppler/poppler/PSTokenizer.cc
	3dparty/poppler/poppler/Rendition.cc
	3dparty/poppler/poppler/SecurityHandler.cc
	3dparty/poppler/poppler/Sound.cc
	3dparty/poppler/poppler/SplashOutputDev.cc
	3dparty/poppler/poppler/StdinCachedFile.cc
	3dparty/poppler/poppler/StdinPDFDocBuilder.cc
	3dparty/poppler/poppler/Stream.cc
	3dparty/poppler/poppler/TextOutputDev.cc
	3dparty/poppler/poppler/UnicodeMap.cc
	3dparty/poppler/poppler/UnicodeTypeTable.cc
	3dparty/poppler/poppler/UTF.cc
	3dparty/poppler/poppler/ViewerPreferences.cc
	3dparty/poppler/poppler/XpdfPluginAPI.cc
	3dparty/poppler/poppler/XRef.cc
	3dparty/poppler/splash/Splash.cc
	3dparty/poppler/splash/SplashBitmap.cc
	3dparty/poppler/splash/SplashClip.cc
	3dparty/poppler/splash/SplashFont.cc
	3dparty/poppler/splash/SplashFontEngine.cc
	3dparty/poppler/splash/SplashFontFile.cc
	3dparty/poppler/splash/SplashFontFileID.cc
	3dparty/poppler/splash/SplashFTFont.cc
	3dparty/poppler/splash/SplashFTFontEngine.cc
	3dparty/poppler/splash/SplashFTFontFile.cc
	3dparty/poppler/splash/SplashPath.cc
	3dparty/poppler/splash/SplashPattern.cc
	3dparty/poppler/splash/SplashScreen.cc
	3dparty/poppler/splash/SplashState.cc
	3dparty/poppler/splash/SplashT1Font.cc
	3dparty/poppler/splash/SplashT1FontEngine.cc
	3dparty/poppler/splash/SplashT1FontFile.cc
	3dparty/poppler/splash/SplashXPath.cc
	3dparty/poppler/splash/SplashXPathScanner.cc
)

set (TIFF_SOURCES
	3dparty/tiff/tif_aux.c
	3dparty/tiff/tif_close.c
	3dparty/tiff/tif_codec.c
	3dparty/tiff/tif_color.c
	3dparty/tiff/tif_compress.c
	3dparty/tiff/tif_dir.c
	3dparty/tiff/tif_dirinfo.c
	3dparty/tiff/tif_dirread.c
	3dparty/tiff/tif_dirwrite.c
	3dparty/tiff/tif_dumpmode.c
	3dparty/tiff/tif_error.c
	3dparty/tiff/tif_extension.c
	3dparty/tiff/tif_fax3.c
	3dparty/tiff/tif_fax3sm.c
	3dparty/tiff/tif_flush.c
	3dparty/tiff/tif_getimage.c
	3dparty/tiff/tif_jbig.c
	3dparty/tiff/tif_jpeg_12.c
	3dparty/tiff/tif_jpeg.c
	3dparty/tiff/tif_luv.c
	3dparty/tiff/tif_lzma.c
	3dparty/tiff/tif_lzw.c
	3dparty/tiff/tif_next.c
	3dparty/tiff/tif_ojpeg.c
	3dparty/tiff/tif_open.c
	3dparty/tiff/tif_packbits.c
	3dparty/tiff/tif_pixarlog.c
	3dparty/tiff/tif_predict.c
	3dparty/tiff/tif_print.c
	3dparty/tiff/tif_read.c
	3dparty/tiff/tif_stream.cxx
	3dparty/tiff/tif_strip.c
	3dparty/tiff/tif_swab.c
	3dparty/tiff/tif_thunder.c
	3dparty/tiff/tif_tile.c
	3dparty/tiff/tif_unix.c
	3dparty/tiff/tif_version.c
	3dparty/tiff/tif_warning.c
	3dparty/tiff/tif_write.c
	3dparty/tiff/tif_zip.c
)

set (UNIVERSALCHARDET_SOURCES
	3dparty/universalchardet/CharDistribution.cpp
	3dparty/universalchardet/JpCntx.cpp
	3dparty/universalchardet/LangBulgarianModel.cpp
	3dparty/universalchardet/LangCyrillicModel.cpp
	3dparty/universalchardet/LangGreekModel.cpp
	3dparty/universalchardet/LangHebrewModel.cpp
	3dparty/universalchardet/LangHungarianModel.cpp
	3dparty/universalchardet/LangThaiModel.cpp
	3dparty/universalchardet/nsBig5Prober.cpp
	3dparty/universalchardet/nsCharSetProber.cpp
	3dparty/universalchardet/nsEscCharsetProber.cpp
	3dparty/universalchardet/nsEscSM.cpp
	3dparty/universalchardet/nsEUCJPProber.cpp
	3dparty/universalchardet/nsEUCKRProber.cpp
	3dparty/universalchardet/nsEUCTWProber.cpp
	3dparty/universalchardet/nsGB2312Prober.cpp
	3dparty/universalchardet/nsHebrewProber.cpp
	3dparty/universalchardet/nsLatin1Prober.cpp
	3dparty/universalchardet/nsMBCSGroupProber.cpp
	3dparty/universalchardet/nsMBCSSM.cpp
	3dparty/universalchardet/nsSBCharSetProber.cpp
	3dparty/universalchardet/nsSBCSGroupProber.cpp
	3dparty/universalchardet/nsSJISProber.cpp
	3dparty/universalchardet/nsUniversalDetector.cpp
	3dparty/universalchardet/nsUTF8Prober.cpp
)

set (LIBJPEGTURBO_SOURCES
	3dparty/libjpeg-turbo/jaricom.c
	3dparty/libjpeg-turbo/jcapimin.c
	3dparty/libjpeg-turbo/jcapistd.c
	3dparty/libjpeg-turbo/jcarith.c
	3dparty/libjpeg-turbo/jccoefct.c
	3dparty/libjpeg-turbo/jccolor.c
	3dparty/libjpeg-turbo/jcdctmgr.c
	3dparty/libjpeg-turbo/jchuff.c
	3dparty/libjpeg-turbo/jcinit.c
	3dparty/libjpeg-turbo/jcmainct.c
	3dparty/libjpeg-turbo/jcmarker.c
	3dparty/libjpeg-turbo/jcmaster.c
	3dparty/libjpeg-turbo/jcomapi.c
	3dparty/libjpeg-turbo/jcparam.c
	3dparty/libjpeg-turbo/jcphuff.c
	3dparty/libjpeg-turbo/jcprepct.c
	3dparty/libjpeg-turbo/jcsample.c
	3dparty/libjpeg-turbo/jctrans.c
	3dparty/libjpeg-turbo/jdapimin.c
	3dparty/libjpeg-turbo/jdapistd.c
	3dparty/libjpeg-turbo/jdarith.c
	3dparty/libjpeg-turbo/jdatadst.c
	3dparty/libjpeg-turbo/jdatasrc.c
	3dparty/libjpeg-turbo/jdcoefct.c
	3dparty/libjpeg-turbo/jdcolor.c
	3dparty/libjpeg-turbo/jddctmgr.c
	3dparty/libjpeg-turbo/jdhuff.c
	3dparty/libjpeg-turbo/jdinput.c
	3dparty/libjpeg-turbo/jdmainct.c
	3dparty/libjpeg-turbo/jdmarker.c
	3dparty/libjpeg-turbo/jdmaster.c
	3dparty/libjpeg-turbo/jdmerge.c
	3dparty/libjpeg-turbo/jdphuff.c
	3dparty/libjpeg-turbo/jdpostct.c
	3dparty/libjpeg-turbo/jdsample.c
	3dparty/libjpeg-turbo/jdtrans.c
	3dparty/libjpeg-turbo/jerror.c
	3dparty/libjpeg-turbo/jfdctflt.c
	3dparty/libjpeg-turbo/jfdctfst.c
	3dparty/libjpeg-turbo/jfdctint.c
	3dparty/libjpeg-turbo/jidctflt.c
	3dparty/libjpeg-turbo/jidctfst.c
	3dparty/libjpeg-turbo/jidctint.c
	3dparty/libjpeg-turbo/jidctred.c
	3dparty/libjpeg-turbo/jquant1.c
	3dparty/libjpeg-turbo/jquant2.c
	3dparty/libjpeg-turbo/jutils.c
	3dparty/libjpeg-turbo/jmemmgr.c
	3dparty/libjpeg-turbo/jmemnobs.c
)

if (LIBJPEGTURBO_WITH_SIMD)
	if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
		set (SIMD_ARM true)
	elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86")
		if (CMAKE_SIZEOF_VOID_P MATCHES 8)
			set (SIMD_X86_64 true)
		else()
			set (SIMD_I386 true)
		endif()
	endif()

	if (CMAKE_SYSTEM_NAME MATCHES "Windows")
		set (CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -i${CMAKE_SOURCE_DIR}/win/")

		if (SIMD_X86_64)
				set (CMAKE_ASM_NASM_OBJECT_FORMAT win64)
		else()
				set (CMAKE_ASM_NASM_OBJECT_FORMAT win32)
		endif()
	else()
		if (SIMD_X86_64)
				set (CMAKE_ASM_NASM_OBJECT_FORMAT elf64)
		else()
				set (CMAKE_ASM_NASM_OBJECT_FORMAT elf)
		endif()
	endif()

	add_definitions (-DWITH_SIMD)

	if (SIMD_X86_64)
		set (LIBJPEGTURBO_SOURCES ${LIBJPEGTURBO_SOURCES} 3dparty/libjpeg-turbo/simd/jsimd_x86_64.c)

		set (LIBJPEGTURBO_SOURCES ${LIBJPEGTURBO_SOURCES}
			3dparty/libjpeg-turbo/simd/jfsseflt-64.asm
			3dparty/libjpeg-turbo/simd/jccolss2-64.asm
			3dparty/libjpeg-turbo/simd/jdcolss2-64.asm
			3dparty/libjpeg-turbo/simd/jcgrass2-64.asm
			3dparty/libjpeg-turbo/simd/jcsamss2-64.asm
			3dparty/libjpeg-turbo/simd/jdsamss2-64.asm
			3dparty/libjpeg-turbo/simd/jdmerss2-64.asm
			3dparty/libjpeg-turbo/simd/jcqnts2i-64.asm
			3dparty/libjpeg-turbo/simd/jfss2fst-64.asm
			3dparty/libjpeg-turbo/simd/jfss2int-64.asm
			3dparty/libjpeg-turbo/simd/jiss2red-64.asm
			3dparty/libjpeg-turbo/simd/jiss2int-64.asm
			3dparty/libjpeg-turbo/simd/jiss2fst-64.asm
			3dparty/libjpeg-turbo/simd/jcqnts2f-64.asm
			3dparty/libjpeg-turbo/simd/jiss2flt-64.asm
		)

		message (STATUS "Building libjpeg-turbo with x86_64 SIMD acceleration")
	elseif (SIMD_I386)
		set (LIBJPEGTURBO_SOURCES ${LIBJPEGTURBO_SOURCES} 3dparty/libjpeg-turbo/simd/jsimd_i386.c)

		set (LIBJPEGTURBO_SOURCES ${LIBJPEGTURBO_SOURCES}
			3dparty/libjpeg-turbo/simd/jsimdcpu.asm
			3dparty/libjpeg-turbo/simd/jccolmmx.asm
			3dparty/libjpeg-turbo/simd/jcgrammx.asm
			3dparty/libjpeg-turbo/simd/jdcolmmx.asm
			3dparty/libjpeg-turbo/simd/jcsammmx.asm
			3dparty/libjpeg-turbo/simd/jdsammmx.asm
			3dparty/libjpeg-turbo/simd/jdmermmx.asm
			3dparty/libjpeg-turbo/simd/jcqntmmx.asm
			3dparty/libjpeg-turbo/simd/jfmmxfst.asm
			3dparty/libjpeg-turbo/simd/jfmmxint.asm
			3dparty/libjpeg-turbo/simd/jimmxred.asm
			3dparty/libjpeg-turbo/simd/jimmxint.asm
			3dparty/libjpeg-turbo/simd/jimmxfst.asm
			3dparty/libjpeg-turbo/simd/jcqnt3dn.asm
			3dparty/libjpeg-turbo/simd/jf3dnflt.asm
			3dparty/libjpeg-turbo/simd/ji3dnflt.asm
			3dparty/libjpeg-turbo/simd/jcqntsse.asm
			3dparty/libjpeg-turbo/simd/jfsseflt.asm
			3dparty/libjpeg-turbo/simd/jisseflt.asm
			3dparty/libjpeg-turbo/simd/jccolss2.asm
			3dparty/libjpeg-turbo/simd/jcgrass2.asm
			3dparty/libjpeg-turbo/simd/jdcolss2.asm
			3dparty/libjpeg-turbo/simd/jcsamss2.asm
			3dparty/libjpeg-turbo/simd/jdsamss2.asm
			3dparty/libjpeg-turbo/simd/jdmerss2.asm
			3dparty/libjpeg-turbo/simd/jcqnts2i.asm
			3dparty/libjpeg-turbo/simd/jfss2fst.asm
			3dparty/libjpeg-turbo/simd/jfss2int.asm
			3dparty/libjpeg-turbo/simd/jiss2red.asm
			3dparty/libjpeg-turbo/simd/jiss2int.asm
			3dparty/libjpeg-turbo/simd/jiss2fst.asm
			3dparty/libjpeg-turbo/simd/jcqnts2f.asm
			3dparty/libjpeg-turbo/simd/jiss2flt.asm
		)

		message (STATUS "Building libjpeg-turbo with i386 SIMD acceleration")
	elseif (SIMD_ARM)
		set (LIBJPEGTURBO_SOURCES ${LIBJPEGTURBO_SOURCES}
			3dparty/libjpeg-turbo/simd/jsimd_arm.c
			3dparty/libjpeg-turbo/simd/jsimd_arm_neon.S
		)

		message (STATUS "Building libjpeg-turbo with NEON SIMD acceleration")
	endif()

	set (CMAKE_ASM_NASM_FLAGS "-i${CMAKE_SOURCE_DIR}/3dparty/libjpeg-turbo/simd/ -i${CMAKE_BINARY_DIR}/")

	if (NOT CMAKE_SYSTEM_NAME MATCHES "Windows")
		EXECUTE_PROCESS (
			OUTPUT_FILE ${CMAKE_BINARY_DIR}/jsimdcfg.inc
			COMMAND cpp -I${CMAKE_BINARY_DIR}
				-I${CMAKE_BINARY_DIR}/simd
				-I${CMAKE_SOURCE_DIR}/3dparty/libjpeg-turbo/config/${NSR_PLATFORM}
				${CMAKE_SOURCE_DIR}/3dparty/libjpeg-turbo/simd/jsimdcfg.inc.h
			COMMAND egrep "^[\;%]|^\ %"
			COMMAND sed "s%_cpp_protection_%%"
			COMMAND sed "s@% define@%define@g"
		)
	endif()

	if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		set (CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -g")
	endif()
else()
	set (LIBJPEGTURBO_SOURCES ${LIBJPEGTURBO_SOURCES} jsimd_none.c)
	message (STATUS "Building libjpeg-turbo without SIMD acceleration")
endif()

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

set (POPPLER_HEADERS
	3dparty/poppler/config/${NSR_PLATFORM}/config-pdf.h
	3dparty/poppler/fofi/FoFiBase.h
	3dparty/poppler/fofi/FoFiEncodings.h
	3dparty/poppler/fofi/FoFiIdentifier.h
	3dparty/poppler/fofi/FoFiTrueType.h
	3dparty/poppler/fofi/FoFiType1.h
	3dparty/poppler/fofi/FoFiType1C.h
	3dparty/poppler/goo/FixedPoint.h
	3dparty/poppler/goo/gfile.h
	3dparty/poppler/goo/gmem.h
	3dparty/poppler/goo/GooHash.h
	3dparty/poppler/goo/GooLikely.h
	3dparty/poppler/goo/GooList.h
	3dparty/poppler/goo/GooMutex.h
	3dparty/poppler/goo/GooString.h
	3dparty/poppler/goo/GooTimer.h
	3dparty/poppler/goo/grandom.h
	3dparty/poppler/goo/gstrtod.h
	3dparty/poppler/goo/gtypes_p.h
	3dparty/poppler/goo/gtypes.h
	3dparty/poppler/goo/ImgWriter.h
	3dparty/poppler/goo/JpegWriter.h
	3dparty/poppler/goo/PNGWriter.h
	3dparty/poppler/goo/TiffWriter.h
	3dparty/poppler/poppler/Annot.h
	3dparty/poppler/poppler/Array.h
	3dparty/poppler/poppler/BuiltinFont.h
	3dparty/poppler/poppler/BuiltinFontTables.h
	3dparty/poppler/poppler/CachedFile.h
	3dparty/poppler/poppler/Catalog.h
	3dparty/poppler/poppler/CharCodeToUnicode.h
	3dparty/poppler/poppler/CharTypes.h
	3dparty/poppler/poppler/CMap.h
	3dparty/poppler/poppler/CompactFontTables.h
	3dparty/poppler/poppler/CurlCachedFile.h
	3dparty/poppler/poppler/CurlPDFDocBuilder.h
	3dparty/poppler/poppler/DateInfo.h
	3dparty/poppler/poppler/DCTStream.h
	3dparty/poppler/poppler/Decrypt.h
	3dparty/poppler/poppler/Dict.h
	3dparty/poppler/poppler/Error.h
	3dparty/poppler/poppler/ErrorCodes.h
	3dparty/poppler/poppler/FileSpec.h
	3dparty/poppler/poppler/FlateStream.h
	3dparty/poppler/poppler/FontEncodingTables.h
	3dparty/poppler/poppler/FontInfo.h
	3dparty/poppler/poppler/Form.h
	3dparty/poppler/poppler/Function.h
	3dparty/poppler/poppler/Gfx.h
	3dparty/poppler/poppler/GfxFont.h
	3dparty/poppler/poppler/GfxState_helpers.h
	3dparty/poppler/poppler/GfxState.h
	3dparty/poppler/poppler/GlobalParams.h
	3dparty/poppler/poppler/Hints.h
	3dparty/poppler/poppler/JArithmeticDecoder.h
	3dparty/poppler/poppler/JBIG2Stream.h
	3dparty/poppler/poppler/JPXStream.h
	3dparty/poppler/poppler/Lexer.h
	3dparty/poppler/poppler/Linearization.h
	3dparty/poppler/poppler/Link.h
	3dparty/poppler/poppler/LocalPDFDocBuilder.h
	3dparty/poppler/poppler/Movie.h
	3dparty/poppler/poppler/NameToCharCode.h
	3dparty/poppler/poppler/NameToUnicodeTable.h
	3dparty/poppler/poppler/Object.h
	3dparty/poppler/poppler/OptionalContent.h
	3dparty/poppler/poppler/Outline.h
	3dparty/poppler/poppler/OutputDev.h
	3dparty/poppler/poppler/Page.h
	3dparty/poppler/poppler/PageLabelInfo.h
	3dparty/poppler/poppler/PageTransition.h
	3dparty/poppler/poppler/Parser.h
	3dparty/poppler/poppler/PDFDoc.h
	3dparty/poppler/poppler/PDFDocBuilder.h
	3dparty/poppler/poppler/PDFDocEncoding.h
	3dparty/poppler/poppler/PDFDocFactory.h
	3dparty/poppler/poppler/poppler-config.h
	3dparty/poppler/poppler/PopplerCache.h
	3dparty/poppler/poppler/PreScanOutputDev.h
	3dparty/poppler/poppler/ProfileData.h
	3dparty/poppler/poppler/PSOutputDev.h
	3dparty/poppler/poppler/PSTokenizer.h
	3dparty/poppler/poppler/Rendition.h
	3dparty/poppler/poppler/SecurityHandler.h
	3dparty/poppler/poppler/Sound.h
	3dparty/poppler/poppler/SplashOutputDev.h
	3dparty/poppler/poppler/StdinCachedFile.h
	3dparty/poppler/poppler/StdinPDFDocBuilder.h
	3dparty/poppler/poppler/Stream-CCITT.h
	3dparty/poppler/poppler/Stream.h
	3dparty/poppler/poppler/TextOutputDev.h
	3dparty/poppler/poppler/UnicodeCClassTables.h
	3dparty/poppler/poppler/UnicodeCompTables.h
	3dparty/poppler/poppler/UnicodeDecompTables.h
	3dparty/poppler/poppler/UnicodeMap.h
	3dparty/poppler/poppler/UnicodeMapTables.h
	3dparty/poppler/poppler/UnicodeTypeTable.h
	3dparty/poppler/poppler/UTF.h
	3dparty/poppler/poppler/UTF8.h
	3dparty/poppler/poppler/ViewerPreferences.h
	3dparty/poppler/poppler/XpdfPluginAPI.h
	3dparty/poppler/poppler/XRef.h
	3dparty/poppler/splash/Splash.h
	3dparty/poppler/splash/SplashBitmap.h
	3dparty/poppler/splash/SplashClip.h
	3dparty/poppler/splash/SplashErrorCodes.h
	3dparty/poppler/splash/SplashFont.h
	3dparty/poppler/splash/SplashFontEngine.h
	3dparty/poppler/splash/SplashFontFile.h
	3dparty/poppler/splash/SplashFontFileID.h
	3dparty/poppler/splash/SplashFTFont.h
	3dparty/poppler/splash/SplashFTFontEngine.h
	3dparty/poppler/splash/SplashFTFontFile.h
	3dparty/poppler/splash/SplashGlyphBitmap.h
	3dparty/poppler/splash/SplashMath.h
	3dparty/poppler/splash/SplashPath.h
	3dparty/poppler/splash/SplashPattern.h
	3dparty/poppler/splash/SplashScreen.h
	3dparty/poppler/splash/SplashState.h
	3dparty/poppler/splash/SplashT1Font.h
	3dparty/poppler/splash/SplashT1FontEngine.h
	3dparty/poppler/splash/SplashT1FontFile.h
	3dparty/poppler/splash/SplashTypes.h
	3dparty/poppler/splash/SplashXPath.h
	3dparty/poppler/splash/SplashXPathScanner.h
)

set (TIFF_HEADERS
	3dparty/tiff/config/${NSR_PLATFORM}/tif_config.h
	3dparty/tiff/config/${NSR_PLATFORM}/tiffconf.h
	3dparty/tiff/t4.h
	3dparty/tiff/tif_dir.h
	3dparty/tiff/tif_fax3.h
	3dparty/tiff/tif_predict.h
	3dparty/tiff/tiff.h
	3dparty/tiff/tiffio.h
	3dparty/tiff/tiffiop.h
	3dparty/tiff/tiffvers.h
	3dparty/tiff/uvcode.h
)

set (UNIVERSALCHARDET_HEADERS
	3dparty/universalchardet/CharDistribution.h
	3dparty/universalchardet/JpCntx.h
	3dparty/universalchardet/nsBig5Prober.h
	3dparty/universalchardet/nsCharSetProber.h
	3dparty/universalchardet/nsCodingStateMachine.h
	3dparty/universalchardet/nsEscCharsetProber.h
	3dparty/universalchardet/nsEUCJPProber.h
	3dparty/universalchardet/nsEUCKRProber.h
	3dparty/universalchardet/nsEUCTWProber.h
	3dparty/universalchardet/nsGB2312Prober.h
	3dparty/universalchardet/nsHebrewProber.h
	3dparty/universalchardet/nsLatin1Prober.h
	3dparty/universalchardet/nsMBCSGroupProber.h
	3dparty/universalchardet/nsPkgInt.h
	3dparty/universalchardet/nsSBCharSetProber.h
	3dparty/universalchardet/nsSBCSGroupProber.h
	3dparty/universalchardet/nsSJISProber.h
	3dparty/universalchardet/nsUniversalDetector.h
	3dparty/universalchardet/nsUTF8Prober.h
)

set (LIBJPEGTURBO_HEADERS
	3dparty/libjpeg-turbo/config/${NSR_PLATFORM}/config-jpeg.h
	3dparty/libjpeg-turbo/config/${NSR_PLATFORM}/jconfig.h
	3dparty/libjpeg-turbo/jchuff.h
	3dparty/libjpeg-turbo/jdct.h
	3dparty/libjpeg-turbo/jdhuff.h
	3dparty/libjpeg-turbo/jerror.h
	3dparty/libjpeg-turbo/jinclude.h
	3dparty/libjpeg-turbo/jmemsys.h
	3dparty/libjpeg-turbo/jmorecfg.h
	3dparty/libjpeg-turbo/jpegcomp.h
	3dparty/libjpeg-turbo/jpegint.h
	3dparty/libjpeg-turbo/jpeglib.h
	3dparty/libjpeg-turbo/jsimd.h
	3dparty/libjpeg-turbo/jsimddct.h
	3dparty/libjpeg-turbo/jversion.h
)

include_directories (
	3dparty
	3dparty/poppler/config/${NSR_PLATFORM}
	3dparty/tiff/config/${NSR_PLATFORM}
	3dparty/libjpeg-turbo/config/${NSR_PLATFORM}
	3dparty/tiff
	3dparty/poppler
	3dparty/poppler/goo
	3dparty/poppler/poppler
	3dparty/universalchardet
	${DJVU_INCLUDES}
	${CMAKE_BINARY_DIR}
	${PROJECT_SOURCE_DIR}
	${NSR_INCLUDE_DIRECTORIES}
)

add_library (${PROJECT_NAME} SHARED
	${SOURCES}
	${THIRDPARTY_SOURCES}
	${POPPLER_SOURCES}
	${TIFF_SOURCES}
	${UNIVERSALCHARDET_SOURCES}
	${LIBJPEGTURBO_SOURCES}
	${HEADERS}
	${THIRDPARTY_HEADERS}
	${POPPLER_HEADERS}
	${TIFF_HEADERS}
	${UNIVERSALCHARDET_HEADERS}
	${LIBJPEGTURBO_HEADERS}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions (${NSR_PLATFORM_DEFINES_DEBUG})
else()
	add_definitions (${NSR_PLATFORM_DEFINES_RELEASE})
endif()

set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${NSR_C_FLAGS_DEBUG}")
set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${NSR_C_FLAGS_RELEASE}")
set (CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} ${NSR_C_FLAGS_MINSIZEREL}")
set (CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${NSR_C_FLAGS_RELWITHDEBINFO}")

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${NSR_CXX_FLAGS_DEBUG}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${NSR_CXX_FLAGS_RELEASE}")
set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${NSR_CXX_FLAGS_MINSIZEREL}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${NSR_CXX_FLAGS_RELWITHDEBINFO}")

target_link_libraries (${PROJECT_NAME}
	${QT_LIBRARIES}
	${DJVU_LIBRARIES}
	png
	jpeg
	curl
	iconv
	fontconfig
	freetype
	z
	bb
)

add_definitions (-DHAVE_CONFIG_H -DNSRREADERCORE_LIBRARY)
