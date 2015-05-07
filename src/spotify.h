#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <libspotify/api.h>


struct Spotify {
	boost::mutex mutex;
	boost::condition_variable condition;
	bool notify;

	bool is_logged_in;
	sp_session *session = NULL;
	sp_playlistcontainer *playlistcontainer = NULL;
};

extern struct Spotify spotify;


#endif
