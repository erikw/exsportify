#include "spotify.h"

#include "debug.h"

class Spotify *spotify;

Spotify::~Spotify() {
	if (this->pl_container != NULL) {
		if (sp_playlistcontainer_release(this->pl_container) !=
		    SP_ERROR_OK) {
			logt(error) << "Could not release playlistcontainer.";
			exit(EXIT_FAILURE);
		}
	}
	if (sp_session_release(this->session) != SP_ERROR_OK) {
		logt(error) << "Could not release spotify session.";
			exit(EXIT_FAILURE);
	}
}
