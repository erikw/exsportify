#include <iostream>
#include <string>

#include <boost/program_options.hpp>
//#include <libspotify/api.h>

//using namespace std;
namespace bpo = boost::program_options;


static const char *USAGE_DESC = "dfdf";

int main(int argc, const char *argv[])
{
	bpo::options_description desc("Possible options. If login credentials"
		"are not given we will try to log you in from stored a"
		"stored session.");
	desc.add_options()
		("help,h", "Show help message.")
		("store-session,s", "Store the login session.")
		("username,u", bpo::value<std::string>(), "Spotify/Facebook username")
		("password,p", bpo::value<std::string>(), "Your password. Can be omitted.")
		;

	bpo::variables_map vmap;

	try {
		bpo::store(bpo::parse_command_line(argc, argv, desc), vmap);
		if (vmap.count("help")) {
			std::cout << USAGE_DESC << std::endl;
		}
	} catch (bpo::error &err) {
		std::cerr << "Command parsing failed:" << err.what() << "." <<
			std::endl;
	}

	return 0;
}
