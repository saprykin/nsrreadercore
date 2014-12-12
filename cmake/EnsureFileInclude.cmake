# Ensure that an include file is provided by the system
# Add the check about the mandatory status to the check_include_file() macro 
# provided by cmake

include (${CMAKE_ROOT}/Modules/CheckIncludeFile.cmake)

macro (ensure_file_include INCLUDE_FILENAME VARIABLE_NAME MANDATORY_STATUS)

check_include_file (${INCLUDE_FILENAME} ${VARIABLE_NAME})

if (NOT ${${VARIABLE_NAME}})
	if (${MANDATORY_STATUS})
		message (FATAL_ERROR "The file ${INCLUDE_FILENAME} is mandatory but not found on your system")
 	endif()
endif()

endmacro()
