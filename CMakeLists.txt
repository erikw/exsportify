project("exsportify Spotify Playlist Exporter" CXX)

add_executable(exsportify src/appkey.c src/main/spotify-playlist-exporter.cc src/main/exporter.cc)
