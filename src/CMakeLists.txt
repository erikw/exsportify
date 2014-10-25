# Variables
set(INSTALLED_BIN ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}])
# Boost needs this when linking dynamically to boost.
add_definitions(-DBOOST_LOG_DYN_LINK)


# Libraries
FIND_PACKAGE(Boost 1.50 COMPONENTS program_options log REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/inc)

subdirs(spotify)

# Sources and executable.
set(${PROJECT_NAME}_srcs
	${PROJECT_NAME}.cc
	appkey.c
)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_srcs})

# Linking
target_link_libraries(${PROJECT_NAME} spotify)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
