#include <stdlib.h>

#include <libspotify/api.h>

#include "session.h"
#include "debug.h"
#include "spotify.h"

sp_session_callbacks session_callbacks = {
	session_logged_in,
	session_logged_out,
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
		logt(trace) << "Logged in as user " << sp_session_user_name(session);
		spotify->is_logged_in = true;
		spotify->pl_container =  sp_session_playlistcontainer(spotify->session);
	} else {
		logt(error) << "Could not login: " << sp_error_message(error);
		sp_session_release(spotify->session);
		exit(EXIT_FAILURE);
	}
}

void session_logged_out(sp_session *session)
{
	(void) session;
	logt(trace) << "Has logged out.";
	spotify->has_logged_out = true;
}

void session_notify_main_thread(sp_session *session) {
	(void) session;
	spotify->mutex.lock();
	spotify->notify = true;
	spotify->condition.notify_all();
	spotify->mutex.unlock();
}
