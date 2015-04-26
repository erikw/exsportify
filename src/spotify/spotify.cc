#include <iostream>

//#include <libspotify/api.h>

#include "debug.h"
#include "spotify/spotify.h"

namespace spotify {

	Spotify::Spotify() {
		BOOST_LOG_TRIVIAL(debug) << "in normal constructor";
	}

	Spotify::Spotify(std::string username, std::string password) {
		BOOST_LOG_TRIVIAL(debug)
		    << "in passwd const constructor username = " << username <<
		    ", password =  " << password;
	}

	void spotify::Spotify::print() {
		std::cout << "hi" << std::endl;
	}

}
