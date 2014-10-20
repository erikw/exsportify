cmake_minimum_required (VERSION 2.6 FATAL_ERROR)

project("exsportify Spotify Playlist Exporter" CXX)

add_executable(exsportify src/appkey.c src/exsportify.cc)

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
