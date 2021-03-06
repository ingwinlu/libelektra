include (LibAddMacros)

# ~~~
# This functions checks if the dependencies for the YAMBi plugin are available.
#
# If they are, the function sets the variable `FOUND_DEPENDENCIES` to `TRUE`. The function then also sets:
#
# - BISON_YAMBI_OUTPUT_SOURCE to the path of the source files generated by Bison, and
# - BISON_YAMBI_OUTPUT_HEADER to the path of the header files generated by Bison
#
# . If the function was unsuccessful it sets `FOUND_DEPENDENCIES` to `FALSE` and stores the reason for the failure in the variable
# `FAILURE_MESSAGE`.
# ~~~
function (check_dependencies)
	set (FOUND_DEPENDENCIES FALSE PARENT_SCOPE)

	include (CheckIncludeFileCXX)
	set (CMAKE_REQUIRED_QUIET ON)
	check_include_file_cxx (codecvt HAVE_CODECVT)
	if (NOT HAVE_CODECVT)
		set (FAILURE_MESSAGE "because the current C++ library does not provide header file `codecvt`" PARENT_SCOPE)
		return ()
	endif (NOT HAVE_CODECVT)

	if (APPLE)
		# Add path to Homebrew version of Bison
		list (APPEND CMAKE_PREFIX_PATH
			     "/usr/local/opt/bison")
	endif (APPLE)
	find_package (BISON 3 QUIET)

	if (NOT BISON_FOUND)
		set (FAILURE_MESSAGE "Bison 3 (bison) not found" PARENT_SCOPE)
		return ()
	endif (NOT BISON_FOUND)

	if (BISON_VERSION VERSION_LESS 3)
		set (FAILURE_MESSAGE "Bison version 3 or later required (found version ${BISON_VERSION})" PARENT_SCOPE)
		return ()
	endif (BISON_VERSION VERSION_LESS 3)

	bison_target (YAMBI ${CMAKE_CURRENT_SOURCE_DIR}/parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)

	if (NOT BISON_YAMBI_DEFINED)
		set (FAILURE_MESSAGE "generating the parser code failed" PARENT_SCOPE)
		return ()
	endif (NOT BISON_YAMBI_DEFINED)

	set (BISON_YAMBI_OUTPUT_SOURCE ${BISON_YAMBI_OUTPUT_SOURCE} PARENT_SCOPE)
	set (BISON_YAMBI_OUTPUT_HEADER ${BISON_YAMBI_OUTPUT_HEADER} PARENT_SCOPE)

	set (FOUND_DEPENDENCIES TRUE PARENT_SCOPE)
endfunction (check_dependencies)

if (DEPENDENCY_PHASE)
	check_dependencies ()
	if (NOT FOUND_DEPENDENCIES)
		remove_plugin (yambi ${FAILURE_MESSAGE})
	endif (NOT FOUND_DEPENDENCIES)

	set (SOURCE_FILES_INPUT input.hpp input.cpp)

	if (ENABLE_ASAN AND CMAKE_COMPILER_IS_GNUCXX)
		# ~~~
		# Ignore runtime error about member call on address, which does not point to object of type `__codecvt_abstract_base`.
		# See also: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81068
		# ~~~
		set_source_files_properties (${SOURCE_FILES_INPUT} PROPERTIES COMPILE_FLAGS "-fno-sanitize=undefined")
	endif (ENABLE_ASAN AND CMAKE_COMPILER_IS_GNUCXX)

	set (SOURCE_FILES
	     convert.hpp
	     convert.cpp
	     driver.hpp
	     driver.cpp
	     lexer.hpp
	     lexer.cpp
	     yambi.hpp
	     yambi.cpp
	     ${SOURCE_FILES_INPUT}
	     ${BISON_YAMBI_OUTPUT_SOURCE}
	     ${BISON_YAMBI_OUTPUT_HEADER})
endif (DEPENDENCY_PHASE)

add_plugin (yambi
	    CPP
	    ADD_TEST
	    CPP_TEST
	    INSTALL_TEST_DATA
	    TEST_README
	    TEST_REQUIRED_PLUGINS directoryvalue
				  yamlsmith
	    SOURCES ${SOURCE_FILES}
	    INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})
