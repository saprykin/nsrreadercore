set (CMAKE_SYSTEM_NAME QNX)
set (CMAKE_SYSTEM_VERSION 8.0.0)
set (TOOLCHAIN QNX)

set (CPUVARDIR $ENV{CPUVARDIR})

if (${CPUVARDIR} STREQUAL "armle-v7")
	set (CMAKE_SYSTEM_PROCESSOR armv7)
elseif (${CPUVARDIR} STREQUAL "x86")
	set (CMAKE_SYSTEM_PROCESSOR x86)
else()
	message (CRITICAL "Unsupported CPU architecture: ${CPUVARDIR}")
endif()

set (QNX_HOST $ENV{QNX_HOST})
set (QNX_TARGET $ENV{QNX_TARGET})

if (NOT QNX_HOST)
	message (CRITICAL "You must define QNX_HOST environment variable for toolchain")
endif()

if (NOT QNX_TARGET)
	message (CRITICAL "You must define QNX_TARGET environment variable for toolchain")
endif()

set (CMAKE_SHARED_LIBRARY_PREFIX "lib")
set (CMAKE_SHARED_LIBRARY_SUFFIX ".so")
set (CMAKE_STATIC_LIBRARY_PREFIX "lib")
set (CMAKE_STATIC_LIBRARY_SUFFIX ".a")

if (CMAKE_HOST_WIN32)
  set (HOST_EXECUTABLE_SUFFIX ".exe")
endif()

set (CMAKE_MAKE_PROGRAM
	"${QNX_HOST}/usr/bin/make${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX Make Program")

set (CMAKE_SH
	"${QNX_HOST}/usr/bin/sh${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX shell Program")

set (CMAKE_AR
	"${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ar${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX ar Program")

set (CMAKE_RANLIB
	"${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ranlib${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX ranlib Program")

set (CMAKE_NM
	"${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-nm${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX nm Program")

set (CMAKE_OBJCOPY
	"${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-objcopy${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX objcopy Program")

set (CMAKE_OBJDUMP
	"${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-objdump${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX objdump Program")

set (CMAKE_LINKER
	"${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-ld"
	CACHE PATH "QNX Linker Program")

set (CMAKE_STRIP
	"${QNX_HOST}/usr/bin/nto${CMAKE_SYSTEM_PROCESSOR}-strip${HOST_EXECUTABLE_SUFFIX}"
	CACHE PATH "QNX Strip Program")

set (CMAKE_C_COMPILER ${QNX_HOST}/usr/bin/qcc${HOST_EXECUTABLE_SUFFIX})
set (CMAKE_CXX_COMPILER ${QNX_HOST}/usr/bin/qcc${HOST_EXECUTABLE_SUFFIX})

set (NSR_ARM_FLAGS "-mcpu=cortex-a9 -mthumb")
set (NSR_BASE_FLAGS "-D_REENTRANT -Wno-psabi -fstack-protector -fstack-protector-all")

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7")
	set (NSR_CPU_FLAGS ${NSR_ARM_FLAGS})
endif()

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "armv7")
	set (NSR_QCC_FLAGS "-Vgcc_ntoarmv7le")
elseif (CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
	set (NSR_QCC_FLAGS "-Vgcc_ntox86")
endif()

set (NSR_C_FLAGS_DEBUG "${NSR_QCC_FLAGS} ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -g")
set (NSR_C_FLAGS_MINSIZEREL "${NSR_QCC_FLAGS} ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -O2 -fstack-protector-strong -fvisibility=hidden -Os")
set (NSR_C_FLAGS_RELEASE "${NSR_QCC_FLAGS} ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -O2 -fstack-protector-strong -fvisibility=hidden -Os")
set (NSR_C_FLAGS_RELWITHDEBINFO "${NSR_QCC_FLAGS} ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -O2 -g -fstack-protector-strong -fvisibility=hidden")

set (NSR_CXX_FLAGS_DEBUG "${NSR_QCC_FLAGS} -lang-c++ ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -g")
set (NSR_CXX_FLAGS_MINSIZEREL "${NSR_QCC_FLAGS} -lang-c++ ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -O2 -fstack-protector-strong -fvisibility=hidden -Os")
set (NSR_CXX_FLAGS_RELEASE "${NSR_QCC_FLAGS} -lang-c++ ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -O2 -fstack-protector-strong -fvisibility=hidden -Os")
set (NSR_CXX_FLAGS_RELWITHDEBINFO "${NSR_QCC_FLAGS} -lang-c++ ${NSR_BASE_FLAGS} ${NSR_CPU_FLAGS} -O2 -g -fstack-protector-strong -fvisibility=hidden")

set (NSR_C_FLAGS_CHECK ${NSR_QCC_FLAGS})
set (NSR_CXX_FLAGS_CHECK "${NSR_QCC_FLAGS} -lang-c++")

set (NSR_INCLUDE_DIRECTORIES "${QNX_TARGET}/usr/include")

set (NSR_PLATFORM_BASE_DEFINES "-DQT_NO_IMPORT_QT47_QML -DQ_OS_BLACKBERRY -DQT_DECLARATIVE_DEBUG -DQT_DECLARATIVE_LIB -DQT_XML_LIB -DQT_CORE_LIB -DQT_SHARED")
set (NSR_PLATFORM_DEFINES_DEBUG "${NSR_PLATFORM_BASE_DEFINES}")
set (NSR_PLATFORM_DEFINES_RELEASE "${NSR_PLATFORM_BASE_DEFINES} -D_FORTIFY_SOURCE=2 -DQT_NO_DEBUG")

set (NSR_LIBRARIES bb)

set (CMAKE_FIND_ROOT_PATH ${QNX_TARGET})
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set (CMAKE_LIBRARY_PATH
	"${QNX_TARGET}/${CPUVARDIR}/usr/lib"
	"${QNX_TARGET}/${CPUVARDIR}/lib"
)

set (CMAKE_INCLUDE_PATH
	"${QNX_TARGET}/usr/include"
	"${QNX_TARGET}/usr"
)

set (NSR_PLATFORM "blackberry")
