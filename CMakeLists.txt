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
add_subdirectory (3dparty/poppler)
add_subdirectory (3dparty/tiff)

foreach (DJVU_FILE ${DJVU_SOURCES})
	set (DJVU_FILE "3dparty/djvu/${DJVU_FILE}")
	set (THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${DJVU_FILE})
endforeach()

foreach (DJVU_FILE ${DJVU_HEADERS})
	set (DJVU_FILE "3dparty/djvu/${DJVU_FILE}")
	set (THIRDPARTY_HEADERS ${THIRDPARTY_HEADERS} ${DJVU_FILE})
endforeach()

foreach (POPPLER_FILE ${POPPLER_SOURCES})
	set (POPPLER_FILE "3dparty/poppler/${POPPLER_FILE}")
	set (THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${POPPLER_FILE})
endforeach()

foreach (POPPLER_FILE ${POPPLER_HEADERS})
	set (POPPLER_FILE "3dparty/poppler/${POPPLER_FILE}")
	set (THIRDPARTY_HEADERS ${THIRDPARTY_HEADERS} ${POPPLER_FILE})
endforeach()

foreach (TIFF_FILE ${TIFF_SOURCES})
	set (TIFF_FILE "3dparty/tiff/${TIFF_FILE}")
	set (THIRDPARTY_SOURCES ${THIRDPARTY_SOURCES} ${TIFF_FILE})
endforeach()

foreach (TIFF_FILE ${TIFF_HEADERS})
	set (TIFF_FILE "3dparty/tiff/${TIFF_FILE}")
	set (THIRDPARTY_HEADERS ${THIRDPARTY_HEADERS} ${TIFF_FILE})
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
	3dparty/libjpeg-turbo/config/${NSR_PLATFORM}
	3dparty/universalchardet
	${DJVU_INCLUDES}
	${POPPLER_INCLUDES}
	${TIFF_INCLUDES}
	${CMAKE_BINARY_DIR}
	${PROJECT_SOURCE_DIR}
	${NSR_INCLUDE_DIRECTORIES}
)

add_library (${PROJECT_NAME} SHARED
	${SOURCES}
	${THIRDPARTY_SOURCES}
	${UNIVERSALCHARDET_SOURCES}
	${LIBJPEGTURBO_SOURCES}
	${HEADERS}
	${THIRDPARTY_HEADERS}
	${UNIVERSALCHARDET_HEADERS}
	${LIBJPEGTURBO_HEADERS}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_definitions (${NSR_PLATFORM_DEFINES_DEBUG})
else()
	add_definitions (${NSR_PLATFORM_DEFINES_RELEASE})
endif()

add_definitions ("${DJVU_DEFINITIONS} ${POPPLER_DEFINITIONS} ${TIFF_DEFINITIONS}")

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
	${NSR_LIBRARIES}
	${DJVU_LIBRARIES}
	${POPPLER_LIBRARIES}
	${TIFF_LIBRARIES}
	jpeg
)

add_definitions (-DNSRREADERCORE_LIBRARY)
