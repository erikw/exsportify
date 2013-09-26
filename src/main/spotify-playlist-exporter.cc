#include <iostream>
#include <string>

#include <boost/program_options.hpp> 
#include <libspotify/api.h>

//using namespace std;
namespace bpo = boost::program_options;


int main(int argc, const char *argv[])
{
	bpo::options_description desc("Possible options. If login credentials"
		"are not given we will try to log you in from stored a"
		"stored session.");
	desc.add_options()
		("help,h", "Show help message.")
		("store-session,s", "Store the login session.")
		("username,u", bpo::value<std:::string>(), "Spotify/Facebook username")
		("password,p", bpo::value<std:::string>(), "Your password. Can be omitted.")
		;

	return 0;
}
