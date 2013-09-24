#ifndef SPOTIFY_PLAYLIST_EXPORTER_H
#define SPOTIFY_PLAYLIST_EXPORTER_H

#define _POSIX_C_SOURCE  199309L	// Needed to get clock_gettime to work.

#include <sys/time.h>
#include <libspotify/api.h>

bool print_playlist(sp_playlist *pl, int pl_number);
void print_playlists();

#endif
