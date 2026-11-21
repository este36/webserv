#include "CGI/CGIHandler.hpp"
#include "HttpConnexion.hpp"
#include "LocationConfig.hpp"


//returne un pointeur LocationConfig, si elle existe, o NULL dans le cas contraire 
const LocationConfig *find_location(const ServerConfig &serverConfig, const std::string &uri_path)
{
	const LocationConfig 	*match 		= NULL;
	size_t 					long_max 	= 0;

	for (size_t i = 0; i < serverConfig.location.size(); i++)
	{
		const LocationConfig	&current_loc 	= serverConfig.location[i];
		const std::string 		&loc_path 		= current_loc.path;

		if (uri_path.length() >= loc_path.length() && 
			uri_path.substr(0, loc_path.length()) == loc_path)
			{
				if (loc_path.length() > long_max)				//Si le path est encore plus specifique (plus long...), encore mieux.
				{
					match 		= &current_loc;
					long_max 	= loc_path.length();
				}
			}
	}

	return match;
}

