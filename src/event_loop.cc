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

static void check_all_data_loaded() {
	logt(trace) << "Checking if all data is loaded.";

	int n_playlists =
		sp_playlistcontainer_num_playlists(spotify->pl_container);
	for (int pl_n = 0; pl_n < n_playlists; ++pl_n) {
		sp_playlist *pl =
		    sp_playlistcontainer_playlist(spotify->pl_container, pl_n);
		if (!sp_playlist_is_loaded(pl)) {
			throw NotAllSpotifyDataLoadedException(
			    "Not all playlists are loaded");
		}
	}
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
				spotify->all_data_loaded = true;
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
