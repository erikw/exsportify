#include <string>
#include <stdlib.h>

#include "debug.h"
#include "event_loop.h"
#include "spotify.h"

static void process_libspotify_events(int *next_timeout) {
	do {
		sp_session_process_events(spotify.session, next_timeout);
	} while (next_timeout == 0);
}

static void start_logout() {
	spotify.is_logged_in = false;
	if (sp_session_logout(spotify.session) == SP_ERROR_OK) {
		BOOST_LOG_TRIVIAL(trace) << "Logout started.";
	} else {
		BOOST_LOG_TRIVIAL(error) << "Failed to start Logout.";
		sp_session_release(spotify.session);
		exit(EXIT_FAILURE);
	}
}

void event_loop::operator()() {
	int next_timeout = 0;
	bool process_events = true;


	BOOST_LOG_TRIVIAL(trace) << "In event_loop";
	boost::unique_lock<boost::mutex> lock(spotify.mutex);
	int i = 0;
	while (process_events) {
		if (next_timeout == 0) {
			while (!spotify.notify)
				spotify.condition.wait(lock);
		} else {
			boost::system_time const timeout =
				boost::get_system_time() +
				boost::posix_time::milliseconds(next_timeout);
			while (!spotify.notify) {
				if (!spotify.condition.timed_wait(lock, timeout)) {
					break;
				}
			}
		}


		// Program Work
		if (spotify.has_logged_out) {
			process_events = false;
		} else if (i++ == 10) {
			start_logout();
		} else if (spotify.is_logged_in) {
			BOOST_LOG_TRIVIAL(trace) << "Working... " << i;
		}


		// Process libspotify events.
		spotify.notify = false;
		lock.unlock();
		process_libspotify_events(&next_timeout);
		lock = boost::unique_lock<boost::mutex>(spotify.mutex);
	}
	sp_session_release(spotify.session); // TODO move to ~Spotify()?
}
