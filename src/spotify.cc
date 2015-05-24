#include "spotify.h"

#include "debug.h"

class Spotify *spotify;

Spotify::~Spotify() {
	if (this->playlistcontainer != NULL) {
		if (sp_playlistcontainer_release(this->playlistcontainer) !=
		    SP_ERROR_OK) {
			BOOST_LOG_TRIVIAL(error)
			    << "Could not release playlistcontainer.";
			exit(EXIT_FAILURE);
		}
	}
	if (sp_session_release(this->session) != SP_ERROR_OK) {
		BOOST_LOG_TRIVIAL(error) << "Could not release spotify session.";
			exit(EXIT_FAILURE);
	}
}
