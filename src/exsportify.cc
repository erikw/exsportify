#include <iostream>
#include <string>
#include <cstdlib>

#include <unistd.h>
#include <termios.h>

#include <boost/program_options.hpp>

#include "exsportify.h"
#include "debug.h"
#include "spotify/spotify.h"

namespace bpo = boost::program_options;
using namespace spotify;


static const char *USAGE_DESC = "Make a backup of all your playlists.";

static void parse_args(int argc, const char *argv[], std::string &username,
		std::string password, bool &store_session, bool &load_session) {
	bpo::options_description desc("Possible options. Either log in always or"
			" store a login session for later reuse. Typically you "
			"will log in once and reuse the session on future "
			"(scripted) runs.");
	desc.add_options()
		("help,h", "Show help message.")
		("version,v", "Print version and exit.")
		("store-session,s", "Store the login session.")
		("load-session,l", "Login using stored session.")
		("username,u", bpo::value<std::string>(&username),
		 	 "Spotify/Facebook username")
		("password,p", bpo::value<std::string>(&password),
		 	 "Your password. Can be omitted.")
		;

	bpo::variables_map vmap;
	try {
		bpo::store(bpo::parse_command_line(argc, argv, desc), vmap);
		if (vmap.count("help")) {
			std::cout << USAGE_DESC << std::endl;
			std::cout << desc;
			exit(EXIT_SUCCESS);
		}
		bpo::notify(vmap); // Throw errors if there are any.

		if (vmap.count("version")) {
			std::cout << PROGRAM_NAME << " v" <<
				EXSPORTIFY_VERSION_MAJOR << "." <<
				EXSPORTIFY_VERSION_MINOR << std::endl;
			exit(EXIT_SUCCESS);
		}

		if (vmap.count("store-session")) {
			store_session = true;
		}

		if (vmap.count("load-session")) {
			load_session = true;
		}

		if (!load_session) {
			if (!vmap.count("username")) {
				std::cout << "username: ";
				std::getline(std::cin, username);
			}

			if (!vmap.count("password")) {
				std::cout << "password: ";
				termios termattr_old;
				tcgetattr(STDIN_FILENO, &termattr_old);
				termios termattr_new = termattr_old;
				termattr_new.c_lflag &= ~ECHO;
				tcsetattr(STDIN_FILENO, TCSANOW, &termattr_new);
				//std::cin >> password;
				std::getline(std::cin, password);
				tcsetattr(STDIN_FILENO, TCSANOW, &termattr_old);
				std::cout << std::endl;
			}
		}
	} catch (bpo::error &err) {
		std::cerr << "Command parsing failed:" << err.what() << "." <<
			std::endl;
	}
}

static void init(void) {
#ifdef DEBUG
	boost::log::core::get()->set_filter (
    			boost::log::trivial::severity >= boost::log::trivial::trace
    			);
#else
	boost::log::core::get()->set_filter (
    			boost::log::trivial::severity >= boost::log::trivial::error
    			);
#endif
}

int main(int argc, const char *argv[]) {
	std::string username;
	std::string password;
	bool store_session = false;
	bool load_session = false;

	init();
	BOOST_LOG_TRIVIAL(trace) << "Reading command line arguments.";
	parse_args(argc, argv, username, password, store_session, load_session);

	Spotify *sp = NULL;
	if (load_session) {
		*sp = Spotify();
	} else {
		*sp = Spotify(username, password);

	}
	sp->print();

	return EXIT_SUCCESS;
}
