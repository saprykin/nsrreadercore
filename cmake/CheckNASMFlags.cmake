  ###
  #
  # This file provides a NSR_CHECK_NASM_FLAGS macro that detects required
  # definitions and compile flags for specific OS when using NASM compiler.
  # Macro uses SYSTEM_NAME variable to detect target OS.
  #
  # Common usage:
  #
  # NSR_CHECK_NASM_FLAGS (FLAGS OBJ_FORMAT)
  # set (CMAKE_ASM_NASM_FLAGS "${CMAKE_ASM_NASM_FLAGS} ${FLAGS}")
  # set (CMAKE_ASM_NASM_OBJECT_FORMAT ${OBJ_FORMAT})
  #
  # (c) 2014 Alexander Saprykin.
  #
  ###
  
include (CheckCSourceCompiles)
include (CheckTypeSize)

macro (NSR_CHECK_NASM_FLAGS FLAGS OBJ_FORMAT)
	if (NOT NSR_NASM_FLAGS OR NOT NSR_NASM_OBJ_FORMAT)
		set (NASM_OBJ_FORMAT "")

		check_type_size ("unsigned char *" SIZEOF_UNSIGNED_CHAR_P)

		string (TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)

		if (
			SYSTEM_NAME MATCHES "^cygwin" OR
			SYSTEM_NAME MATCHES "^mingw" OR
			SYSTEM_NAME MATCHES "^pw32" OR
			SYSTEM_NAME MATCHES "^interix"
		)
			if (SIZEOF_UNSIGNED_CHAR_P EQUAL 8)
				set (NASM_OBJ_FORMAT "Win64-COFF")
			else()
				set (NASM_OBJ_FORMAT "Win32-COFF")
			endif()
		elseif (
			SYSTEM_NAME MATCHES "^msdosdjgpp" OR
			SYSTEM_NAME MATCHES "^go32"
		)
			set (NASM_OBJ_FORMAT "COFF")
		elseif (SYSTEM_NAME MATCHES "^os2-emx")
			set (NASM_OBJ_FORMAT "MSOMF")
		elseif (
			SYSTEM_NAME MATCHES "^linux.*coff" OR
			SYSTEM_NAME MATCHES "^linux.*oldld"
		)
			set (NASM_OBJ_FORMAT "COFF")
		elseif (SYSTEM_NAME MATCHES "^linux.*aout")
			set (NASM_OBJ_FORMAT "a.aout")
		elseif (SYSTEM_NAME MATCHES "^linux")
			if (SIZEOF_UNSIGNED_CHAR_P EQUAL 8)
				set (NASM_OBJ_FORMAT "ELF64")
			else()
				set (NASM_OBJ_FORMAT "ELF")
			endif()
		elseif (
			SYSTEM_NAME MATCHES "^kfreebsd" OR
			SYSTEM_NAME MATCHES "^freebsd" OR
			SYSTEM_NAME MATCHES "^netbsd" OR
			SYSTEM_NAME MATCHES "^openbsd"
		)
			EXECUTE_PROCESS (
				OUTPUT_VARIABLE BSD_OBJ_FORMAT
				COMMAND echo __ELF__
				COMMAND cpp -					
				COMMAND grep "__ELF__"
			)

			if (BSD_OBJ_FORMAT STREQUAL "__ELF__")
				set (NASM_OBJ_FORMAT "BSD-a.out")
			else()
				if (SIZEOF_UNSIGNED_CHAR_P EQUAL 8)
					set (NASM_OBJ_FORMAT "ELF64")
				else()
					set (NASM_OBJ_FORMAT "ELF")
				endif()
			endif()
		elseif (
			SYSTEM_NAME MATCHES "^solaris" OR
			SYSTEM_NAME MATCHES "^sunos" OR
			SYSTEM_NAME MATCHES "^sysv" OR
			SYSTEM_NAME MATCHES "^sco"
		)
			if (SIZEOF_UNSIGNED_CHAR_P EQUAL 8)
				set (NASM_OBJ_FORMAT "ELF64")
			else()
				set (NASM_OBJ_FORMAT "ELF")
			endif()
		elseif (
			SYSTEM_NAME MATCHES "^darwin" OR
			SYSTEM_NAME MATCHES "^rhapsody" OR
			SYSTEM_NAME MATCHES "^nextstep" OR
			SYSTEM_NAME MATCHES "^openstep"
		)
			if (SIZEOF_UNSIGNED_CHAR_P EQUAL 8)
				set (NASM_OBJ_FORMAT "Mach-O64")
			else()
				set (NASM_OBJ_FORMAT "Mach-O")
			endif()
		elseif (SYSTEM_NAME MATCHES "^qnx")
			set (NASM_OBJ_FORMAT "ELF")
		else()
			set (NASM_OBJ_FORMAT "ELF?")
		endif()

		message (STATUS "NASM file object format: ${NASM_OBJ_FORMAT}")

		if (NASM_OBJ_FORMAT MATCHES "ELF.")
			set (NASM_OBJ_FORMAT "ELF")
			message (STATUS "Unexpected NASM obejct format, assuming ELF.")
		endif()

		set (NAFLAGS "")
		set (NAOBJFORMAT "")

		if (NASM_OBJ_FORMAT STREQUAL "MSOMF")
			set (NAFLAGS "-DOBJ32")
			set (NAOBJFORMAT "obj")
		elseif (NASM_OBJ_FORMAT STREQUAL "Win32-COFF")
			set (NAFLAGS "-DWIN32")
			set (NAOBJFORMAT "win32")
		elseif (NASM_OBJ_FORMAT STREQUAL "Win64-COFF")
			set (NAFLAGS "-DWIN64 -D__x86_64__")
			set (NAOBJFORMAT "win64")
		elseif (NASM_OBJ_FORMAT STREQUAL "COFF")
			set (NAFLAGS "-DCOFF")
			set (NAOBJFORMAT "coff")
		elseif (NASM_OBJ_FORMAT STREQUAL "a.out")
			set (NAFLAGS "-DAOUT")
			set (NAOBJFORMAT "aout")
		elseif (NASM_OBJ_FORMAT STREQUAL "BSD-a.out")
			set (NAFLAGS "-DAOUT")
			set (NAOBJFORMAT "aoutb")
		elseif (NASM_OBJ_FORMAT STREQUAL "ELF")
			set (NAFLAGS "-DELF")
			set (NAOBJFORMAT "elf")
		elseif (NASM_OBJ_FORMAT STREQUAL "ELF64")
			set (NAFLAGS "-DELF -D__x86_64__")
			set (NAOBJFORMAT "elf64")
		elseif (NASM_OBJ_FORMAT STREQUAL "RDF")
			set (NAFLAGS "-DRDF")
			set (NAOBJFORMAT "rdf")
		elseif (NASM_OBJ_FORMAT STREQUAL "Mach-O")
			set (NAFLAGS "-DMACHO")
			set (NAOBJFORMAT "macho")
		elseif (NASM_OBJ_FORMAT STREQUAL "Mach-O64")
			set (NAFLAGS "-DMACHO -D__x86_64__")
			set (NAOBJFORMAT "macho64")
		endif()

		message (STATUS "NASM compile flags: ${NAFLAGS}")
		message (STATUS "NASM file object format: ${NAOBJFORMAT}")
	endif()

	if (NOT NSR_NASM_FLAGS)
		set (NSR_NASM_FLAGS "${NAFLAGS}" CACHE INTERNAL "NASM compile flags")
	endif()

	if (NOT NSR_NASM_OBJ_FORMAT)
		set (NSR_NASM_OBJ_FORMAT "${NAOBJFORMAT}" CACHE INTERNAL "NASM file object format")
	endif()

	set (FLAGS "${FLAGS} ${NSR_NASM_FLAGS}")
	set (OBJ_FORMAT ${NAOBJFORMAT})
endmacro()
