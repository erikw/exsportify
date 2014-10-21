include_directories(${PROJECT_SOURCE_DIR}/inc/spotify)
project("spotify" CXX)

set(${PROJECT_NAME}_srcs
  spotify.cc
)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_srcs})
