// TODO memleaks

#include "spotify-playlist-exporter.h"

#define _POSIX_C_SOURCE  199309L	// Needed to get clock_gettime to work.

#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <pthread.h>
#include <libspotify/api.h>

#define USER_AGENT "spotify-playlist-exporter"

#ifdef DEBUG_PRINTING
	#define DEBUG 1
#else
	#define DEBUG 0
#endif

// TODO expand ... to empty if none supplied
#define DEBUG_PRINTF(format, ...)	do { if (DEBUG) { fprintf(stderr, \
				format ,__VA_ARGS__); } } while(0)

#define DEBUG_PRINT(format)	do { if (DEBUG) { fprintf(stderr, \
				format);} } while(0)

sp_session *g_session;
static bool notify_events;
static bool is_logged_in = false;
static bool has_logged_out = false;
static bool all_work_done = false;
static bool all_pl_printed = false;
static bool plc_is_loaded = false;
static sp_playlistcontainer *g_plc = NULL;
static pthread_mutex_t notify_mutex;
static pthread_cond_t notify_cond;
static unsigned int nbr_unloaded_pl = -1;




void playlist_metadata_updated(sp_playlist *pl, void *userdata)
{
	DEBUG_PRINTF("Meta data updated for playlist number %d", *((int *) userdata));
	if (sp_playlist_is_loaded(pl)) {
		DEBUG_PRINTF(", and it's also loaded: %s.\n", sp_playlist_name(pl));
		print_playlist(pl, *(int *) userdata);
		if (nbr_unloaded_pl > 0) { // TODO well this is not unique, one pls can be updated many times.
						// Have to make a dynamica bool 
						// array and check each 
						// individual pls or something,
						// OR screw the 
						// print_playlists() thing and 
						// only print each pls when it's 
						// loaded from callback.
			--nbr_unloaded_pl;
		}
	} else {
		DEBUG_PRINT(", and it's not loaded.\n");
	}
	// TODO unregister callback for pl? Some are called multiple times.
	/*free(userdata);*/
}

static sp_playlist_callbacks playlist_callbacks = {
	.playlist_metadata_updated = playlist_metadata_updated

};

static void connection_error(sp_session *session, sp_error error)
{
	
}

static void playlist_added(sp_playlistcontainer *plc, sp_playlist *pl,
		int position, void *userdata)
{
	DEBUG_PRINTF("Added playlist %d", position);
	if (sp_playlist_is_loaded(pl)) {
		DEBUG_PRINTF(", and it's also loaded: %s.\n", sp_playlist_name(pl));
	} else {
		DEBUG_PRINT(", and it's not loaded.\n");
	}
	// TODO when is it OK to free this? after unregister callback?
	int *user_data = (int *) malloc(sizeof(int));
	if (user_data == NULL) {
		DEBUG_PRINT("Could not allocate int.\n");
		exit(34);
	}
	*user_data = position;
	sp_playlist_add_callbacks(pl, &playlist_callbacks, user_data);
	// TODO check if this is the current playlist we're interested int pos
	/*sp_playlist_release(pl);*/
}

static void playlist_removed(sp_playlistcontainer *plc, sp_playlist *pl, int position,
		void *userdata)
{
	DEBUG_PRINTF("Removed playlist: %s", sp_playlist_name(pl));
}

static void playlist_moved(sp_playlistcontainer *plc, sp_playlist *pl, 
		int position, int new_position, void *userdata)
{
	DEBUG_PRINTF("Playlist \"%s\" moved from position %d to %d.\n",
			sp_playlist_name(pl), position, new_position);
}

static void container_loaded(sp_playlistcontainer *plc, void *userdata)
{
	int n_tracks = sp_playlistcontainer_num_playlists(plc);
	DEBUG_PRINTF("The playlist  container is now loaded with %d tracks.\n", n_tracks);
	nbr_unloaded_pl = n_tracks;
	plc_is_loaded = true;
}

static sp_playlistcontainer_callbacks plc_callbacks = {
	.playlist_added = playlist_added,
	.playlist_removed = playlist_removed,
	.playlist_moved = playlist_moved,
	.container_loaded = container_loaded
};

static void logged_in(sp_session *session, sp_error error)
{
	if (error == SP_ERROR_OK) {
		DEBUG_PRINTF("Logged in as user %s!\n", sp_session_user_name(session));
		is_logged_in = true;
		g_plc =  sp_session_playlistcontainer(g_session);
		// TODO needs to check is_loaded before adding callbacks?
		if (sp_playlistcontainer_add_callbacks(g_plc, &plc_callbacks,
					NULL) == SP_ERROR_OK) {
			DEBUG_PRINT("Callbacks for playlistcontainer succesfully "
					"added.\n");

		}

	} else {
		DEBUG_PRINTF("could not login: %s\n",
				sp_error_message(error));
		sp_session_release(g_session);
		exit(2);
	}
}

static void logged_out(sp_session *session)
{
	DEBUG_PRINT("User is now logged out.\n");
	has_logged_out = true;
}


/**
 * This callback is called for log messages.
 *
 * @sa sp_session_callbacks#log_message
 */
static void log_message(sp_session *session, const char *data)
{
	DEBUG_PRINTF("%s",data);
}

void notify_main_thread(sp_session *session)
{
	pthread_mutex_lock(&notify_mutex);
	notify_events = true;
	pthread_cond_signal(&notify_cond);
	pthread_mutex_unlock(&notify_mutex);
}

static sp_session_callbacks callbacks = {
	.logged_in = logged_in,
	.logged_out = logged_out,
	.connection_error = connection_error,
	.notify_main_thread = notify_main_thread,
	.log_message = log_message
};

int spotify_init(const char *username,const char *password)
{
	sp_session_config config;
	sp_error error;
	sp_session *session;

	// Default values to 0.
	memset(&config, 0, sizeof(sp_session_config));

	/// The application key is specific to each project, and allows Spotify
	/// to produce statistics on how our service is used.
	extern const char g_appkey[];
	/// The size of the application key.
	extern const size_t g_appkey_size;

	// Always do this. It allows libspotify to check for
	// header/library inconsistencies.
	config.api_version = SPOTIFY_API_VERSION;

	// The path of the directory to store the cache. This must be specified.
	// Please read the documentation on preferred values.
	config.cache_location = "cache";

	// The path of the directory to store the settings. 
	// This must be specified.
	// Please read the documentation on preferred values.
	config.settings_location = "cache";

	// The key of the application. They are generated by Spotify,
	// and are specific to each application using libspotify.
	config.application_key = g_appkey;
	config.application_key_size = g_appkey_size;

	// This identifies the application using some
	// free-text string [1, 255] characters.
	config.user_agent = USER_AGENT;


	// Register the callbacks.
	config.callbacks = &callbacks;

	error = sp_session_create(&config, &session);
	if (SP_ERROR_OK != error) {
		DEBUG_PRINTF("failed to create session: %s\n",
		                sp_error_message(error));
		return 2;
	}

	// Login using the credentials given on the command line.
	error = sp_session_login(session, username, password, false, NULL);

	if (SP_ERROR_OK != error) {
		DEBUG_PRINTF("failed to login: %s\n",
		                sp_error_message(error));
		return 3;
	}

	g_session = session;
	return 0;
}



void print_playlist(sp_playlist *pl, int pl_number)
{
	const char *pl_name = sp_playlist_name(pl);
	/*pl = sp_playlistcontainer_playlist(g_plc, i);*/
	int pl_num_tracks = sp_playlist_num_tracks(pl);
	int pl_num_subscribers = sp_playlist_num_subscribers(pl);
	switch (sp_playlistcontainer_playlist_type(g_plc, pl_number)) {
			case SP_PLAYLIST_TYPE_PLAYLIST:
			DEBUG_PRINTF("====> %d. %s (%d tracks, %d subscribers)\n", pl_number, pl_name, 
			pl_num_tracks, pl_num_subscribers);
			break;
		case SP_PLAYLIST_TYPE_START_FOLDER:
			break; case SP_PLAYLIST_TYPE_END_FOLDER:
			break;
		case SP_PLAYLIST_TYPE_PLACEHOLDER:
			break;
	}
}

void print_playlists()
{
	sp_playlist *pl; // TODO Extremly strange parser bug, can declare + init 
	// this type after "case + label:"
	if (!sp_playlistcontainer_is_loaded(g_plc)) {
		DEBUG_PRINT("plc is not loaded yet.\n");
		return;
	}
	int pls_count = sp_playlistcontainer_num_playlists(g_plc);
	DEBUG_PRINTF("%d playlists.\n", pls_count);
	for (int i = 0; i < pls_count; ++i) {
		switch (sp_playlistcontainer_playlist_type(g_plc, i)) {
			case SP_PLAYLIST_TYPE_PLAYLIST:
			pl = sp_playlistcontainer_playlist(g_plc, i);
			print_playlist(pl, i);
			break;
			case SP_PLAYLIST_TYPE_START_FOLDER:
			break;
			case SP_PLAYLIST_TYPE_END_FOLDER:
			break;
			case SP_PLAYLIST_TYPE_PLACEHOLDER:
			break;
		}

	}
	all_pl_printed = true;
	// Sefgault if we release plc, why?
	/*if (sp_playlistcontainer_release(g_plc) == SP_ERROR_OK) {*/
	/*DEBUG_PRINTF("Playlistcontainer released.\n");;*/
	/*} else {*/
	/*DEBUG_PRINTF("Failed to release Playlistcontainer.\n");*/
	/*exit(3);*/
	/*}*/
}

static void process_libspotify_events(int *next_timeout)
{
	do {
		sp_session_process_events(g_session, next_timeout);
	} while (next_timeout == 0);

}

int main(int argc, char **argv)
{
	int next_timeout = 0;
	if (argc < 3) {
		DEBUG_PRINTF("Usage: %s <username> <password>\n",argv[0]);
		return 1;
	}
	pthread_mutex_init(&notify_mutex, NULL);
	pthread_cond_init(&notify_cond, NULL);

	// TODO allow for argc == 1 and try logging in from cache.  Like spshell
	if (spotify_init(argv[1], argv[2]) != 0) {
		DEBUG_PRINT("Spotify failed to initialize\n");
		exit(-1);
	}
	pthread_mutex_lock(&notify_mutex);
	while (!all_work_done) {
		/*DEBUG_PRINTF("Continuing loop work.\n");*/
		if (next_timeout == 0) {
			while (!notify_events)
				pthread_cond_wait(&notify_cond, &notify_mutex);
		} else {
			struct timespec ts;

#if _POSIX_TIMERS > 0
			clock_gettime(CLOCK_REALTIME, &ts);
#else
			struct timeval tv;
			gettimeofday(&tv, NULL);
			TIMEVAL_TO_TIMESPEC(&tv, &ts);
#endif

			ts.tv_sec += next_timeout / 1000;
			ts.tv_nsec += (next_timeout % 1000) * 1000000;

			while (!notify_events) {
				if (pthread_cond_timedwait(&notify_cond, &notify_mutex, &ts))
					break;
			}
		}


		// Program work.
		if (is_logged_in && plc_is_loaded && nbr_unloaded_pl == 0 && !all_pl_printed) {
			pthread_mutex_unlock(&notify_mutex);
			print_playlists();
			pthread_mutex_lock(&notify_mutex);
		}

		if (all_pl_printed && is_logged_in) {
			is_logged_in = false;
			if (sp_session_logout(g_session) == SP_ERROR_OK) {
				DEBUG_PRINT("Logging out...\n");
			} else {
				DEBUG_PRINT("Failed to start log out.\n");
				sp_session_release(g_session);
				return EXIT_FAILURE;
			}
		}
		if (all_pl_printed && has_logged_out) {
			all_work_done = true;
		}
		// Process libspotify events
		notify_events = false;
		pthread_mutex_unlock(&notify_mutex);
		process_libspotify_events(&next_timeout);	
		pthread_mutex_lock(&notify_mutex);
	}
	DEBUG_PRINT("All done, exiting.\n");
	sp_session_release(g_session);
}
