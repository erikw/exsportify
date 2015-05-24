#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <libspotify/api.h>


class Spotify {
public:
	virtual ~Spotify();

	boost::mutex mutex;
	boost::condition_variable condition;
	bool notify;

	bool is_logged_in;	// When we are considered logged in.
	bool has_logged_out;	// When we're truly logged out.
	bool all_loaded;
	sp_session *session = NULL;
	sp_playlistcontainer *playlistcontainer = NULL;
};

extern class Spotify *spotify;


#endif
