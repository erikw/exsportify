cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project("exsportify" CXX)
set (PROJECT_NAME "${PROJECT_NAME}")
set ("${PROJECT_NAME}_VERSION_MAJOR" 0)
set ("${PROJECT_NAME}_VERSION_MINOR" 1)

# Pass variables to headerfile
configure_file (
  	"${PROJECT_SOURCE_DIR}/inc/${PROJECT_NAME}.h.in"
  	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}.h"
	)
# Search for generated project header in bin folder.
include_directories("${PROJECT_BINARY_DIR}")

# Include source directory.
subdirs(src)

# CMake does not ship with an uninstall target.
add_custom_target(uninstall
	COMMAND ${SH_EXECUTABLE} test -f ${CMAKE_BINARY_DIR}/install_manifest.txt &&
		while read f\; do
			echo \"rm -f $$f\"\;
			rm -f \"$$f\"\;
			done <${CMAKE_BINARY_DIR}/install_manifest.txt
		|| :
)
