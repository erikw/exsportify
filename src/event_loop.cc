#include <string>

#include "debug.h"
#include "event_loop.h"
#include "spotify.h"

static void process_libspotify_events(int *next_timeout) {
	do {
		sp_session_process_events(spotify.session, next_timeout);
	} while (next_timeout == 0);
}

void event_loop::operator()() {
	int next_timeout = 0;
	bool process_events = true;


	BOOST_LOG_TRIVIAL(trace) << "In event_loop";
	boost::unique_lock<boost::mutex> lock(spotify.mutex);
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
		BOOST_LOG_TRIVIAL(trace) << "Returned from wait...";
		spotify.notify = false;
		lock.unlock();
		process_libspotify_events(&next_timeout);
		lock = boost::unique_lock<boost::mutex>(spotify.mutex);
		BOOST_LOG_TRIVIAL(trace) << "timeout=" << next_timeout;
	}
}
