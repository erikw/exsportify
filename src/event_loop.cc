#include "debug.h"

#include "event_loop.h"

void event_loop::operator()() {
	BOOST_LOG_TRIVIAL(trace) << "In event_loop";

}
