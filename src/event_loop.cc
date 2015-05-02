#include <string>

#include "debug.h"
#include "event_loop.h"

void event_loop::operator()() {
	BOOST_LOG_TRIVIAL(trace) << "In event_loop";

	//if (spotify_init(username, password, store_session, load_session) != 0) {
		//DEBUG_PRINTF("Spotify failed to initialize\n");
		//exit(-1);
	//}
}
