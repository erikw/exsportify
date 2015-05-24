#include <string>
#include <stdlib.h>

#include "debug.h"
#include "event_loop.h"
#include "spotify.h"

struct NotAllSpotifyDataLoadedException : std::runtime_error::runtime_error {
	NotAllSpotifyDataLoadedException()
	    : NotAllSpotifyDataLoadedException(
		  "Not All spotify data is loaded yet.") {}

	NotAllSpotifyDataLoadedException(std::string msg)
		: std::runtime_error(msg) {}

};

static void process_libspotify_events(int *next_timeout) {
	do {
		sp_session_process_events(spotify->session, next_timeout);
	} while (next_timeout == 0);
}

static void start_logout() {
	spotify->is_logged_in = false;
	if (sp_session_logout(spotify->session) == SP_ERROR_OK) {
		logt(trace) << "Logout started.";
	} else {
		logt(error) << "Failed to start Logout.";
		exit(EXIT_FAILURE);
	}
}

static void check_album_loaded(sp_album *album) {
	if (!sp_album_is_loaded(album)) {
		throw NotAllSpotifyDataLoadedException(
		    "All albums are not loaded.");
	}
}

static void check_artist_loaded(sp_artist *artist) {
	if (!sp_artist_is_loaded(artist)) {
		throw NotAllSpotifyDataLoadedException(
		    "All artists are not loaded.");
	}
}

static void check_track_loaded(sp_track *track) {
	if (!sp_track_is_loaded(track)) {
		throw NotAllSpotifyDataLoadedException(
		    "All tracks are not loaded.");
	}
	sp_album *album = sp_track_album(track);
	check_album_loaded(album);

	int n_artists = sp_track_num_artists(track);
	for (int cur_artist = 0; cur_artist < n_artists; ++cur_artist) {
		sp_artist *artist = sp_track_artist(track, cur_artist);
		check_artist_loaded(artist);
	}

}

static void check_playlist_loaded(sp_playlist *pl) {
	if (!sp_playlist_is_loaded(pl)) {
		throw NotAllSpotifyDataLoadedException(
		    "Not all playlists are loaded");
	}

	for(int cur_track = 0; cur_track < sp_playlist_num_tracks(pl); ++cur_track) {
		sp_track *track = sp_playlist_track(pl, cur_track);
		check_track_loaded(track);
	}
}

static void check_all_data_loaded() {
	logt(trace) << "Checking if all data is loaded.";

	int n_playlists =
		sp_playlistcontainer_num_playlists(spotify->pl_container);
	for (int cur_pl = 0; cur_pl < n_playlists; ++cur_pl) {
		sp_playlist *pl =
		    sp_playlistcontainer_playlist(spotify->pl_container, cur_pl);
		check_playlist_loaded(pl);
	}

	logt(trace) << "All data is loaded!";
	spotify->all_data_loaded = true;
}

static void print_all_data() {
	logt(trace) << "Printing all data";
	;

}


void event_loop::operator()() {
	int next_timeout = 0;
	bool process_events = true;


	logt(trace) << "In event_loop";
	boost::unique_lock<boost::mutex> lock(spotify->mutex);
	while (process_events) {
		if (next_timeout == 0) {
			while (!spotify->notify)
				spotify->condition.wait(lock);
		} else {
			boost::system_time const timeout =
				boost::get_system_time() +
				boost::posix_time::milliseconds(next_timeout);
			while (!spotify->notify) {
				if (!spotify->condition.timed_wait(lock, timeout)) {
					break;
				}
			}
		}


		// Program Work
		if (spotify->has_logged_out) {
			process_events = false;
		} else if (spotify->is_logged_in) {
			try {
				check_all_data_loaded();
			} catch (NotAllSpotifyDataLoadedException &nasdle) {
				logt(trace) << nasdle.what();
			}

			if (spotify->all_data_loaded) {
				print_all_data();
				start_logout();
			}
		}


		// Process libspotify events.
		spotify->notify = false;
		lock.unlock();
		process_libspotify_events(&next_timeout);
		lock = boost::unique_lock<boost::mutex>(spotify->mutex);
	}
	lock.unlock();
}
