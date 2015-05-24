cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project("exsportify" C CXX)

# Settings
set (PROJECT_NAME "${PROJECT_NAME}")
set ("${PROJECT_NAME}_VERSION_MAJOR" 0)
set ("${PROJECT_NAME}_VERSION_MINOR" 1)

#set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_BUILD_TYPE DEBUG)


# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Compilation flags.
# C
set(CMAKE_C_FLAGS "-Wall -Wextra -Wmissing-braces -Wparentheses")
set(CMAKE_C_FLAGS_DEBUG "-O0 -ggdb3 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O0 -ggdb3")

# C++
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wmissing-braces -Wparentheses -Wold-style-cast -pipe --std=c++11 -pedantic-errors -fstack-protector-all")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb3 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Pass variables to headerfile
configure_file (
  	"${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}.h.in"
  	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}.h"
	)
# Search for generated project header in bin folder.
include_directories("${PROJECT_BINARY_DIR}")

# Include source directory.
subdirs(src)

add_custom_target(uninstall
	COMMAND ${SH_EXECUTABLE} test -f ${CMAKE_BINARY_DIR}/install_manifest.txt &&
		while read f\; do
			echo \"rm -f $$f\"\;
			rm -f \"$$f\"\;
			done <${CMAKE_BINARY_DIR}/install_manifest.txt
		|| :
	COMMENT Uninstall target (CMake provide one by default).
)

add_custom_target(checkmem
	COMMAND valgrind --leak-check=full --suppressions=${CMAKE_SOURCE_DIR}/valgrind.supp bin/${PROJECT_NAME}
	DEPENDS bin/${PROJECT_NAME}
	COMMENT Memory leakcheck target.
)
