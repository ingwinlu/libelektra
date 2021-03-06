# ~~~
# Elektra Test Suite
#
# This file is responsible for generating all tests
# regarding Elektra's core.
#
# Backends and Bindings specific tests might be in
# their folders.
# ~~~

include (LibAddMacros)

if (ENABLE_TESTING)

	include (FindGTest)

	if (DEFINED GOOGLETEST_ROOT)
		set (googletest_SOURCE_DIR "${GOOGLETEST_ROOT}")
		set (googletest_BINARY_DIR "${CMAKE_BINARY_DIR}/gtest")
	else (DEFINED GOOGLETEST_ROOT)
		include (DownloadProject)
		if (CMAKE_VERSION VERSION_LESS 3.2)
			set (UPDATE_DISCONNECTED_IF_AVAILABLE "")
		else ()
			set (UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
		endif ()

		# Since the last released version of Google Test (`release-1.8.0`) is quite old, we download a more recent commit of the
		# repository instead.
		download_project (PROJ googletest
				  GIT_REPOSITORY https://github.com/google/googletest.git
				  GIT_TAG 4e4df226fc197c0dda6e37f5c8c3845ca1e73a49
				  QUIET
				  ${UPDATE_DISCONNECTED_IF_AVAILABLE})
	endif (DEFINED GOOGLETEST_ROOT)

	# Prevent GoogleTest from overriding our compiler/linker options when building with Visual Studio
	set (gtest_force_shared_crt
	     ON
	     CACHE BOOL
		   ""
	     FORCE)

	add_subdirectory (${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

	# Old versions of Google Test do not include Google Mock
	if (TARGET gmock)
		set_property (TARGET gmock
			      PROPERTY COMPILE_FLAGS
				       "-Wno-undef -Wno-missing-field-initializers")
		set_property (TARGET gmock_main
			      PROPERTY COMPILE_FLAGS
				       "-Wno-undef")
	endif (TARGET gmock)

	set_property (TARGET gtest
		      PROPERTY COMPILE_FLAGS
			       "-Wno-undef -Wno-missing-field-initializers")
	set_property (TARGET gtest_main
		      PROPERTY COMPILE_FLAGS
			       "-Wno-undef")
endif (ENABLE_TESTING)

# don't call add_headers in a loop
add_headers (HDR_FILES)

macro (do_test source)
	include_directories ("${CMAKE_CURRENT_SOURCE_DIR}")
	set (SOURCES ${HDR_FILES} ${source}.c $<TARGET_OBJECTS:cframework>)
	add_headers (SOURCES)
	add_testheaders (SOURCES)
	add_executable (${source} ${SOURCES})
	add_dependencies (${source} kdberrors_generated)

	if (INSTALL_TESTING)
		install (TARGETS ${source} DESTINATION ${TARGET_TOOL_EXEC_FOLDER})
	endif (INSTALL_TESTING)

	target_link_elektra (${source})

	set_target_properties (${source}
			       PROPERTIES COMPILE_DEFINITIONS
					  HAVE_KDBCONFIG_H)
	set_target_properties (${source}
			       PROPERTIES LINKER_LANGUAGE
					  CXX)
	add_test (${source} "${CMAKE_BINARY_DIR}/bin/${source}" "${CMAKE_CURRENT_BINARY_DIR}")
	set_property (TEST ${source}
		      PROPERTY ENVIRONMENT
			       "LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib")
endmacro (do_test)

if (NOT CMAKE_VERSION VERSION_LESS 3.2)
	set (USES_TERMINAL USES_TERMINAL)
endif (NOT CMAKE_VERSION VERSION_LESS 3.2)

add_custom_target (run_all
		   COMMAND "${CMAKE_SOURCE_DIR}/scripts/run_all" "$<CONFIGURATION>"
		   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
		   ${USES_TERMINAL})

add_custom_target (run_nokdbtests
		   COMMAND "${CMAKE_SOURCE_DIR}/scripts/run_nokdbtests" "$<CONFIGURATION>"
		   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

add_custom_target (run_memcheck
		   COMMAND "${CMAKE_SOURCE_DIR}/scripts/run_memcheck" "$<CONFIGURATION>"
		   WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

include_directories (data)

add_subdirectory (cframework)
add_subdirectory (shell)
add_subdirectory (shell/shell_recorder)
add_subdirectory (abi)
add_subdirectory (ctest)
if (ENABLE_KDB_TESTING)
	add_subdirectory (kdb)
endif (ENABLE_KDB_TESTING)
