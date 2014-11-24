include (CheckIncludeFiles)
include (CheckFunctionExists)
include (CheckTypeSize)
include (CheckCSourceCompiles)
include (CheckCXXSourceCompiles)
include (TestLargeFiles)
include (CheckDIRSymbolExists)

set (DJVU_VERSION 3.5.25)

check_include_files (inttypes.h			HAVE_INTTYPES_H)
check_include_files (stdint.h			HAVE_STDINT_H)
check_include_files (stdlib.h			HAVE_STDLIB_H)
check_include_files (sys/ipc.h			HAVE_SYS_IPC_H)
check_include_files (sys/mman.h			HAVE_SYS_MMAN_H)
check_include_files (sys/shm.h			HAVE_SYS_SHM_H)
check_include_files (unistd.h			HAVE_UNISTD_H)
check_include_files (wchar.h			HAVE_WCHAR_H)
check_include_files (wctype.h			HAVE_WCTYPE_H)

set (CMAKE_REQUIRED_FLAGS ${NSR_C_FLAGS_CHECK})

check_function_exists (getpwuid			HAVE_GETPWUID)
check_function_exists (iswspace 		HAVE_ISWSPACE)
check_function_exists (mmap 			HAVE_MMAP)
check_function_exists (strerror 		HAVE_STRERROR)
check_function_exists (vsnprintf 		HAVE_VSNPRINTF)
check_function_exists (wcrtomb 			HAVE_WCRTOMB)

find_package (Iconv)

if (Iconv_FOUND)
	set (HAVE_ICONV_H true)
	set (HAVE_ICONV true)
	set (DJVU_LIBRARIES ${DJVU_LIBRARIES} ${Iconv_LIBRARIES})
	set (DJVU_INCLUDES ${DJVU_INCLUDES} ${Iconv_INCLUDE_DIR})
endif()

set (CMAKE_REQUIRED_INCLUDES "")
set (CMAKE_REQUIRED_LIBRARIES "")

set (CMAKE_REQUIRED_FLAGS ${NSR_CXX_FLAGS_CHECK})

check_type_size (bool			DJVU_SIZEOF_BOOL BUILTIN_TYPES_ONLY LANGUAGE CXX)
check_type_size ("long long int"	DJVU_SIZEOF_LONG_LONG_INT LANGUAGE CXX)
check_type_size (wchar_t		DJVU_SIZEOF_WCHAR_T LANGUAGE CXX)

if (HAVE_DJVU_SIZEOF_BOOL)
	set (HAVE_BOOL true)
endif()

if (HAVE_DJVU_SIZEOF_LONG_LONG_INT)
	set (HAVE_LONG_LONG_INT true)
endif()

if (HAVE_DJVU_SIZEOF_WCHAR_T)
	set (HAVE_WCHAR_T true)
endif()

set (CMAKE_REQUIRED_INCLUDES "wchar.h")
check_type_size (mbstate_t DJVU_SIZEOF_MBSTATE_T LANGUAGE CXX)

if (HAVE_DJVU_SIZEOF_MBSTATE_T)
	set (HAVE_MBSTATE_T true)
endif()

set (CMAKE_REQUIRED_FLAGS ${NSR_C_FLAGS_CHECK})

NSR_TEST_LARGE_FILES (DJVU_HAVE_LARGEFILES)
NSR_CHECK_DIRSYMBOL_EXISTS ("dirent.h"		HAVE_DIRENT_H)
NSR_CHECK_DIRSYMBOL_EXISTS ("ndir.h" 		HAVE_NDIR_H)
NSR_CHECK_DIRSYMBOL_EXISTS ("sys/ndir.h"	HAVE_SYS_NDIR_H)
NSR_CHECK_DIRSYMBOL_EXISTS ("sys/dir.h"		HAVE_SYS_DIR_H)

set (CMAKE_REQUIRED_FLAGS ${NSR_CXX_FLAGS_CHECK})

check_cxx_source_compiles (
	"int main () {
		try { throw  1; } catch (int i) { return i; }
	}"
	HAVE_EXCEPTIONS
)

check_cxx_source_compiles (
	"template<typename T>class X {public: X () {}};
	int main () {
		X<float> z;
		return 0;
	}"
	HAVE_TYPENAME
)

check_cxx_source_compiles (
	"template<class T, int N> class A {
		public:
		template<int N2> A<T, N> operator= (const A<T, N2>& z) {
			return A<T, N> ();
		}
	};
	int main () {
		A<double, 4> x;
		A<double, 7> y;
		x = y;
		return 0;
	}"
	HAVE_MEMBER_TEMPLATES
)

check_cxx_source_compiles (
	"namespace Outer { namespace Inner {
		int i = 0;
	}}

	using namespace Outer::Inner;

	int main () {
		return i;
	}"
	HAVE_NAMESPACES
)

check_cxx_source_compiles (
	"#include <new>
	struct X {
		int a;
		X (int a) : a (a) {};
	};
	X* foo (void *x) {
		return new (x) X(2);
	}
	int main () {
		return 0;
	}"
	HAVE_STDINCLUDES
)

set (CMAKE_REQUIRED_FLAGS ${NSR_C_FLAGS_CHECK})

check_c_source_compiles (
	"static int volatile l;
	int main () {
		__sync_lock_test_and_set (&l, 1);
		__sync_lock_release (&l);
		__sync_add_and_fetch (&l, 1);
		__sync_bool_compare_and_swap (&l, &l, 1);
		return 0;
	}"
	HAVE_INTEL_ATOMIC_BUILTINS
)

check_c_source_compiles (
	"#include <sys/time.h>
	#include <time.h>
	int main () {
		return 0;
	}"
	TIME_WITH_SYS_TIME
)

set (CMAKE_REQUIRED_FLAGS "")

find_package (Threads)

if (Threads_FOUND)
	if (CMAKE_USE_PTHREADS_INIT)
		set (HAVE_PTHREAD true)
	endif()

	set (DJVU_LIBRARIES ${DJVU_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
endif()

configure_file (config-djvu.h.in ${CMAKE_BINARY_DIR}/config-djvu.h)

set (DJVU_SOURCES
	Arrays.cpp
	atomic.cpp
	BSByteStream.cpp
	BSEncodeByteStream.cpp
	ByteStream.cpp
	DataPool.cpp
	ddjvuapi.cpp
	debug.cpp
	DjVmDir.cpp
	DjVmDir0.cpp
	DjVmDoc.cpp
	DjVmNav.cpp
	DjVuAnno.cpp
	DjVuDocEditor.cpp
	DjVuDocument.cpp
	DjVuDumpHelper.cpp
	DjVuErrorList.cpp
	DjVuFile.cpp
	DjVuFileCache.cpp
	DjVuGlobal.cpp
	DjVuGlobalMemory.cpp
	DjVuImage.cpp
	DjVuInfo.cpp
	DjVuMessage.cpp
	DjVuMessageLite.cpp
	DjVuNavDir.cpp
	DjVuPalette.cpp
	DjVuPort.cpp
	DjVuText.cpp
	DjVuToPS.cpp
	GBitmap.cpp
	GContainer.cpp
	GException.cpp
	GIFFManager.cpp
	GMapAreas.cpp
	GOS.cpp
	GPixmap.cpp
	GRect.cpp
	GScaler.cpp
	GSmartPointer.cpp
	GString.cpp
	GThreads.cpp
	GUnicode.cpp
	GURL.cpp
	IFFByteStream.cpp
	IW44EncodeCodec.cpp
	IW44Image.cpp
	JB2EncodeCodec.cpp
	JB2Image.cpp
	JPEGDecoder.cpp
	miniexp.cpp
	MMRDecoder.cpp
	MMX.cpp
	UnicodeByteStream.cpp
	XMLParser.cpp
	XMLTags.cpp
	ZPCodec.cpp
)

set (DJVU_HEADERS
	Arrays.h
	atomic.h
	BSByteStream.h
	ByteStream.h
	DataPool.h
	ddjvuapi.h
	debug.h
	DjVmDir.h
	DjVmDir0.h
	DjVmDoc.h
	DjVmNav.h
	DjVuAnno.h
	DjVuDocEditor.h
	DjVuDocument.h
	DjVuDumpHelper.h
	DjVuErrorList.h
	DjVuFile.h
	DjVuFileCache.h
	DjVuGlobal.h
	DjVuImage.h
	DjVuInfo.h
	DjVuMessage.h
	DjVuMessageLite.h
	DjVuNavDir.h
	DjVuPalette.h
	DjVuPort.h
	DjVuText.h
	DjVuToPS.h
	GBitmap.h
	GContainer.h
	GException.h
	GIFFManager.h
	GMapAreas.h
	GOS.h
	GPixmap.h
	GRect.h
	GScaler.h
	GSmartPointer.h
	GString.h
	GThreads.h
	GURL.h
	IFFByteStream.h
	IW44Image.h
	JB2Image.h
	JPEGDecoder.h
	miniexp.h
	MMRDecoder.h
	MMX.h
	Template.h
	UnicodeByteStream.h
	XMLParser.h
	XMLTags.h
	ZPCodec.h
)

set (DJVU_DEFINITIONS "${DJVU_DEFINITIONS} -DHAVE_CONFIG_H" PARENT_SCOPE)
set (DJVU_INCLUDES ${DJVU_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
set (DJVU_SOURCES ${DJVU_SOURCES} PARENT_SCOPE)
set (DJVU_HEADERS ${DJVU_HEADERS} PARENT_SCOPE)
set (DJVU_LIBRARIES ${DJVU_LIBRARIES} PARENT_SCOPE)
