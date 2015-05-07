#include <stdlib.h>

#include <libspotify/api.h>

#include "session.h"
#include "debug.h"
#include "spotify.h"

sp_session_callbacks session_callbacks = {
	session_logged_in,
	NULL,
	NULL,
	NULL,
	NULL,
	session_notify_main_thread,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

void session_logged_in(sp_session *session, sp_error error) {
	if (error == SP_ERROR_OK) {
		BOOST_LOG_TRIVIAL(trace) << "Logged in as user " << sp_session_user_name(session);
		spotify.is_logged_in = true;
		spotify.playlistcontainer =  sp_session_playlistcontainer(spotify.session);
		// TODO needs to check is_loaded before adding callbacks?
		/*if (sp_playlistcontainer_add_callbacks(g_plc, &plc_callbacks,*/
					/*NULL) == SP_ERROR_OK) {*/
			/*BOOST_LOG_TRIVIAL(trace) << "Callbacks for playlistcontainer succesfully "*/
					/*"added.";*/
		/*}*/

	} else {
		BOOST_LOG_TRIVIAL(error) << "Could not login: " << sp_error_message(error);
		sp_session_release(spotify.session);
		exit(EXIT_FAILURE);
	}
}

void session_notify_main_thread(sp_session *session) {
	spotify.mutex.lock();
	spotify.notify = true;
	spotify.condition.notify_all();
	spotify.mutex.unlock();
}
