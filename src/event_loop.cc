#include <string>

#include "debug.h"
#include "event_loop.h"
#include "spotify.h"


void wait_for_notify_events(int &next_timeout) {
	boost::unique_lock<boost::mutex> lock(spotify.mutex);

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
}

void event_loop::operator()() {
	int next_timeout = 0;
	bool process_events = true;


	BOOST_LOG_TRIVIAL(trace) << "In event_loop";
	while (process_events) {
		wait_for_notify_events(next_timeout);
	}
}
