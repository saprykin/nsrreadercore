include (CheckIncludeFiles)
include (CheckTypeSize)
include (CheckFunctionExists)
include (CheckCSourceCompiles)
include (CheckCInline)
include (CheckNASMFlags)

string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

option (LIBJPEGTURBO_WITH_SIMD "Use SIMD instructions for speed up if available" true)
option (LIBJPEGTURBO_WITH_ARITH_ENC "Use arithmetic encoding support" true)
option (LIBJPEGTURBO_WITH_ARITH_DEC "Use arithmetic decoding support" true)

set (JPEG_LIB_VERSION "62")
set (LIBJPEGTURBO_VERSION "1.3.1")

if (LIBJPEGTURBO_WITH_ARITH_ENC)
	set (C_ARITH_CODING_SUPPORTED true)
endif()

if (LIBJPEGTURBO_WITH_ARITH_DEC)
	set (D_ARITH_CODING_SUPPORTED true)
endif()

set (MEM_SRCDST_SUPPORTED		true)
set (HAVE_PROTOTYPES			true)

check_include_files (stddef.h		HAVE_STDDEF_H)
check_include_files (stdlib.h		HAVE_STDLIB_H)
check_include_files (sys/types.h	NEED_SYS_TYPES_H)

check_type_size ("unsigned char"	SIZEOF_UNSIGNED_CHAR)
check_type_size ("unsigned short"	SIZEOF_UNSIGNED_SHORT)
check_type_size ("size_t"		SIZEOF_SIZE_T)
check_type_size ("unsigned long"	SIZEOF_UNSIGNED_LONG)
check_type_size ("unsigned long long"	SIZEOF_UNSIGNED_LONG_LONG)
check_type_size ("unsigned char *"	SIZEOF_UNSIGNED_CHAR_P)

set (HAVE_UNSIGNED_CHAR ${HAVE_SIZEOF_UNSIGNED_CHAR})
set (HAVE_UNSIGNED_SHORT ${HAVE_SIZEOF_UNSIGNED_SHORT})

check_function_exists (memset HAVE_MEMSET)
check_function_exists (memcpy HAVE_MEMCPY)

if (HAVE_MEMSET AND HAVE_MEMCPY)
	set (NEED_BSD_STRINGS true)
endif()

set (CMAKE_REQUIRED_FLAGS ${NSR_C_FLAGS_CHECK})

check_c_source_compiles (
	"typedef struct undefined_structure * undef_struct_ptr;
	int main () {
		return 0;
	}"
	HAVE_INCOMPLETE_TYPES
)

if (NOT HAVE_INCOMPLETE_TYPES)
	set (INCOMPLETE_TYPES_BROKEN true)
endif()

check_c_source_compiles (
	"int possibly_duplicate_function () { return 0; }
	int possibly_dupli_function () { return 1; }
	int main () {
		return 0;
	}"
	HAVE_LONG_NAMES
)

if (NOT HAVE_LONG_NAMES)
	set (NEED_SHORT_EXTERNAL_NAMES true)
endif()

NSR_CHECK_C_INLINE (C_INLINE)

if (NOT C_INLINE STREQUAL "inline")
	set (HAVE_CUSTOM_INLINE true)
	set (JPEG_C_INLINE ${C_INLINE})
endif ()

check_c_source_compiles (
	"__forceinline int foo() { return 0; }
	int main () {
		return foo ();
	}"
	HAVE_INLINE_FORCE
)

check_c_source_compiles (
	"inline __attribute__((always_inline)) int foo() { return 0; }
	int main () {
		return foo ();
	}"
	HAVE_INLINE_ALWAYS
)

check_c_source_compiles (
	"__inline__ int foo() { return 0; }
	int main () {
		return foo ();
	}"
	HAVE_INLINE_UNDER_2
)

check_c_source_compiles (
	"__inline int foo() { return 0; }
	int main () {
		return foo ();
	}"
	HAVE_INLINE_UNDER_1
)

check_c_source_compiles (
	"inline int foo() { return 0; }
	int main () {
		return foo ();
	}"
	HAVE_INLINE_SIMPLE
)

if (HAVE_INLINE_FORCE)
	set (JPEG_C_INLINE_FORCE "__forceinline")
elseif (HAVE_INLINE_ALWAYS)
	set (JPEG_C_INLINE_FORCE "inline __attribute__((always_inline))")
elseif (HAVE_INLINE_UNDER_2)
	set (JPEG_C_INLINE_FORCE "__inline__")
elseif (HAVE_INLINE_UNDER_1)
	set (JPEG_C_INLINE_FORCE "__inline")
elseif (HAVE_INLINE_SIMPLE)
	set (JPEG_C_INLINE_FORCE "inline")
endif()

if (JPEG_C_INLINE_FORCE)
	message (STATUS "Using function forcing inline: ${JPEG_C_INLINE_FORCE}")
endif()

set (CMAKE_REQUIRED_FLAGS "")

if (NOT HAVE_SIZEOF_SIZE_T)
	if (SIZEOF_UNSIGNED_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
		set (JPEG_SIZE_T "unsigned long")
	elseif (SIZEOF_UNSIGNED_LONG_LONG EQUAL SIZEOF_UNSIGNED_CHAR_P)
		set (JPEG_SIZE_T "unsigned long long")
	else()
		set (JPEG_SIZE_T "unsigned int")
	endif()

	set (HAVE_CUSTOM_SIZE_T true)
endif()

if (LIBJPEGTURBO_WITH_SIMD)
	if (CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
		if (DEFINED NSR_PLATFORM_NEON_ENABLED)
			set (SIMD_ARM ${NSR_PLATFORM_NEON_ENABLED})
		else()
			set (SIMD_ARM true)
		endif()

		set (WITH_SIMD ${SIMD_ARM})
	elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "x86" OR
		CMAKE_SYSTEM_PROCESSOR MATCHES "i.86")
		if (SIZEOF_UNSIGNED_CHAR_P MATCHES 8)
			set (SIMD_X86_64 true)
		else()
			set (SIMD_I386 true)
		endif()

		set (WITH_SIMD true)
	endif()
endif()

if (SIMD_I386 OR SIMD_X86_64)
	enable_language (ASM_NASM)

	if (NOT CMAKE_ASM_NASM_COMPILER_WORKS)
		set (WITH_SIMD false)
	endif()
endif()

set (RIGHT_SHIFT_IS_UNSIGNED ${NSR_RIGHT_SHIFT_IS_UNSIGNED})

if (SYSTEM_NAME MATCHES "windows")
	configure_file (win/config.h.in ${CMAKE_BINARY_DIR}/config-jpeg.h)
	configure_file (win/jconfig.h.in ${CMAKE_BINARY_DIR}/jconfig.h)
else()
	configure_file (config.h.in ${CMAKE_BINARY_DIR}/config-jpeg.h)
	configure_file (jconfig.h.in ${CMAKE_BINARY_DIR}/jconfig.h)
endif()

set (SOURCES
	jaricom.c
	jcapimin.c
	jcapistd.c
	jcarith.c
	jccoefct.c
	jccolor.c
	jcdctmgr.c
	jchuff.c
	jcinit.c
	jcmainct.c
	jcmarker.c
	jcmaster.c
	jcomapi.c
	jcparam.c
	jcphuff.c
	jcprepct.c
	jcsample.c
	jctrans.c
	jdapimin.c
	jdapistd.c
	jdarith.c
	jdatadst.c
	jdatasrc.c
	jdcoefct.c
	jdcolor.c
	jddctmgr.c
	jdhuff.c
	jdinput.c
	jdmainct.c
	jdmarker.c
	jdmaster.c
	jdmerge.c
	jdphuff.c
	jdpostct.c
	jdsample.c
	jdtrans.c
	jerror.c
	jfdctflt.c
	jfdctfst.c
	jfdctint.c
	jidctflt.c
	jidctfst.c
	jidctint.c
	jidctred.c
	jquant1.c
	jquant2.c
	jutils.c
	jmemmgr.c
	jmemnobs.c
)

set (HEADERS
	jchuff.h
	jdct.h
	jdhuff.h
	jerror.h
	jinclude.h
	jmemsys.h
	jmorecfg.h
	jpegcomp.h
	jpegint.h
	jpeglib.h
	jsimd.h
	jsimddct.h
	jversion.h
)

if (WITH_SIMD)
	if (SIMD_X86_64)
		set (SOURCES ${SOURCES} simd/jsimd_x86_64.c)

		set (SOURCES ${SOURCES}
			simd/jfsseflt-64.asm
			simd/jccolss2-64.asm
			simd/jdcolss2-64.asm
			simd/jcgrass2-64.asm
			simd/jcsamss2-64.asm
			simd/jdsamss2-64.asm
			simd/jdmerss2-64.asm
			simd/jcqnts2i-64.asm
			simd/jfss2fst-64.asm
			simd/jfss2int-64.asm
			simd/jiss2red-64.asm
			simd/jiss2int-64.asm
			simd/jiss2fst-64.asm
			simd/jcqnts2f-64.asm
			simd/jiss2flt-64.asm
		)

		message (STATUS "Building libjpeg-turbo with x86_64 SIMD acceleration")
	elseif (SIMD_I386)
		set (SOURCES ${SOURCES} simd/jsimd_i386.c)

		set (SOURCES ${SOURCES}
			simd/jsimdcpu.asm
			simd/jccolmmx.asm
			simd/jcgrammx.asm
			simd/jdcolmmx.asm
			simd/jcsammmx.asm
			simd/jdsammmx.asm
			simd/jdmermmx.asm
			simd/jcqntmmx.asm
			simd/jfmmxfst.asm
			simd/jfmmxint.asm
			simd/jimmxred.asm
			simd/jimmxint.asm
			simd/jimmxfst.asm
			simd/jcqnt3dn.asm
			simd/jf3dnflt.asm
			simd/ji3dnflt.asm
			simd/jcqntsse.asm
			simd/jfsseflt.asm
			simd/jisseflt.asm
			simd/jccolss2.asm
			simd/jcgrass2.asm
			simd/jdcolss2.asm
			simd/jcsamss2.asm
			simd/jdsamss2.asm
			simd/jdmerss2.asm
			simd/jcqnts2i.asm
			simd/jfss2fst.asm
			simd/jfss2int.asm
			simd/jiss2red.asm
			simd/jiss2int.asm
			simd/jiss2fst.asm
			simd/jcqnts2f.asm
			simd/jiss2flt.asm
		)

		message (STATUS "Building libjpeg-turbo with i386 SIMD acceleration")
	elseif (SIMD_ARM)
		set (SOURCES ${SOURCES}
			simd/jsimd_arm.c
			simd/jsimd_arm_neon.S
		)

		set_source_files_properties (simd/jsimd_arm_neon.S PROPERTIES LANGUAGE C)

		message (STATUS "Building libjpeg-turbo with NEON SIMD acceleration")
	endif()

	if (NOT SIMD_ARM)
		set (CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -I${CMAKE_CURRENT_SOURCE_DIR}/simd/ -I${CMAKE_BINARY_DIR}/")

		if (NOT SYSTEM_NAME MATCHES "windows")
			EXECUTE_PROCESS (
				OUTPUT_FILE ${CMAKE_BINARY_DIR}/jsimdcfg.inc
				COMMAND cpp -I${CMAKE_BINARY_DIR}
					-I${CMAKE_BINARY_DIR}/simd
					${CMAKE_CURRENT_SOURCE_DIR}/simd/jsimdcfg.inc.h
				COMMAND egrep "^[\;%]|^\ %"
				COMMAND sed "s%_cpp_protection_%%"
				COMMAND sed "s@% define@%define@g"
			)
		endif()

		if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
			set (CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} -g")
		endif()

		NSR_CHECK_NASM_FLAGS (NAFLAGS NAOBJFORMAT)

		set (CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} ${NAFLAGS} -DPIC")
		set (CMAKE_ASM_NASM_OBJECT_FORMAT ${NAOBJFORMAT})
	endif()

	add_definitions ("-DWITH_SIMD")
endif()

if (NOT WITH_SIMD)
	set (SOURCES ${SOURCES} jsimd_none.c)
	message (STATUS "Building libjpeg-turbo without SIMD acceleration")
endif()

set (HEADERS ${HEADERS} simd/jsimd.h)

include_directories (BEFORE ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
add_library (jpegturbo OBJECT ${SOURCES} ${HEADERS})
set_target_properties (jpegturbo PROPERTIES POSITION_INDEPENDENT_CODE true)

set (LIBJPEGTURBO_DEFINITIONS "${LIBJPEGTURBO_DEFINITIONS}" PARENT_SCOPE)
set (LIBJPEGTURBO_INCLUDES ${LIBJPEGTURBO_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set (LIBJPEGTURBO_LIBRARIES ${LIBJPEGTURBO_LIBRARIES} PARENT_SCOPE)
