#include <string>
#include <stdlib.h>

#include "debug.h"
#include "event_loop.h"
#include "spotify.h"

/* Maximum lenght of a playlist folder name. */
#define PLAYLIST_FOLDER_NAME_LEN	(256)

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

static void export_track(sp_track *track) {

	if (sp_track_is_starred(spotify->session, track)) {
		std::cout << "* ";
	} else {
		std::cout << "  ";
	}

	int nbr_artists = sp_track_num_artists(track);
	for (int cur_artist = 0; cur_artist < nbr_artists; ++cur_artist) {
		sp_artist *artist = sp_track_artist(track, cur_artist);
		const char *artist_name = sp_artist_name(artist);
		std::cout << artist_name;
		if (cur_artist < nbr_artists - 1) {
			std::cout << ", ";
		}
	}
	std::cout << " - ";

	sp_album *album = sp_track_album(track);
	const char *album_name = sp_album_name(album);
	std::cout << album_name;
	std::cout << " - ";
	const char *track_name = sp_track_name(track);
	std::cout << track_name;
	std::cout << std::endl;
	// TODO print more data e.g. generate link.
}

static char *get_pl_folder_name(int pl_number)
{
	char *pl_folder_name = new char[PLAYLIST_FOLDER_NAME_LEN];
	if (pl_folder_name == NULL) {
		logt(error) << "Could not alloc pl_folder_name";
		exit(42);
	}
	pl_folder_name[0] = '\0';
	sp_error error = sp_playlistcontainer_playlist_folder_name(
	    spotify->pl_container, pl_number, pl_folder_name,
	    PLAYLIST_FOLDER_NAME_LEN);
	if (error != SP_ERROR_OK) {
		logt(error) << "Could not get folder name for playlist " <<
				pl_number;
	}
	return pl_folder_name;
}

static void export_playlist(sp_playlist *pl, int pl_number) {
	sp_user * pl_owner = sp_playlist_owner(pl);
	const char *pl_owner_name = sp_user_display_name(pl_owner);
	const char *pl_name = sp_playlist_name(pl);
	int pl_num_tracks = sp_playlist_num_tracks(pl);
	int pl_num_subscribers = sp_playlist_num_subscribers(pl);
	std::cout << "===> " << pl_number << ". ";
	switch (sp_playlistcontainer_playlist_type(spotify->pl_container,
						   pl_number)) {
		case SP_PLAYLIST_TYPE_PLAYLIST:
			std::cout <<  pl_name << " ("
			<< pl_num_tracks << "tracks, " << pl_num_subscribers <<
			" subscribers), owner: " << pl_owner_name << std::endl;
			break;
		case SP_PLAYLIST_TYPE_START_FOLDER: {
			char *pl_folder_name = get_pl_folder_name(pl_number);
			std::cout << "BEGIN folder: " << pl_folder_name
				  << std::endl;
			delete pl_folder_name;
			break;
		}
		case SP_PLAYLIST_TYPE_END_FOLDER:
			std::cout << "END folder" << std::endl;
			break;
		case SP_PLAYLIST_TYPE_PLACEHOLDER:
			std::cout << "PLACEHOLDER " << std::endl;
			break;
	}


	for(int cur_track = 0; cur_track < pl_num_tracks; ++cur_track) {
		sp_track *track = sp_playlist_track(pl, cur_track);
		export_track(track);
	}
}

static void print_all_data() {
	logt(trace) << "Printing all data";
	int n_playlists =
		sp_playlistcontainer_num_playlists(spotify->pl_container);
	for (int cur_pl = 0; cur_pl < n_playlists; ++cur_pl) {
		sp_playlist *pl =
		    sp_playlistcontainer_playlist(spotify->pl_container, cur_pl);
		export_playlist(pl, cur_pl);
	}
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
