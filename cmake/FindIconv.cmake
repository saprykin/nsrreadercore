# - Try to find Iconv
# Once done this will define
# 
#  Iconv_FOUND - system has Iconv
#  Iconv_INCLUDE_DIR - the Iconv include directory
#  Iconv_LIBRARIES - Link these to use Iconv
#  Iconv_VERSION - Iconv version string
#  Iconv_SECOND_ARGUMENT_IS_CONST - the second argument for iconv() is const
# 

include (CheckCSourceCompiles)
include (FindPackageHandleStandardArgs)

set (Iconv_FOUND false)

find_path (Iconv_INCLUDE_DIR iconv.h)
find_library (Iconv_LIBRARIES NAMES iconv libiconv libiconv-2 c)

# Handle the QUIETLY and REQUIRED arguments and set Iconv_FOUND to TRUE if
# all listed variables are TRUE

find_package_handle_standard_args (Iconv
	FOUND_VAR
	Iconv_FOUND
	REQUIRED_VARS
	Iconv_LIBRARIES
	Iconv_INCLUDE_DIR
)

if (Iconv_FOUND)
	set (CMAKE_REQUIRED_INCLUDES ${Iconv_INCLUDE_DIR})
	set (CMAKE_REQUIRED_LIBRARIES ${Iconv_LIBRARIES})

	check_c_source_compiles (
		"#include <iconv.h>
		int main(){
			iconv_t conv = 0;
			const char* in = 0;
			size_t ilen = 0;
			char* out = 0;
			size_t olen = 0;
			iconv(conv, &in, &ilen, &out, &olen);
			return 0;
 		}"
		Iconv_SECOND_ARGUMENT_IS_CONST
	)
endif (Iconv_FOUND)

mark_as_advanced (
  Iconv_INCLUDE_DIR
  Iconv_LIBRARIES
  Iconv_SECOND_ARGUMENT_IS_CONST
)
