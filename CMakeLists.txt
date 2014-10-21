cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project("exsportify" CXX)
set (PROJECT_NAME "${PROJECT_NAME}")
set ("${PROJECT_NAME}_VERSION_MAJOR" 0)
set ("${PROJECT_NAME}_VERSION_MINOR" 1)

# Pass variables to headerfile
configure_file (
  	"${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}.h.in"
  	"${PROJECT_BINARY_DIR}/${PROJECT_NAME}.h"
	)
# Search for generated project header in bin folder.
include_directories("${PROJECT_BINARY_DIR}")


# Libraries
FIND_PACKAGE(Boost 1.50 COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# Executable
add_executable(${PROJECT_NAME} src/appkey.c src/${PROJECT_NAME}.cc)

# Linking
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})



#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Installation
install (TARGETS ${PROJECT_NAME} DESTINATION bin)
